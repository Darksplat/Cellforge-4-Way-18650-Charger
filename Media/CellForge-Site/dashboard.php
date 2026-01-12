<?php
require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

$pageTitle = 'Dashboard';

$userId = $_SESSION['user_id'] ?? null;
$pdo = getPdo();

/*
|--------------------------------------------------------------------------
| Dashboard metrics
|--------------------------------------------------------------------------
*/

// Chargers online (checked in within last 5 minutes)
$chargersOnline = 0;
$stmt = $pdo->prepare("
  SELECT COUNT(*)
  FROM chargers
  WHERE user_id = ?
    AND last_checkin >= (NOW() - INTERVAL 5 MINUTE)
");
$stmt->execute([$userId]);
$chargersOnline = (int) $stmt->fetchColumn();

// Active runs
$activeRuns = 0;
$stmt = $pdo->prepare("
  SELECT COUNT(*)
  FROM runs
  WHERE user_id = ?
    AND status = 'RUNNING'
");
$stmt->execute([$userId]);
$activeRuns = (int) $stmt->fetchColumn();

// Cells tracked
$cellsTracked = 0;
$stmt = $pdo->prepare("
  SELECT COUNT(*)
  FROM cells
  WHERE user_id = ?
");
$stmt->execute([$userId]);
$cellsTracked = (int) $stmt->fetchColumn();

// Last activity
$lastActivity = null;
$stmt = $pdo->prepare("
  SELECT MAX(start_time)
  FROM runs
  WHERE user_id = ?
");
$stmt->execute([$userId]);
$lastActivity = $stmt->fetchColumn();

/*
|--------------------------------------------------------------------------
| Render
|--------------------------------------------------------------------------
*/
require __DIR__ . '/templates/header.php';
?>

<section>
  <h1 class="hero-title">
    CellForge <span>Dashboard</span>
  </h1>

  <p class="hero-text">
    System overview and recent activity.
  </p>

  <div class="right-panel">
    <div class="right-heading">System Status</div>

    <div class="status-badges">
      <div class="status-pill">
        <strong><?= $chargersOnline ?></strong>
        <span>chargers online</span>
      </div>
      <div class="status-pill">
        <strong><?= $activeRuns ?></strong>
        <span>active runs</span>
      </div>
      <div class="status-pill">
        <strong><?= $cellsTracked ?></strong>
        <span>cells tracked</span>
      </div>
    </div>

    <div class="mini-grid">
      <div class="mini-card">
        <div class="mini-label">Chargers online</div>
        <div class="mini-value"><?= $chargersOnline ?></div>
        <div class="mini-sub">last 5 minutes</div>
      </div>

      <div class="mini-card">
        <div class="mini-label">Active runs</div>
        <div class="mini-value"><?= $activeRuns ?></div>
        <div class="mini-sub">currently running</div>
      </div>

      <div class="mini-card">
        <div class="mini-label">Cells tracked</div>
        <div class="mini-value"><?= $cellsTracked ?></div>
        <div class="mini-sub">inventory size</div>
      </div>

      <div class="mini-card">
        <div class="mini-label">Last activity</div>
        <div class="mini-value">
          <?= $lastActivity ? date('H:i', strtotime($lastActivity)) : '—' ?>
        </div>
        <div class="mini-sub">
          <?= $lastActivity ? date('d M Y', strtotime($lastActivity)) : 'no runs yet' ?>
        </div>
      </div>
    </div>
  </div>
</section>

<?php
require __DIR__ . '/templates/footer.php';
