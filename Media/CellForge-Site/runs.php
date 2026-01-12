<?php
require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

$pageTitle = 'Runs';

$userId = $_SESSION['user_id'] ?? null;
$pdo = getPdo();

/*
|--------------------------------------------------------------------------
| Fetch runs with cell + charger context
|--------------------------------------------------------------------------
*/
$stmt = $pdo->prepare("
  SELECT
    r.id,
    r.start_time,
    r.status,
    r.slot_label,
    r.capacity_mah,
    r.energy_wh,

    c.barcode AS cell_barcode,
    ch.name AS charger_name

  FROM runs r
  INNER JOIN cells c ON c.id = r.cell_id
  INNER JOIN chargers ch ON ch.id = r.charger_id

  WHERE r.user_id = ?
  ORDER BY r.start_time DESC
");
$stmt->execute([$userId]);
$runs = $stmt->fetchAll(PDO::FETCH_ASSOC);

require __DIR__ . '/templates/header.php';
?>

<section>
  <h1 class="hero-title">
    Runs <span>History</span>
  </h1>

  <p class="hero-text">
    Historical charge and discharge test runs.
  </p>

  <table class="cells-table">
    <thead>
      <tr>
        <th>Date</th>
        <th>Barcode</th>
        <th>Charger</th>
        <th>Slot</th>
        <th>Status</th>
        <th>Capacity</th>
        <th>Energy</th>
      </tr>
    </thead>
    <tbody>

<?php if (empty($runs)): ?>
      <tr>
        <td colspan="7" class="text-muted">
          No runs have been recorded yet.
        </td>
      </tr>
<?php else: ?>

<?php foreach ($runs as $run): ?>
      <tr>
        <td>
          <?= date('d M Y H:i', strtotime($run['start_time'])) ?>
        </td>

        <td>
          <span class="cell-id">
            <?= htmlspecialchars($run['cell_barcode'], ENT_QUOTES, 'UTF-8') ?>
          </span>
        </td>

        <td>
          <?= htmlspecialchars($run['charger_name'], ENT_QUOTES, 'UTF-8') ?>
        </td>

        <td>
          <?= htmlspecialchars($run['slot_label'], ENT_QUOTES, 'UTF-8') ?>
        </td>

        <td>
          <?= htmlspecialchars($run['status'], ENT_QUOTES, 'UTF-8') ?>
        </td>

        <td>
          <?= $run['capacity_mah']
            ? (int)$run['capacity_mah'] . ' mAh'
            : '—'
          ?>
        </td>

        <td>
          <?= $run['energy_wh']
            ? number_format((float)$run['energy_wh'], 2) . ' Wh'
            : '—'
          ?>
        </td>
      </tr>
<?php endforeach; ?>

<?php endif; ?>

    </tbody>
  </table>
</section>

<?php
require __DIR__ . '/templates/footer.php';
