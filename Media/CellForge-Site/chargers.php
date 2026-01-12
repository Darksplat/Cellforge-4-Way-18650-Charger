<?php
require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

$pageTitle = 'Chargers';

$userId = $_SESSION['user_id'] ?? null;
$pdo = getPdo();

/*
|--------------------------------------------------------------------------
| Fetch chargers (owned OR unclaimed)
|--------------------------------------------------------------------------
*/
$stmt = $pdo->prepare("
  SELECT
    id,
    user_id,
    name,
    hardware_type,
    firmware_version,
    last_checkin,
    connection_state,
    wifi_rssi
  FROM chargers
  WHERE user_id = ? OR user_id IS NULL
  ORDER BY
    user_id IS NULL DESC,
    name ASC
");
$stmt->execute([$userId]);
$chargers = $stmt->fetchAll(PDO::FETCH_ASSOC);

require __DIR__ . '/templates/header.php';
?>

<style>
/* ===== Traffic light connection indicator ===== */
.traffic {
  width: 14px;
  height: 14px;
  border-radius: 50%;
  display: inline-block;
  box-shadow: 0 0 4px rgba(0,0,0,0.6);
}

.traffic.disconnected {
  background: #ff4b4b;
}

.traffic.connecting {
  background: #ffb347;
  animation: pulse 1s infinite;
}

.traffic.connected {
  background: #4cd964;
}

@keyframes pulse {
  0%   { opacity: 0.35; }
  50%  { opacity: 1; }
  100% { opacity: 0.35; }
}

/* ===== RSSI signal bars ===== */
.signal {
  display: inline-flex;
  align-items: flex-end;
  gap: 2px;
  height: 14px;
}

.signal span {
  width: 3px;
  border-radius: 1px;
  background: rgba(255,255,255,0.25);
}

.signal.good span { background: #4cd964; }
.signal.ok span   { background: #ffb347; }
.signal.poor span { background: #ff4b4b; }

.signal span:nth-child(1) { height: 4px; }
.signal span:nth-child(2) { height: 7px; }
.signal span:nth-child(3) { height: 10px; }
.signal span:nth-child(4) { height: 13px; }
</style>

<section>
  <h1 class="hero-title">
    Chargers <span>Overview</span>
  </h1>

  <p class="hero-text">
    Registered battery chargers. Unclaimed chargers can be claimed to your account.
  </p>

  <table class="cells-table">
    <thead>
      <tr>
        <th>Name</th>
        <th>Hardware</th>
        <th>Firmware</th>
        <th>Last seen</th>
        <th>Status</th>
        <th>Signal</th>
        <th></th>
      </tr>
    </thead>
    <tbody>

<?php if (empty($chargers)): ?>
      <tr>
        <td colspan="7" class="text-muted">
          No chargers found.
        </td>
      </tr>
<?php else: ?>

<?php foreach ($chargers as $charger): ?>

<?php
  $isUnclaimed = empty($charger['user_id']);
  $state = $charger['connection_state'] ?? 'DISCONNECTED';
  $stateClass = strtolower($state);
?>

      <tr>
        <td>
          <?= htmlspecialchars($charger['name'], ENT_QUOTES, 'UTF-8') ?>
<?php if ($isUnclaimed): ?>
          <span class="cell-sub">unclaimed</span>
<?php endif; ?>
        </td>

        <td>
          <?= htmlspecialchars($charger['hardware_type'] ?? '—', ENT_QUOTES, 'UTF-8') ?>
        </td>

        <td>
          <?= htmlspecialchars($charger['firmware_version'] ?? '—', ENT_QUOTES, 'UTF-8') ?>
        </td>

        <td>
<?php if ($charger['last_checkin']): ?>
          <span
            class="local-time"
            data-raw="<?= htmlspecialchars($charger['last_checkin'], ENT_QUOTES, 'UTF-8') ?>"
          >
            <?= htmlspecialchars($charger['last_checkin'], ENT_QUOTES, 'UTF-8') ?>
          </span>
<?php else: ?>
          —
<?php endif; ?>
        </td>

        <td>
          <span
            class="traffic <?= $stateClass ?>"
            title="<?= htmlspecialchars($state, ENT_QUOTES, 'UTF-8') ?>"
          ></span>
        </td>

        <td>
<?php
$rssi = $charger['wifi_rssi'];

if ($rssi === null) {
    echo '—';
} else {
    if ($rssi >= -60) {
        $class = 'good';
        $bars  = 4;
    } elseif ($rssi >= -75) {
        $class = 'ok';
        $bars  = 3;
    } else {
        $class = 'poor';
        $bars  = 2;
    }

    echo '<div class="signal ' . $class . '" title="RSSI ' . (int)$rssi . ' dBm">';
    for ($i = 0; $i < 4; $i++) {
        echo '<span style="opacity:' . ($i < $bars ? '1' : '0.2') . '"></span>';
    }
    echo '</div>';
}
?>
        </td>

        <td>
<?php if ($isUnclaimed): ?>
          <form method="post" action="chargers_claim.php">
            <input type="hidden" name="charger_id" value="<?= (int)$charger['id'] ?>">
            <button class="btn btn-ghost" type="submit">Claim</button>
          </form>
<?php else: ?>
          —
<?php endif; ?>
        </td>
      </tr>

<?php endforeach; ?>

<?php endif; ?>

    </tbody>
  </table>
</section>

<script>
document.addEventListener('DOMContentLoaded', () => {
  document.querySelectorAll('.local-time').forEach(el => {
    const raw = el.dataset.raw;
    if (!raw) return;

    const date = new Date(raw.replace(' ', 'T'));

    el.textContent = date.toLocaleString(undefined, {
      year: 'numeric',
      month: 'short',
      day: '2-digit',
      hour: '2-digit',
      minute: '2-digit',
      hour12: false
    });
  });
});
</script>

<?php
require __DIR__ . '/templates/footer.php';
