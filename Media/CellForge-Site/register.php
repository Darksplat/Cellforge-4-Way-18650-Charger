<?php
require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

$pageTitle = 'Register';

$error = null;

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Use the same field name Safari expects
    $email = trim($_POST['username'] ?? '');
    $displayName = trim($_POST['display_name'] ?? '');
    $password = $_POST['password'] ?? '';

    if ($email !== '' && $password !== '') {
        $pdo = getPdo();

        $stmt = $pdo->prepare('SELECT id FROM users WHERE email = ? LIMIT 1');
        $stmt->execute([$email]);

        if ($stmt->fetch()) {
            $error = 'An account with that email already exists.';
        } else {
            $stmt = $pdo->prepare(
                'INSERT INTO users (email, password_hash, display_name)
                 VALUES (?, ?, ?)'
            );
            $stmt->execute([
                $email,
                password_hash($password, PASSWORD_DEFAULT),
                $displayName ?: null
            ]);

            $_SESSION['user_id'] = (int) $pdo->lastInsertId();
            $_SESSION['user_name'] = $displayName ?: $email;

            header('Location: dashboard.php');
            exit;
        }
    } else {
        $error = 'Email and password are required.';
    }
}

require __DIR__ . '/templates/header.php';
?>

<section>
  <h1 class="hero-title">
    Register <span>New Account</span>
  </h1>

  <p class="hero-text">
    Create an account to manage chargers and track cell performance.
  </p>

  <?php if ($error): ?>
    <p style="color: var(--danger); font-size: 13px;">
      <?= htmlspecialchars($error, ENT_QUOTES, 'UTF-8') ?>
    </p>
  <?php endif; ?>

  <form method="post" action="register.php" name="register" id="register-form" autocomplete="on">
    <div class="cells-form">
      <label for="username" style="display:none;">Email</label>
      <input
        id="username"
        type="text"
        name="username"
        placeholder="Email address"
        autocomplete="username"
        inputmode="email"
        autocapitalize="none"
        spellcheck="false"
        required
      >

      <label for="display_name" style="display:none;">Display name</label>
      <input
        id="display_name"
        type="text"
        name="display_name"
        placeholder="Display name (optional)"
        autocomplete="name"
      >

      <label for="password" style="display:none;">Password</label>
      <input
        id="password"
        type="password"
        name="password"
        placeholder="Password"
        autocomplete="new-password"
        required
      >

      <button type="submit">Register</button>
    </div>
  </form>
</section>

<?php
require __DIR__ . '/templates/footer.php';
