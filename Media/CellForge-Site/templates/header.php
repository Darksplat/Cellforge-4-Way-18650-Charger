<?php
// templates/header.php
// Shared header for all CellForge pages.
?>
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <title><?= htmlspecialchars($pageTitle ?? 'CellForge', ENT_QUOTES, 'UTF-8') ?></title>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <style>
    :root {
      --bg: #050608;
      --card-bg: #101015;
      --accent: #ff7a2a;
      --accent-soft: rgba(255, 122, 42, 0.2);
      --text-main: #f7f7ff;
      --text-muted: #9a9ab5;
      --danger: #ff4b4b;
      --radius-lg: 16px;
      --radius-pill: 999px;
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }

    body {
      font-family: system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
      background:
        radial-gradient(circle at top left, #202030, #050608 55%),
        radial-gradient(circle at bottom right, #1a0f08, #050608 60%);
      color: var(--text-main);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 24px;
    }

    .shell {
      max-width: 960px;
      width: 100%;
      background: linear-gradient(135deg, rgba(255, 122, 42, 0.25), rgba(255, 245, 225, 0.04));
      border-radius: 24px;
      padding: 1px;
      box-shadow:
        0 24px 60px rgba(0, 0, 0, 0.8),
        0 0 40px rgba(255, 122, 42, 0.5);
    }

    .card {
      border-radius: 24px;
      background:
        radial-gradient(circle at top right, #2b1a10, #101015 52%),
        radial-gradient(circle at bottom left, #15151e, #050608 60%);
      padding: 24px 24px 20px 24px;
    }

    header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 16px;
      margin-bottom: 12px;
    }

    .brand {
      display: flex;
      align-items: center;
      gap: 12px;
    }

    .brand-logo {
      width: 40px;
      height: 40px;
      border-radius: 14px;
      background:
        radial-gradient(circle at 30% 10%, #ffe0c2, #ff7a2a 55%, #a63b00 90%);
      display: flex;
      align-items: center;
      justify-content: center;
      color: #050608;
      font-weight: 800;
      font-size: 20px;
      box-shadow:
        0 0 14px rgba(255, 122, 42, 0.9),
        0 0 28px rgba(255, 122, 42, 0.4);
    }

    .brand-logo span {
      transform: translateY(1px);
    }

    .brand-text-title {
      font-size: 20px;
      font-weight: 600;
    }

    .brand-text-subtitle {
      font-size: 13px;
      color: var(--text-muted);
    }

    .pill {
      border-radius: var(--radius-pill);
      padding: 6px 12px;
      font-size: 11px;
      letter-spacing: 0.08em;
      text-transform: uppercase;
      border: 1px solid var(--accent-soft);
      background: rgba(5, 6, 8, 0.85);
      color: var(--accent);
      display: inline-flex;
      align-items: center;
      gap: 6px;
    }

    .pill-dot {
      width: 7px;
      height: 7px;
      border-radius: 999px;
      background: #ffb347;
      box-shadow: 0 0 10px rgba(255, 179, 71, 0.95);
    }

    /* Top navigation */
    .nav {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 8px;
      margin-bottom: 8px;
      font-size: 12px;
      color: var(--text-muted);
    }

    .nav-links {
      display: flex;
      flex-wrap: wrap;
      gap: 6px;
    }

    .nav-link {
      border-radius: 999px;
      padding: 4px 10px;
      border: 1px solid transparent;
      text-decoration: none;
      font-size: 12px;
      color: var(--text-muted);
      background: rgba(5,6,8,0.8);
      display: inline-flex;
      align-items: center;
      gap: 6px;
    }

    .nav-link:hover {
      border-color: rgba(255, 122, 42, 0.6);
      color: var(--accent);
    }

    .nav-link-primary {
      border-color: rgba(255, 122, 42, 0.7);
      color: var(--accent);
    }

    .nav-user {
      font-size: 11px;
      color: var(--text-muted);
      display: flex;
      align-items: center;
      gap: 6px;
    }

    .nav-user-badge {
      border-radius: 999px;
      padding: 3px 8px;
      border: 1px solid rgba(255,255,255,0.12);
      background: rgba(5,6,8,0.9);
      font-size: 11px;
    }

    @media (max-width: 800px) {
      .nav {
        flex-direction: column;
        align-items: flex-start;
      }
    }

    main {
      margin-top: 12px;
      display: grid;
      grid-template-columns: minmax(0, 1.6fr) minmax(0, 1.2fr);
      gap: 20px;
    }

    @media (max-width: 800px) {
      main {
        grid-template-columns: minmax(0, 1fr);
      }
    }

    .hero-title {
      font-size: clamp(24px, 4vw, 30px);
      line-height: 1.2;
      margin-bottom: 10px;
    }

    .hero-title span {
      color: var(--accent);
    }

    .hero-text {
      font-size: 14px;
      color: var(--text-muted);
      margin-bottom: 16px;
    }

    .hero-list {
      list-style: none;
      margin-bottom: 18px;
    }

    .hero-list li {
      display: flex;
      align-items: flex-start;
      gap: 8px;
      font-size: 14px;
      color: var(--text-muted);
      margin-bottom: 6px;
    }

    .hero-list-icon {
      width: 16px;
      height: 16px;
      border-radius: 999px;
      border: 1px solid var(--accent-soft);
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 11px;
      color: var(--accent);
      flex-shrink: 0;
      margin-top: 2px;
    }

    .button-row {
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
    }

    .btn {
      border-radius: var(--radius-pill);
      padding: 9px 18px;
      font-size: 13px;
      font-weight: 500;
      border: 1px solid transparent;
      background: transparent;
      color: var(--text-main);
      cursor: pointer;
      text-decoration: none;
      display: inline-flex;
      align-items: center;
      gap: 8px;
      transition:
        transform 0.12s ease,
        box-shadow 0.12s ease,
        background 0.12s ease,
        border-color 0.12s ease;
    }

    .btn-primary {
      background: linear-gradient(135deg, #ff9a3c, #ff6b1a);
      border-color: rgba(255, 122, 42, 0.95);
      box-shadow: 0 12px 30px rgba(0, 0, 0, 0.8);
      color: #1a0b03;
    }

    .btn-primary:hover {
      transform: translateY(-1px);
      box-shadow: 0 16px 40px rgba(0, 0, 0, 0.9);
    }

    .btn-ghost {
      border-color: rgba(154, 154, 181, 0.35);
      background: rgba(5, 6, 8, 0.9);
      color: var(--text-muted);
    }

    .btn-ghost:hover {
      border-color: rgba(255, 122, 42, 0.6);
      color: var(--accent);
      transform: translateY(-1px);
    }

    .btn-icon {
      font-size: 14px;
    }

    .right-panel {
      background: radial-gradient(circle at top, rgba(255, 122, 42, 0.25), rgba(5, 6, 8, 0.95));
      border-radius: var(--radius-lg);
      border: 1px solid rgba(255, 122, 42, 0.35);
      padding: 14px 16px;
      display: grid;
      gap: 10px;
    }

    .right-heading {
      font-size: 13px;
      text-transform: uppercase;
      letter-spacing: 0.16em;
      color: var(--text-muted);
    }

    .status-badges {
      display: flex;
      flex-wrap: wrap;
      gap: 8px;
      font-size: 11px;
    }

    .status-pill {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      border-radius: var(--radius-pill);
      padding: 5px 10px;
      background: rgba(5, 6, 8, 0.9);
      border: 1px solid rgba(255, 255, 255, 0.04);
      color: var(--text-muted);
    }

    .status-pill strong {
      color: var(--accent);
    }

    .status-pill span {
      font-size: 9px;
      opacity: 0.75;
    }

    .mini-grid {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 10px;
      margin-top: 4px;
    }

    .mini-card {
      background: rgba(5, 6, 8, 0.95);
      border-radius: 12px;
      padding: 8px 9px;
      border: 1px solid rgba(255, 255, 255, 0.04);
      font-size: 11px;
      display: grid;
      gap: 4px;
    }

    .mini-label {
      text-transform: uppercase;
      letter-spacing: 0.12em;
      color: var(--text-muted);
      font-size: 10px;
    }

    .mini-value {
      font-size: 14px;
      font-weight: 600;
    }

    .mini-sub {
      font-size: 10px;
      color: var(--text-muted);
    }

    footer {
      margin-top: 18px;
      font-size: 11px;
      color: var(--text-muted);
      display: flex;
      justify-content: space-between;
      gap: 10px;
      flex-wrap: wrap;
    }

    footer a {
      color: var(--accent);
      text-decoration: none;
      opacity: 0.9;
    }

    footer a:hover {
      text-decoration: underline;
    }
  </style>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

  <style>
/* ===== CellForge PCB-style hero header ===== */
.cell-hero {
  display: grid;
  grid-template-columns: 160px 1fr 220px;
  gap: 14px;
  background: rgba(5,6,8,0.95);
  border: 1px solid rgba(255,255,255,0.08);
  border-radius: 10px;
  padding: 14px;
  margin-bottom: 18px;
}

.cell-hero-image {
  display: flex;
  align-items: center;
  justify-content: center;
  background: rgba(0,0,0,0.6);
  border: 1px dashed rgba(255,255,255,0.15);
  border-radius: 8px;
  font-size: 11px;
  color: #9a9ab5;
  text-align: center;
  padding: 8px;
}

.cell-hero-main h1 {
  font-size: 18px;
  margin: 0 0 4px 0;
}

.cell-hero-main .sub {
  font-size: 12px;
  color: #9a9ab5;
}

.cell-hero-status {
  display: grid;
  gap: 8px;
  align-content: start;
}

.status-box {
  border: 1px solid rgba(255,255,255,0.12);
  border-radius: 8px;
  padding: 8px;
  font-size: 12px;
}

.status-box strong {
  display: block;
  font-size: 11px;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: #9a9ab5;
  margin-bottom: 2px;
}
</style>

<style>
/* ===== Cells page (inventory view) ===== */

.cells-wrap {
  max-width: 980px;
}

.cells-form {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 8px;
  margin-bottom: 14px;
}

.cells-form input,
.cells-form textarea {
  padding: 7px 9px;
  border-radius: 8px;
  border: 1px solid rgba(255,255,255,0.14);
  background: rgba(5,6,8,0.9);
  color: var(--text-main);
  font-size: 12px;
}

.cells-form textarea {
  grid-column: span 2;
  resize: vertical;
  min-height: 60px;
}

.cells-form button {
  grid-column: span 2;
  padding: 8px;
  border-radius: var(--radius-pill);
  background: linear-gradient(135deg, #ff9a3c, #ff6b1a);
  border: 1px solid rgba(255,122,42,0.85);
  font-size: 12px;
  font-weight: 600;
  cursor: pointer;
  color: #1a0b03;
}

.cells-form button:hover {
  transform: translateY(-1px);
  box-shadow: 0 10px 26px rgba(0,0,0,0.7);
}

.cells-search {
  display: flex;
  gap: 6px;
  margin-bottom: 10px;
}

.cells-search input {
  flex: 1;
  padding: 6px 8px;
  border-radius: 8px;
  border: 1px solid rgba(255,255,255,0.15);
  background: rgba(5,6,8,0.9);
  color: var(--text-main);
  font-size: 12px;
}

.cells-search button,
.cells-search a {
  padding: 6px 10px;
  border-radius: var(--radius-pill);
  border: 1px solid rgba(255,255,255,0.15);
  background: rgba(5,6,8,0.9);
  color: var(--text-muted);
  font-size: 11px;
  text-decoration: none;
}

.cells-search button:hover,
.cells-search a:hover {
  border-color: rgba(255,122,42,0.6);
  color: var(--accent);
}

.cells-table {
  width: 100%;
  border-collapse: collapse;
  font-size: 12px;
  margin-top: 6px;
}

.cells-table th {
  text-align: left;
  padding: 8px 9px;
  border-bottom: 1px solid rgba(255,255,255,0.22);
  font-weight: 600;
  color: var(--text-main);
}

.cells-table td {
  padding: 7px 9px;
  border-bottom: 1px solid rgba(255,255,255,0.08);
  vertical-align: top;
}

.cells-table tr:hover {
  background: rgba(255,255,255,0.04);
}

.cell-id {
  font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace;
  color: var(--accent);
  text-decoration: none;
  font-size: 12px;
}

.cell-sub {
  display: block;
  font-size: 10px;
  color: var(--text-muted);
  margin-top: 1px;
}

/* ===== Cells list thumbnails ===== */
.cell-thumb {
  width: 42px;
  height: 42px;
  border-radius: 8px;
  background: rgba(0,0,0,0.6);
  border: 1px solid rgba(255,255,255,0.12);
  display: flex;
  align-items: center;
  justify-content: center;
  overflow: hidden;
}

.cell-thumb img {
  max-width: 100%;
  max-height: 100%;
  object-fit: contain;
}
</style>

</head>
<body>
  <div class="shell">
    <div class="card">
      <header>
        <div class="brand">
          <div class="brand-logo">
            <!-- Spark-style mark (can later be replaced with SVG) -->
            <span>⚡</span>
          </div>
          <div>
            <div class="brand-text-title">CellForge</div>
            <div class="brand-text-subtitle">Battery Test &amp; Analytics Portal</div>
          </div>
        </div>
        <div class="pill">
          <span class="pill-dot"></span>
          <span>Developer Preview</span>
        </div>
      </header>

      <nav class="nav">
        <div class="nav-links">
          <a href="index.php" class="nav-link">🏠 Home</a>
<?php if (!empty($_SESSION['user_id'])): ?>
<a href="dashboard.php" class="nav-link nav-link-primary">📊 Dashboard</a>
<a href="chargers.php" class="nav-link">⚡ Chargers</a>
<a href="cells.php" class="nav-link">🔋 Cells</a>
<a href="runs.php" class="nav-link">🧾 Runs</a>
          <a href="change_password.php" class="nav-link">🔒 Change password</a>
          <a href="logout.php" class="nav-link">🚪 Logout</a>
<?php else: ?>

          <a href="register.php" class="nav-link">✨ Register</a>
          <a href="login.php" class="nav-link nav-link-primary">🔐 Login</a>
<?php endif; ?>
        </div>
        <div class="nav-user">
<?php if (!empty($_SESSION['user_id'])): ?>
          <span>Signed in as</span>
          <span class="nav-user-badge">
            <?= htmlspecialchars($_SESSION['user_name'] ?? 'User', ENT_QUOTES, 'UTF-8') ?>
          </span>
<?php else: ?>
          <span class="nav-user-badge">Guest</span>
<?php endif; ?>
        </div>
      </nav>

      <main>
