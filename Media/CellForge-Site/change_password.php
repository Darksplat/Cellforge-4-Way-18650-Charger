<?php
// change_password.php - allow a logged-in user to change their password

require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

// Must be logged in
if (empty($_SESSION['user_id'])) {
    header('Location: login.php');
    exit;
}

$pageTitle = 'CellForge - Change Password';

$errors     = [];
$successMsg = '';

// Handle form submit
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $currentPassword = $_POST['current_password'] ?? '';
    $newPassword     = $_POST['new_password'] ?? '';
    $confirmPassword = $_POST['confirm_password'] ?? '';

    if ($currentPassword === '') {
        $errors[] = 'Current password is required.';
    }

    if ($newPassword === '') {
        $errors[] = 'New password is required.';
    } elseif (strlen($newPassword) < 10) {
        $errors[] = 'New password must be at least 10 characters long.';
    }

    if ($confirmPassword === '') {
        $errors[] = 'Please confirm your new password.';
    } elseif ($newPassword !== $confirmPassword) {
        $errors[] = 'New password and confirmation do not match.';
    }

    if (!$errors) {
        try {
            $pdo = getPdo();

            // Load current user's password hash
            $stmt = $pdo->prepare('SELECT password_hash FROM users WHERE id = :id LIMIT 1');
            $stmt->execute(['id' => $_SESSION['user_id']]);
            $user = $stmt->fetch();

            if (!$user) {
                $errors[] = 'Could not find your account. Please log in again.';
            } else {
                // Check current password
                if (!password_verify($currentPassword, $user['password_hash'])) {
                    $errors[] = 'Current password is incorrect.';
                } else {
                    // Hash new password
                    $newHash = password_hash($newPassword, PASSWORD_DEFAULT);

                    // Update in database
                    $update = $pdo->prepare('UPDATE users SET password_hash = :hash WHERE id = :id');
                    $update->execute([
                        'hash' => $newHash,
                        'id'   => $_SESSION['user_id'],
                    ]);

                    // Regenerate session ID for security
                    session_regenerate_id(true);

                    $successMsg = 'Your password has been updated successfully.';
                }
            }
        } catch (Throwable $e) {
            $errors[] = 'Unexpected error while updating your password.';
        }
    }
}

require __DIR__ . '/templates/header.php';
?>

        <section>
          <h1 class="hero-title">
            Change your <span>CellForge</span> password.
          </h1>
          <p class="hero-text">
            For security, you need to confirm your current password before choosing a
            new one. Your new password will be stored using a strong one-way hash.
          </p>

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

          <form method="post" action="change_password.php" style="display: grid; gap: 10px; max-width: 360px;">
            <div>
              <label for="current_password" style="font-size: 13px; display: block; margin-bottom: 4px;">
                Current password
              </label>
              <input
                type="password"
                id="current_password"
                name="current_password"
                required
                style="width: 100%; padding: 8px 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.12); background: rgba(5,6,8,0.9); color: #f7f7ff; font-size: 13px;"
              >
            </div>

            <div>
              <label for="new_password" style="font-size: 13px; display: block; margin-bottom: 4px;">
                New password
              </label>
              <input
                type="password"
                id="new_password"
                name="new_password"
                required
                style="width: 100%; padding: 8px 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.12); background: rgba(5,6,8,0.9); color: #f7f7ff; font-size: 13px;"
              >
            </div>

            <div>
              <label for="confirm_password" style="font-size: 13px; display: block; margin-bottom: 4px;">
                Confirm new password
              </label>
              <input
                type="password"
                id="confirm_password"
                name="confirm_password"
                required
                style="width: 100%; padding: 8px 10px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.12); background: rgba(5,6,8,0.9); color: #f7f7ff; font-size: 13px;"
              >
            </div>

            <button type="submit" class="btn btn-primary" style="margin-top: 4px; justify-content: center;">
              <span class="btn-icon">🔒</span>
              <span>Update password</span>
            </button>

            <p style="font-size: 11px; color: #9a9ab5; margin-top: 4px;">
              Your password is never stored in plain text. We use PHP's
              <code>password_hash()</code> and <code>password_verify()</code> for
              secure storage and checking.
            </p>
          </form>
        </section>

        <aside class="right-panel">
          <div>
            <div class="right-heading">Security details</div>
            <div class="status-badges">
              <div class="status-pill">
                <strong>Hashing</strong>
                <span>password_hash</span>
              </div>
              <div class="status-pill">
                <strong>Session</strong>
                <span>Regenerated on change</span>
              </div>
            </div>
          </div>

          <div class="mini-grid">
            <div class="mini-card">
              <div class="mini-label">Tip</div>
              <div class="mini-value">Use a passphrase</div>
              <div class="mini-sub">e.g. a long sentence you remember easily.</div>
            </div>
            <div class="mini-card">
              <div class="mini-label">Next step</div>
              <div class="mini-value">Dashboard</div>
              <div class="mini-sub"><a href="dashboard.php" style="color: #ff7a2a; text-decoration: none;">Return to dashboard</a></div>
            </div>
          </div>
        </aside>

<?php
require __DIR__ . '/templates/footer.php';
