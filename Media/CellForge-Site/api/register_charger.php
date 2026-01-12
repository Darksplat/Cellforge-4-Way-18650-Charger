<?php
declare(strict_types=1);

header('Content-Type: application/json');

require_once __DIR__ . '/../app/bootstrap.php';
require_once __DIR__ . '/../app/db.php';

$input = json_decode(file_get_contents('php://input'), true);

if (!is_array($input)) {
    http_response_code(400);
    echo json_encode(['error' => 'Invalid JSON']);
    exit;
}

$hardwareType = trim($input['hardware_type'] ?? '');
$serialNumber = trim($input['serial_number'] ?? '');
$firmware     = trim($input['firmware_version'] ?? null);

if ($hardwareType === '' || $serialNumber === '') {
    http_response_code(400);
    echo json_encode(['error' => 'Missing hardware_type or serial_number']);
    exit;
}

$pdo = getPdo();

// Look for existing charger
$stmt = $pdo->prepare("
    SELECT id, api_token
    FROM chargers
    WHERE serial_number = ?
    LIMIT 1
");
$stmt->execute([$serialNumber]);
$charger = $stmt->fetch(PDO::FETCH_ASSOC);

if ($charger) {
    // Update metadata
    $pdo->prepare("
        UPDATE chargers
        SET firmware_version = ?, last_checkin = NOW()
        WHERE id = ?
    ")->execute([$firmware, $charger['id']]);

    echo json_encode([
        'api_token'  => $charger['api_token'],
        'charger_id' => (int)$charger['id']
    ]);
    exit;
}

// Create new charger
$apiToken = bin2hex(random_bytes(32));

$stmt = $pdo->prepare("
    INSERT INTO chargers (
        user_id,
        name,
        hardware_type,
        serial_number,
        api_token,
        firmware_version,
        last_checkin
    ) VALUES (
        NULL,
        ?,
        ?,
        ?,
        ?,
        ?,
        NOW()
    )
");

$stmt->execute([
    'Unclaimed Charger',
    $hardwareType,
    $serialNumber,
    $apiToken,
    $firmware
]);

echo json_encode([
    'api_token'  => $apiToken,
    'charger_id' => (int)$pdo->lastInsertId()
]);
