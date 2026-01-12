<?php
/**
 * CellForge — Charger auto-disconnect cron
 *
 * Marks chargers DISCONNECTED if no heartbeat
 * has been received within the timeout window.
 *
 * Uses server-local time (NOW()) to match DB storage.
 */

require_once __DIR__ . '/../app/bootstrap.php';
require_once __DIR__ . '/../app/db.php';

$pdo = getPdo();

// Timeout in seconds
$timeoutSeconds = 90;

$sql = "
  UPDATE chargers
  SET connection_state = 'DISCONNECTED'
  WHERE connection_state != 'DISCONNECTED'
    AND last_checkin < (NOW() - INTERVAL :timeout SECOND)
";

$stmt = $pdo->prepare($sql);
$stmt->execute(['timeout' => $timeoutSeconds]);

// Intentionally silent — no output for cron
