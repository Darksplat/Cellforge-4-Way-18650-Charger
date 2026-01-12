<?php
// /api/ingest.php
// Main ingest endpoint for CellForge chargers.

declare(strict_types=1);

header('Content-Type: application/json');

require_once __DIR__ . '/../app/bootstrap.php';
require_once __DIR__ . '/../app/db.php';

/*
|--------------------------------------------------------------------------
| Obtain PDO
|--------------------------------------------------------------------------
*/
$pdo = getPdo();

/*
|--------------------------------------------------------------------------
| Authenticate charger using API token
|--------------------------------------------------------------------------
*/
function authenticateCharger(PDO $pdo): array
{
    $apiToken = $_SERVER['HTTP_AUTHORIZATION'] ?? '';

    if ($apiToken === '') {
        http_response_code(401);
        echo json_encode(['error' => 'Missing API token']);
        exit;
    }

    $stmt = $pdo->prepare("
        SELECT id, user_id, firmware_version
        FROM chargers
        WHERE api_token = ?
        LIMIT 1
    ");
    $stmt->execute([$apiToken]);

    $charger = $stmt->fetch(PDO::FETCH_ASSOC);

    if (!$charger) {
        http_response_code(401);
        echo json_encode(['error' => 'Invalid API token']);
        exit;
    }

    return $charger;
}

/*
|--------------------------------------------------------------------------
| Helper: Find or create cell by barcode
|--------------------------------------------------------------------------
*/
function findOrCreateCell(PDO $pdo, int $userId, string $barcode): int
{
    $stmt = $pdo->prepare("
        SELECT id
        FROM cells
        WHERE barcode = ? AND user_id = ?
        LIMIT 1
    ");
    $stmt->execute([$barcode, $userId]);

    $cell = $stmt->fetch(PDO::FETCH_ASSOC);

    if ($cell) {
        return (int)$cell['id'];
    }

    $stmt = $pdo->prepare("
        INSERT INTO cells (user_id, barcode)
        VALUES (?, ?)
    ");
    $stmt->execute([$userId, $barcode]);

    return (int)$pdo->lastInsertId();
}

/*
|--------------------------------------------------------------------------
| Parse JSON input
|--------------------------------------------------------------------------
*/
$input = json_decode(file_get_contents('php://input'), true);

if (!is_array($input)) {
    http_response_code(400);
    echo json_encode(['error' => 'Invalid JSON']);
    exit;
}

/*
|--------------------------------------------------------------------------
| Authenticate charger
|--------------------------------------------------------------------------
*/
$charger = authenticateCharger($pdo);

$event = $input['event'] ?? null;

if (!$event) {
    http_response_code(400);
    echo json_encode(['error' => 'Missing event']);
    exit;
}

/*
|--------------------------------------------------------------------------
| Event handling
|--------------------------------------------------------------------------
*/
switch ($event) {

    /*
    |--------------------------------------------------------------------------
    | PING — heartbeat / Wi-Fi status
    |--------------------------------------------------------------------------
    */
    case 'PING':

        $state = $input['state'] ?? 'CONNECTED';
        $rssi  = isset($input['rssi']) ? (int)$input['rssi'] : null;

        $stmt = $pdo->prepare("
            UPDATE chargers
            SET
                connection_state = ?,
                last_checkin = NOW(),
                wifi_rssi = ?
            WHERE id = ?
            LIMIT 1
        ");

        $stmt->execute([
            $state,
            $rssi,
            (int)$charger['id']
        ]);

        echo json_encode(['ok' => true]);
        break;

    /*
    |--------------------------------------------------------------------------
    | RUN_START
    |--------------------------------------------------------------------------
    */
    case 'RUN_START':

        if (empty($input['barcode'])) {
            http_response_code(400);
            echo json_encode(['error' => 'Missing barcode']);
            exit;
        }

        $barcode = trim((string)$input['barcode']);
        $cellId  = findOrCreateCell($pdo, (int)$charger['user_id'], $barcode);

        $slotLabel = $input['slot_label'] ?? 'UNKNOWN';
        $timestamp = $input['timestamp'] ?? date('Y-m-d H:i:s');

        $stmt = $pdo->prepare("
            INSERT INTO runs (
                user_id,
                cell_id,
                charger_id,
                slot_label,
                status,
                start_time,
                firmware_version
            ) VALUES (?, ?, ?, ?, 'RUNNING', ?, ?)
        ");

        $stmt->execute([
            (int)$charger['user_id'],
            $cellId,
            (int)$charger['id'],
            $slotLabel,
            $timestamp,
            $charger['firmware_version']
        ]);

        echo json_encode([
            'status' => 'run_started',
            'run_id' => (int)$pdo->lastInsertId()
        ]);
        break;

    /*
    |--------------------------------------------------------------------------
    | IR_RESULT
    |--------------------------------------------------------------------------
    */
    case 'IR_RESULT':

        if (empty($input['run_id']) || empty($input['data'])) {
            http_response_code(400);
            echo json_encode(['error' => 'Missing run_id or data']);
            exit;
        }

        $data = $input['data'];

        $stmt = $pdo->prepare("
            UPDATE runs SET
                internal_resistance_mohm = ?,
                ir_test_current_ma = 500,
                ir_voltage_open_v = ?,
                ir_voltage_load_v = ?,
                ir_temperature_c = ?,
                ir_timestamp = NOW()
            WHERE id = ?
        ");

        $stmt->execute([
            $data['internal_resistance_mohm'] ?? null,
            $data['v_open'] ?? null,
            $data['v_load'] ?? null,
            $data['temperature_c'] ?? null,
            (int)$input['run_id']
        ]);

        echo json_encode(['status' => 'ir_recorded']);
        break;

    /*
    |--------------------------------------------------------------------------
    | RUN_COMPLETE
    |--------------------------------------------------------------------------
    */
    case 'RUN_COMPLETE':

        if (empty($input['run_id']) || empty($input['data'])) {
            http_response_code(400);
            echo json_encode(['error' => 'Missing run_id or data']);
            exit;
        }

        $data = $input['data'];

        $stmt = $pdo->prepare("
            UPDATE runs SET
                capacity_mah = ?,
                energy_wh = ?,
                status = 'COMPLETE',
                end_time = NOW()
            WHERE id = ?
        ");

        $stmt->execute([
            $data['capacity_mah'] ?? null,
            $data['energy_wh'] ?? null,
            (int)$input['run_id']
        ]);

        echo json_encode(['status' => 'run_complete']);
        break;

    /*
    |--------------------------------------------------------------------------
    | RUN_ERROR
    |--------------------------------------------------------------------------
    */
    case 'RUN_ERROR':

        if (empty($input['run_id'])) {
            http_response_code(400);
            echo json_encode(['error' => 'Missing run_id']);
            exit;
        }

        $stmt = $pdo->prepare("
            UPDATE runs SET
                status = 'ERROR',
                error_code = ?
            WHERE id = ?
        ");

        $stmt->execute([
            $input['error_code'] ?? 'UNKNOWN',
            (int)$input['run_id']
        ]);

        echo json_encode(['status' => 'run_error_recorded']);
        break;

    /*
    |--------------------------------------------------------------------------
    | Unknown event
    |--------------------------------------------------------------------------
    */
    default:
        http_response_code(400);
        echo json_encode(['error' => 'Unknown event type']);
        break;
}
