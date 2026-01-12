<?php
// CellForge API – run_event.php (minimal, verified)

// Always return JSON
header('Content-Type: application/json');

// Only allow POST
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo json_encode([
        'ok' => false,
        'error' => 'POST required'
    ]);
    exit;
}

// Read request body
$raw = file_get_contents('php://input');
if ($raw === false || trim($raw) === '') {
    http_response_code(400);
    echo json_encode([
        'ok' => false,
        'error' => 'Empty request body'
    ]);
    exit;
}

// Decode JSON
$data = json_decode($raw, true);
if (!is_array($data)) {
    http_response_code(400);
    echo json_encode([
        'ok' => false,
        'error' => 'Invalid JSON'
    ]);
    exit;
}

// Validate event
$event = $data['event'] ?? null;
$allowed = ['START', 'DATA', 'END'];

if (!is_string($event) || !in_array($event, $allowed, true)) {
    http_response_code(400);
    echo json_encode([
        'ok' => false,
        'error' => 'Invalid event',
        'allowed' => $allowed
    ]);
    exit;
}

// Append to log (best-effort, never fatal)
$logLine = date('c') . ' ' . json_encode($data, JSON_UNESCAPED_SLASHES) . PHP_EOL;
@file_put_contents(__DIR__ . '/../run_events.log', $logLine, FILE_APPEND);

// Success response
echo json_encode([
    'ok' => true,
    'event' => $event,
    'received_at' => date('c')
]);
