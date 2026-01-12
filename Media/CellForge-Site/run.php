<?php
// run.php - detail view for a single test run and its samples

require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

// Must be logged in
if (empty($_SESSION['user_id'])) {
    header('Location: login.php');
    exit;
}

$pageTitle = 'CellForge - Test run details';

$userId = (int)($_SESSION['user_id'] ?? 0);

// Get run ID from query
$runId = isset($_GET['id']) ? (int)$_GET['id'] : 0;
if ($runId <= 0) {
    http_response_code(400);
    echo 'Invalid run ID.';
    exit;
}

$pdo = getPdo();

$run     = null;
$samples = [];

try {
    $stmt = $pdo->prepare(
        'SELECT
            tr.*,
            ch.name AS charger_name,
            ch.hardware_type,
            c.barcode AS cell_barcode,
            c.brand AS cell_brand,
            c.model AS cell_model
         FROM test_runs tr
         LEFT JOIN chargers ch ON ch.id = tr.charger_id
         LEFT JOIN cells c ON c.id = tr.cell_id
         WHERE tr.id = :id
           AND tr.user_id = :user_id
         LIMIT 1'
    );
    $stmt->execute([
        'id'      => $runId,
        'user_id' => $userId,
    ]);
    $run = $stmt->fetch();

    if ($run) {
        $sStmt = $pdo->prepare(
            'SELECT id, recorded_at, voltage_v, current_a, temperature_c, milliamp_hours, watt_hours
             FROM test_samples
             WHERE test_run_id = :run_id
             ORDER BY recorded_at ASC, id ASC
             LIMIT 500'
        );
        $sStmt->execute(['run_id' => $runId]);
        $samples = $sStmt->fetchAll();
    }
} catch (Throwable $e) {
    $run = null;
}

if (!$run) {
    http_response_code(404);
    echo 'Test run not found.';
    exit;
}

// Prepare simple data for JS chart (voltage vs time)
$chartSamples = [];
if ($samples) {
    foreach ($samples as $s) {
        $chartSamples[] = [
            'recorded_at' => $s['recorded_at'],
            'voltage_v'   => $s['voltage_v'] !== null ? (float)$s['voltage_v'] : null,
        ];
    }
}

require __DIR__ . '/templates/header.php';
?>

        <section>
          <h1 class="hero-title">
            Test run <span>#<?= htmlspecialchars((string)$runId, ENT_QUOTES, 'UTF-8') ?></span>
          </h1>
          <p class="hero-text">
            Detailed view of this test run, including charger, slot, mode, any
            recorded samples sent via the live logging API, and a quick voltage
            graph for a visual sanity check.
          </p>

          <div style="margin-bottom: 16px; font-size: 13px; color: #9a9ab5;">
            <a href="dashboard.php" style="color: #ff7a2a; text-decoration: none;">← Back to dashboard</a>
          </div>

          <div style="display: grid; grid-template-columns: repeat(2, minmax(0, 1fr)); gap: 12px; margin-bottom: 18px;">
            <div style="background: rgba(5,6,8,0.95); border-radius: 12px; padding: 10px 12px; border: 1px solid rgba(255,255,255,0.06); font-size: 13px;">
              <div style="font-size: 11px; text-transform: uppercase; letter-spacing: 0.12em; color: #9a9ab5; margin-bottom: 4px;">
                Run summary
              </div>
              <div><strong>Charger:</strong> <?= htmlspecialchars($run['charger_name'] ?? 'Unknown', ENT_QUOTES, 'UTF-8') ?></div>
              <div><strong>Hardware:</strong> <?= htmlspecialchars($run['hardware_type'] ?? '—', ENT_QUOTES, 'UTF-8') ?></div>
              <div><strong>Slot:</strong> <?= htmlspecialchars((string)$run['slot_number'], ENT_QUOTES, 'UTF-8') ?></div>
              <div><strong>Mode:</strong> <?= htmlspecialchars($run['mode'], ENT_QUOTES, 'UTF-8') ?></div>
              <div><strong>Status:</strong> <?= htmlspecialchars($run['status'], ENT_QUOTES, 'UTF-8') ?></div>
            </div>

            <div style="background: rgba(5,6,8,0.95); border-radius: 12px; padding: 10px 12px; border: 1px solid rgba(255,255,255,0.06); font-size: 13px;">
              <div style="font-size: 11px; text-transform: uppercase; letter-spacing: 0.12em; color: #9a9ab5; margin-bottom: 4px;">
                Results &amp; timing
              </div>
              <div><strong>Started:</strong> <?= htmlspecialchars($run['started_at'], ENT_QUOTES, 'UTF-8') ?></div>
              <div><strong>Ended:</strong> <?= $run['ended_at'] ? htmlspecialchars($run['ended_at'], ENT_QUOTES, 'UTF-8') : '—' ?></div>
              <div><strong>Capacity:</strong>
                <?= $run['result_capacity_mah'] !== null ? htmlspecialchars((string)$run['result_capacity_mah'], ENT_QUOTES, 'UTF-8') . ' mAh' : '—' ?>
              </div>
              <div><strong>Energy:</strong>
                <?= $run['result_energy_wh'] !== null ? htmlspecialchars((string)$run['result_energy_wh'], ENT_QUOTES, 'UTF-8') . ' Wh' : '—' ?>
              </div>
              <div><strong>Error code:</strong> <?= $run['error_code'] ? htmlspecialchars($run['error_code'], ENT_QUOTES, 'UTF-8') : '—' ?></div>
            </div>
          </div>

          <div style="background: rgba(5,6,8,0.95); border-radius: 12px; padding: 10px 12px; border: 1px solid rgba(255,255,255,0.06); font-size: 13px; margin-bottom: 18px;">
            <div style="font-size: 11px; text-transform: uppercase; letter-spacing: 0.12em; color: #9a9ab5; margin-bottom: 4px;">
              Cell information
            </div>
            <div><strong>Barcode:</strong> <?= $run['cell_barcode'] ? htmlspecialchars($run['cell_barcode'], ENT_QUOTES, 'UTF-8') : '—' ?></div>
            <div><strong>Brand:</strong> <?= $run['cell_brand'] ? htmlspecialchars($run['cell_brand'], ENT_QUOTES, 'UTF-8') : '—' ?></div>
            <div><strong>Model:</strong> <?= $run['cell_model'] ? htmlspecialchars($run['cell_model'], ENT_QUOTES, 'UTF-8') : '—' ?></div>
          </div>

<?php if ($samples): ?>
          <div style="background: rgba(5,6,8,0.95); border-radius: 12px; padding: 10px 12px; border: 1px solid rgba(255,255,255,0.06); font-size: 13px; margin-bottom: 18px;">
            <div style="font-size: 11px; text-transform: uppercase; letter-spacing: 0.12em; color: #9a9ab5; margin-bottom: 4px;">
              Voltage graph
            </div>
            <p style="font-size: 12px; color: #9a9ab5; margin-bottom: 8px;">
              Simple voltage-over-time chart derived from the recorded samples. This
              is just a quick visual check; later we can add more advanced analysis
              (IR, curves, etc.).
            </p>
            <canvas id="voltageChart" style="width: 100%; max-height: 220px;"></canvas>
          </div>
<?php endif; ?>

          <h2 style="font-size: 16px; margin: 10px 0 8px;">Samples (up to 500)</h2>

<?php if (!$samples): ?>
          <p class="hero-text">
            No samples were recorded for this run. If you only sent a START and END
            event, this is expected. Once firmware sends SAMPLE events, they’ll
            appear here.
          </p>
<?php else: ?>
          <div style="overflow-x: auto; max-height: 380px;">
            <table style="width: 100%; border-collapse: collapse; font-size: 11px;">
              <thead>
                <tr>
                  <th style="text-align: left; padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.12);">Time</th>
                  <th style="text-align: left; padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.12);">Voltage (V)</th>
                  <th style="text-align: left; padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.12);">Current (A)</th>
                  <th style="text-align: left; padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.12);">Temp (°C)</th>
                  <th style="text-align: left; padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.12);">mAh</th>
                  <th style="text-align: left; padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.12);">Wh</th>
                </tr>
              </thead>
              <tbody>
<?php foreach ($samples as $sample): ?>
                <tr>
                  <td style="padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.06);">
                    <?= htmlspecialchars($sample['recorded_at'], ENT_QUOTES, 'UTF-8') ?>
                  </td>
                  <td style="padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.06);">
                    <?= $sample['voltage_v'] !== null ? htmlspecialchars((string)$sample['voltage_v'], ENT_QUOTES, 'UTF-8') : '—' ?>
                  </td>
                  <td style="padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.06);">
                    <?= $sample['current_a'] !== null ? htmlspecialchars((string)$sample['current_a'], ENT_QUOTES, 'UTF-8') : '—' ?>
                  </td>
                  <td style="padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.06);">
                    <?= $sample['temperature_c'] !== null ? htmlspecialchars((string)$sample['temperature_c'], ENT_QUOTES, 'UTF-8') : '—' ?>
                  </td>
                  <td style="padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.06);">
                    <?= $sample['milliamp_hours'] !== null ? htmlspecialchars((string)$sample['milliamp_hours'], ENT_QUOTES, 'UTF-8') : '—' ?>
                  </td>
                  <td style="padding: 4px 6px; border-bottom: 1px solid rgba(255,255,255,0.06);">
                    <?= $sample['watt_hours'] !== null ? htmlspecialchars((string)$sample['watt_hours'], ENT_QUOTES, 'UTF-8') : '—' ?>
                  </td>
                </tr>
<?php endforeach; ?>
              </tbody>
            </table>
          </div>
<?php endif; ?>
        </section>

        <aside class="right-panel">
          <div>
            <div class="right-heading">Run meta</div>
            <div class="status-badges">
              <div class="status-pill">
                <strong>Run ID</strong>
                <span><?= htmlspecialchars((string)$runId, ENT_QUOTES, 'UTF-8') ?></span>
              </div>
              <div class="status-pill">
                <strong>Samples</strong>
                <span><?= htmlspecialchars((string)count($samples), ENT_QUOTES, 'UTF-8') ?></span>
              </div>
            </div>
          </div>

          <div class="mini-grid">
            <div class="mini-card">
              <div class="mini-label">Next step</div>
              <div class="mini-value">More charts</div>
              <div class="mini-sub">Later we can add mAh vs time and IR.</div>
            </div>
            <div class="mini-card">
              <div class="mini-label">Cells</div>
              <div class="mini-value">Per barcode</div>
              <div class="mini-sub">View all runs per cell from the Cells page.</div>
            </div>
          </div>
        </aside>

<?php if ($chartSamples): ?>
<script>
  (function() {
    const samples = <?php echo json_encode($chartSamples, JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE); ?>;

    const labels = samples.map((s, idx) => s.recorded_at || ("Sample " + (idx + 1)));
    const voltages = samples.map(s => s.voltage_v);

    const ctx = document.getElementById('voltageChart');
    if (!ctx) return;

    new Chart(ctx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [{
          label: 'Voltage (V)',
          data: voltages,
          borderWidth: 2,
          tension: 0.15,
          pointRadius: 1.5
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
          x: {
            ticks: {
              maxRotation: 0,
              autoSkip: true,
              maxTicksLimit: 8
            }
          },
          y: {
            beginAtZero: false
          }
        },
        plugins: {
          legend: {
            display: true
          }
        }
      }
    });
  })();
</script>
<?php endif; ?>

<?php
require __DIR__ . '/templates/footer.php';
