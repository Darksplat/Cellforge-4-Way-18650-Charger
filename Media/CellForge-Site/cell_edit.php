<?php
// cell_edit.php - edit metadata for a single cell

require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

// Must be logged in
if (empty($_SESSION['user_id'])) {
    header('Location: login.php');
    exit;
}

$pageTitle = 'CellForge - Edit cell';

$userId = (int)($_SESSION['user_id'] ?? 0);

// Get cell ID from query
$cellId = isset($_GET['id']) ? (int)$_GET['id'] : 0;
if ($cellId <= 0) {
    http_response_code(400);
    echo 'Invalid cell ID.';
    exit;
}

$pdo = getPdo();

$cell   = null;
$errors = [];
$successMsg = '';

// Load existing cell
try {
    $stmt = $pdo->prepare(
        'SELECT *
         FROM cells
         WHERE id = :id
           AND user_id = :user_id
         LIMIT 1'
    );
    $stmt->execute([
        'id'      => $cellId,
        'user_id' => $userId,
    ]);
    $cell = $stmt->fetch();
} catch (Throwable $e) {
    $cell = null;
}

if (!$cell) {
    http_response_code(404);
    echo 'Cell not found.';
    exit;
}

// Initialise form values from existing cell
$barcode   = $cell['barcode'] ?? '';
$brand     = $cell['brand'] ?? '';
$model     = $cell['model'] ?? '';
$chemistry = $cell['chemistry'] ?? '';
$nomVolt   = $cell['nominal_voltage'] !== null ? (string)$cell['nominal_voltage'] : '';
$nomCap    = $cell['nominal_capacity_mah'] !== null ? (string)$cell['nominal_capacity_mah'] : '';
$notes     = $cell['notes'] ?? '';

// Handle form submit
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $barcode   = trim($_POST['barcode'] ?? '');
    $brand     = trim($_POST['brand'] ?? '');
    $model     = trim($_POST['model'] ?? '');
    $chemistry = trim($_POST['chemistry'] ?? '');
    $nomVolt   = trim($_POST['nominal_voltage'] ?? '');
    $nomCap    = trim($_POST['nominal_capacity_mah'] ?? '');
    $notes     = trim($_POST['notes'] ?? '');

    $nomVoltVal = null;
    if ($nomVolt !== '') {
        if (!is_numeric($nomVolt)) {
            $errors[] = 'Nominal voltage must be a number (e.g. 3.7).';
        } else {
            $nomVoltVal = (float)$nomVolt;
        }
    }

    $nomCapVal = null;
    if ($nomCap !== '') {
        if (!ctype_digit($nomCap)) {
            $errors[] = 'Nominal capacity must be an integer number of mAh.';
        } else {
            $nomCapVal = (int)$nomCap;
        }
    }

    if (!$errors) {
        try {
            $upd = $pdo->prepare(
                'UPDATE cells
                 SET
                   barcode = :barcode,
                   brand = :brand,
                   model = :model,
                   chemistry = :chemistry,
                   nominal_voltage = :nominal_voltage,
                   nominal_capacity_mah = :nominal_capacity_mah,
                   notes = :notes
                 WHERE id = :id
                   AND user_id = :user_id
                 LIMIT 1'
            );

            $upd->execute([
                'barcode'              => $barcode !== '' ? $barcode : null,
                'brand'                => $brand !== '' ? $brand : null,
                'model'                => $model !== '' ? $model : null,
                'chemistry'            => $chemistry !== '' ? $chemistry : null,
                'nominal_voltage'      => $nomVoltVal,
                'nominal_capacity_mah' => $nomCapVal,
                'notes'                => $notes !== '' ? $notes : null,
                'id'                   => $cellId,
                'user_id'              => $userId,
            ]);

            $successMsg = 'Cell updated successfully.';

            // Refresh cell data for display
            $stmt = $pdo->prepare(
                'SELECT *
                 FROM cells
                 WHERE id = :id
                   AND user_id = :user_id
                 LIMIT 1'
            );
            $stmt->execute([
                'id'      => $cellId,
                'user_id' => $userId,
            ]);
            $cell = $stmt->fetch();
        } catch (Throwable $e) {
            $errors[] = 'Unexpected error while updating the cell.';
        }
    }
}

require __DIR__ . '/templates/header.php';
?>

        <section>
          <h1 class="hero-title">
            Edit cell <span><?= htmlspecialchars($cell['barcode'] ?: ('#' . $cellId), ENT_QUOTES, 'UTF-8') ?></span>
          </h1>
          <p class="hero-text">
            Update the metadata for this cell. Changes will affect how stats and
            grades are calculated on the cell detail page.
          </p>

          <div style="margin-bottom: 16px; font-size: 13px; color: #9a9ab5;">
            <a href="cell.php?id=<?= htmlspecialchars((string)$cellId, ENT_QUOTES, 'UTF-8') ?>"
               style="color: #ff7a2a; text-decoration: none;">← Back to cell details</a>
          </div>

<?php if ($errors): ?>
          <div style="background: rgba(180, 40, 40, 0.15); border: 1px solid rgba(255, 75, 75, 0.6); border-radius: 10px; padding: 10px 12px; margin-bottom: 12px; font-size: 13px;">
            <strong style="color: #ff9a9a;">There were some problems:</strong>
            <ul style="margin: 6px 0 0 16px; padding-left: 0;">
<?php foreach ($errors as $err): ?>
              <li><?= htmlspecialchars($err, ENT_QUOTES, 'UTF-8') ?></li>
<?php endforeach; ?>
            </ul>
          </div>
<?php elseif ($successMsg): ?>
          <div style="background: rgba(40, 180, 100, 0.15); border: 1px solid rgba(80, 255, 160, 0.7); border-radius: 10px; padding: 10px 12px; margin-bottom: 12px; font-size: 13px;">
            <strong style="color: #b8ffcf;">Success:</strong>
            <?= htmlspecialchars($successMsg, ENT_QUOTES, 'UTF-8') ?>
          </div>
<?php endif; ?>

          <form method="post" action="cell_edit.php?id=<?= htmlspecialchars((string)$cellId, ENT_QUOTES, 'UTF-8') ?>"
                style="display: grid; gap: 10px; max-width: 520px;">

            <div>
              <label for="barcode" style="font-size: 13px; display: block; margin-bottom: 4px;">
                Cell barcode / ID
              </label>
              <input
                type="text"
                id="barcode"
                name="barcode"
                value="<?= htmlspecialchars($barcode, ENT_QUOTES, 'UTF-8') ?>"
                style="width: 100%; padding: 8px 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.12); background: rgba(5,6,8,0.9); color: #f7f7ff; font-size: 13px;"
              >
            </div>

            <div style="display: grid; grid-template-columns: repeat(2, minmax(0, 1fr)); gap: 8px;">
              <div>
                <label for="brand" style="font-size: 13px; display: block; margin-bottom: 4px;">
                  Brand
                </label>
                <input
                  type="text"
                  id="brand"
                  name="brand"
                  value="<?= htmlspecialchars($brand, ENT_QUOTES, 'UTF-8') ?>"
                  style="width: 100%; padding: 8px 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.12); background: rgba(5,6,8,0.9); color: #f7f7ff; font-size: 13px;"
                >
              </div>

              <div>
                <label for="model" style="font-size: 13px; display: block; margin-bottom: 4px;">
                  Model
                </label>
                <input
                  type="text"
                  id="model"
                  name="model"
                  value="<?= htmlspecialchars($model, ENT_QUOTES, 'UTF-8') ?>"
                  style="width: 100%; padding: 8px 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.12); background: rgba(5,6,8,0.9); color: #f7f7ff; font-size: 13px;"
                >
              </div>
            </div>

            <div style="display: grid; grid-template-columns: repeat(3, minmax(0, 1fr)); gap: 8px;">
              <div>
                <label for="chemistry" style="font-size: 13px; display: block; margin-bottom: 4px;">
                  Chemistry
                </label>
                <input
                  type="text"
                  id="chemistry"
                  name="chemistry"
                  value="<?= htmlspecialchars($chemistry, ENT_QUOTES, 'UTF-8') ?>"
                  style="width: 100%; padding: 8px 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.12); background: rgba(5,6,8,0.9); color: #f7f7ff; font-size: 13px;"
                >
              </div>

              <div>
                <label for="nominal_voltage" style="font-size: 13px; display: block; margin-bottom: 4px;">
                  Nominal V
                </label>
                <input
                  type="text"
                  id="nominal_voltage"
                  name="nominal_voltage"
                  value="<?= htmlspecialchars($nomVolt, ENT_QUOTES, 'UTF-8') ?>"
                  style="width: 100%; padding: 8px 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.12); background: rgba(5,6,8,0.9); color: #f7f7ff; font-size: 13px;"
                  placeholder="e.g. 3.7"
                >
              </div>

              <div>
                <label for="nominal_capacity_mah" style="font-size: 13px; display: block; margin-bottom: 4px;">
                  Nominal mAh
                </label>
                <input
                  type="text"
                  id="nominal_capacity_mah"
                  name="nominal_capacity_mah"
                  value="<?= htmlspecialchars($nomCap, ENT_QUOTES, 'UTF-8') ?>"
                  style="width: 100%; padding: 8px 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.12); background: rgba(5,6,8,0.9); color: #f7f7ff; font-size: 13px;"
                  placeholder="e.g. 2500"
                >
              </div>
            </div>

            <div>
              <label for="notes" style="font-size: 13px; display: block; margin-bottom: 4px;">
                Notes
              </label>
              <textarea
                id="notes"
                name="notes"
                rows="3"
                style="width: 100%; padding: 8px 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.12); background: rgba(5,6,8,0.9); color: #f7f7ff; font-size: 13px; resize: vertical;"
              ><?= htmlspecialchars($notes, ENT_QUOTES, 'UTF-8') ?></textarea>
            </div>

            <button type="submit" class="btn btn-primary" style="margin-top: 4px; justify-content: center; align-self: flex-start;">
              <span class="btn-icon">💾</span>
              <span>Save changes</span>
            </button>
          </form>
        </section>

        <aside class="right-panel">
          <div>
            <div class="right-heading">Tips</div>
            <div class="mini-grid">
              <div class="mini-card">
                <div class="mini-label">Nominal capacity</div>
                <div class="mini-value">Affects grade</div>
                <div class="mini-sub">Grade is based on average measured mAh vs nominal.</div>
              </div>
              <div class="mini-card">
                <div class="mini-label">Brand / model</div>
                <div class="mini-value">Searchable</div>
                <div class="mini-sub">Used by the Cells search box on the main page.</div>
              </div>
            </div>
          </div>
        </aside>

<?php
require __DIR__ . '/templates/footer.php';
