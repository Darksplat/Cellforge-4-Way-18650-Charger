<?php
require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

$pageTitle = 'Cells';

$userId = $_SESSION['user_id'] ?? null;
$pdo = getPdo();

/*
|--------------------------------------------------------------------------
| Fetch cells with latest run status
|--------------------------------------------------------------------------
*/
$stmt = $pdo->prepare("
  SELECT
    c.id,
    c.barcode,
    c.brand,
    c.model,
    c.nominal_capacity_mah,
    c.photo_url,

    r.status AS last_status

  FROM cells c

  LEFT JOIN runs r
    ON r.id = (
      SELECT r2.id
      FROM runs r2
      WHERE r2.cell_id = c.id
      ORDER BY r2.start_time DESC
      LIMIT 1
    )

  WHERE c.user_id = ?
  ORDER BY c.created_at DESC
");
$stmt->execute([$userId]);
$cells = $stmt->fetchAll(PDO::FETCH_ASSOC);

require __DIR__ . '/templates/header.php';
?>

<section class="cells-wrap">
  <h1 class="hero-title">
    Cells <span>Inventory</span>
  </h1>

  <p class="hero-text">
    Battery cells currently tracked in your inventory.
  </p>

  <table class="cells-table">
 <thead>
  <tr>
    <th></th>
    <th>Barcode</th>
    <th>Brand / Model</th>
    <th>Capacity</th>
    <th>Status</th>
  </tr>
</thead>
    <tbody>

<?php if (empty($cells)): ?>
      <tr>
        <td colspan="5" class="text-muted">
          No cells have been added yet.
        </td>
      </tr>
<?php else: ?>

<?php foreach ($cells as $cell): ?>
      <tr>
        <td>
          <div class="cell-thumb">
<?php if (!empty($cell['photo_url'])): ?>
            <img src="<?= htmlspecialchars($cell['photo_url'], ENT_QUOTES, 'UTF-8') ?>" alt="">
<?php else: ?>
            —
<?php endif; ?>
          </div>
        </td>

        <td>
          <a class="cell-id" href="#">
            <?= htmlspecialchars($cell['barcode'] ?: '—', ENT_QUOTES, 'UTF-8') ?>
          </a>
        </td>

        <td>
          <?= htmlspecialchars(trim(($cell['brand'] ?? '') . ' ' . ($cell['model'] ?? '')), ENT_QUOTES, 'UTF-8') ?: '—' ?>
        </td>

        <td>
          <?= $cell['nominal_capacity_mah']
            ? (int)$cell['nominal_capacity_mah'] . ' mAh'
            : '—'
          ?>
        </td>

        <td>
<?php
  $status = $cell['last_status'] ?? 'NEW';
  echo htmlspecialchars($status, ENT_QUOTES, 'UTF-8');
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
