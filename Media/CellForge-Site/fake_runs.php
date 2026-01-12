<?php
// Temporary script to generate fake test runs for development.
// DELETE THIS FILE after use — it's not secured.

require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

// ONLY allow if you're logged in (basic protection)
if (empty($_SESSION['user_id'])) {
    die("You must be logged in.");
}

$userId = (int)$_SESSION['user_id'];

// CONFIGURE HERE
$cellId = 1;               // <--- CHANGE: ID of the cell you want to add runs to
$chargerId = 1;            // <--- ID of your charger, or keep null
$slotNumber = 1;           
$numberOfRuns = 8;         // How many fake results to generate
$minCapacity = 1500;       // Lower bound for random (mAh)
$maxCapacity = 2600;       // Upper bound for random (mAh)

$pdo = getPdo();

echo "<pre>";
echo "Generating $numberOfRuns fake runs for Cell ID $cellId...\n\n";

for ($i = 0; $i < $numberOfRuns; $i++) {

    $capacity = rand($minCapacity, $maxCapacity);
    $energy   = $capacity * 3.6 / 1000.0;  // approximate Wh

    // STEP 1: Insert a START event
    $start = $pdo->prepare("
        INSERT INTO test_runs
        (user_id, charger_id, cell_id, slot_number, mode, status, started_at)
        VALUES
        (:user_id, :charger_id, :cell_id, :slot, 'DISCHARGE', 'RUNNING', NOW())
    ");

    $start->execute([
        'user_id'    => $userId,
        'charger_id' => $chargerId ?: null,
        'cell_id'    => $cellId,
        'slot'       => $slotNumber,
    ]);

    $runId = $pdo->lastInsertId();

    // STEP 2: Mark run COMPLETE
    $finish = $pdo->prepare("
        UPDATE test_runs
        SET
            status = 'COMPLETE',
            ended_at = DATE_ADD(NOW(), INTERVAL 1 HOUR),
            result_capacity_mah = :cap,
            result_energy_wh = :wh
        WHERE id = :id
    ");

    $finish->execute([
        'cap' => $capacity,
        'wh'  => $energy,
        'id'  => $runId,
    ]);

    echo "Run #$runId → $capacity mAh\n";
}

echo "\nDONE\n";
