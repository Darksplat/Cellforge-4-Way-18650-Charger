<?php
require_once __DIR__ . '/app/bootstrap.php';
require_once __DIR__ . '/app/db.php';

$pageTitle = 'Login';

$error = null;

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Safari-friendly field name
    $email = trim($_POST['username'] ?? '');
    $password = $_POST['password'] ?? '';

    if ($email !== '' && $password !== '') {
        $pdo = getPdo();

        $stmt = $pdo->prepare(
            'SELECT id, email, password_hash, display_name
             FROM users
             WHERE email = ?
             LIMIT 1'
        );
        $stmt->execute([$email]);
        $user = $stmt->fetch(PDO::FETCH_ASSOC);

        if ($user && password_verify($password, $user['password_hash'])) {
            $_SESSION['user_id'] = (int) $user['id'];
            $_SESSION['user_name'] = $user['display_name'] ?: $user['email'];
            header('Location: dashboard.php');
            exit;
        }

        $error = 'Invalid email or password.';
    } else {
        $error = 'Please enter both email and password.';
    }
}

require __DIR__ . '/templates/header.php';
?>

<section>
  <h1 class="hero-title">
    Login <span>to CellForge</span>
  </h1>

  <p class="hero-text">
    Sign in to access chargers, cells, and run history.
  </p>

  <?php if ($error): ?>
    <p style="color: var(--danger); font-size: 13px;">
      <?= htmlspecialchars($error, ENT_QUOTES, 'UTF-8') ?>
    </p>
  <?php endif; ?>

  <form method="post" action="login.php" name="login" id="login-form" autocomplete="on">
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

      <label for="password" style="display:none;">Password</label>
      <input
        id="password"
        type="password"
        name="password"
        placeholder="Password"
        autocomplete="current-password"
        required
      >

      <button type="submit">Login</button>
    </div>
  </form>
</section>

<?php
require __DIR__ . '/templates/footer.php';
