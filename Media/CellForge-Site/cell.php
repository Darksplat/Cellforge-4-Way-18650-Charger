<?php
require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

// Must be logged in
if (empty($_SESSION['user_id'])) {
    header('Location: login.php');
    exit;
}

$pageTitle = 'CellForge – Cell details';

$userId = (int)$_SESSION['user_id'];
$cellId = (int)($_GET['id'] ?? 0);

if ($cellId <= 0) {
    header('Location: cells.php');
    exit;
}

$pdo = getPdo();

// Fetch cell
$stmt = $pdo->prepare(
    'SELECT *
     FROM cells
     WHERE id = :id AND user_id = :uid
     LIMIT 1'
);

$stmt->execute([
    'id'  => $cellId,
    'uid' => $userId,
]);

$cell = $stmt->fetch();

if (!$cell) {
    header('Location: cells.php');
    exit;
}

require __DIR__ . '/templates/header.php';
?>

<section>

  <!-- PCB-style hero header -->
  <div class="cell-hero">

    <!-- Image placeholder -->
<?php
$model = strtolower(trim((string)($cell['model'] ?? '')));
$model = preg_replace('/[^a-z0-9]+/', '', $model);

$imgUrl = $model
  ? "https://secondlifestorage.com/custom/cell_images/inr18650{$model}.jpg"
  : 'cell_images/placeholder.png';
?>

<div class="cell-hero-image">
  <img
    src="<?= htmlspecialchars($imgUrl, ENT_QUOTES, 'UTF-8') ?>"
    alt="Cell image"
    onerror="this.onerror=null;this.src='cell_images/placeholder.png';"
    style="max-width:100%; max-height:100%; object-fit:contain;"
  >
</div>

<!-- Main identity -->
    <div class="cell-hero-main">
      <h1>
        <?= htmlspecialchars($cell['barcode'] ?: ('Cell #' . $cell['id'])) ?>
      </h1>

      <div class="sub">
        <?= htmlspecialchars($cell['brand'] ?? 'Unknown brand') ?>
        <?php if (!empty($cell['model'])): ?>
          · <?= htmlspecialchars($cell['model']) ?>
        <?php endif; ?>
      </div>

      <div class="sub" style="margin-top:4px;">
        Chemistry:
        <?= htmlspecialchars($cell['chemistry'] ?? '—') ?>
      </div>
    </div>

    <!-- Status / summary -->
    <div class="cell-hero-status">

      <div class="status-box">
        <strong>Nominal capacity</strong>
        <?= $cell['nominal_capacity_mah'] !== null
            ? htmlspecialchars($cell['nominal_capacity_mah']) . ' mAh'
            : '—' ?>
      </div>

      <div class="status-box">
        <strong>Nominal voltage</strong>
        <?= $cell['nominal_voltage'] !== null
            ? htmlspecialchars($cell['nominal_voltage']) . ' V'
            : '—' ?>
      </div>

      <div class="status-box">
        <strong>Status</strong>
        Metadata only
      </div>

    </div>
  </div>

  <!-- Main content -->
  <h2 style="font-size:16px; margin-bottom:6px;">Cell information</h2>

  <div style="
    display:grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap:12px;
    margin-bottom:16px;
  ">

    <div class="mini-card">
      <div class="mini-label">Barcode / ID</div>
      <div class="mini-value">
        <?= htmlspecialchars($cell['barcode'] ?? '—') ?>
      </div>
      <div class="mini-sub">Primary identifier for this cell.</div>
    </div>

    <div class="mini-card">
      <div class="mini-label">Brand / Model</div>
      <div class="mini-value">
        <?= htmlspecialchars($cell['brand'] ?? '—') ?>
        <?php if (!empty($cell['model'])): ?>
          <?= htmlspecialchars(' ' . $cell['model']) ?>
        <?php endif; ?>
      </div>
      <div class="mini-sub">Manufacturer and model designation.</div>
    </div>

    <div class="mini-card">
      <div class="mini-label">Chemistry</div>
      <div class="mini-value">
        <?= htmlspecialchars($cell['chemistry'] ?? '—') ?>
      </div>
      <div class="mini-sub">Declared cell chemistry.</div>
    </div>

    <div class="mini-card">
      <div class="mini-label">Added to inventory</div>
      <div class="mini-value">
        <?= htmlspecialchars($cell['created_at']) ?>
      </div>
      <div class="mini-sub">Timestamp when this cell was recorded.</div>
    </div>

  </div>

  <h2 style="font-size:16px; margin-bottom:6px;">Notes</h2>

  <div class="mini-card" style="margin-bottom:16px;">
<?php if (!empty($cell['notes'])): ?>
    <div style="font-size:13px; line-height:1.4;">
      <?= nl2br(htmlspecialchars($cell['notes'])) ?>
    </div>
<?php else: ?>
    <div class="mini-sub">
      No notes recorded for this cell.
    </div>
<?php endif; ?>
  </div>

  <div class="button-row">
    <a href="cells.php" class="btn btn-ghost">
      ← Back to cells
    </a>
  </div>

</section>

<?php require __DIR__ . '/templates/footer.php'; ?>
