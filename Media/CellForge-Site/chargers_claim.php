<?php
require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

$userId = $_SESSION['user_id'] ?? null;
$chargerId = (int)($_POST['charger_id'] ?? 0);

if (!$userId || !$chargerId) {
    header('Location: chargers.php');
    exit;
}

$pdo = getPdo();

$stmt = $pdo->prepare("
    UPDATE chargers
    SET user_id = ?, name = CONCAT('Charger #', id)
    WHERE id = ?
      AND user_id IS NULL
");

$stmt->execute([$userId, $chargerId]);

header('Location: chargers.php');
exit;
