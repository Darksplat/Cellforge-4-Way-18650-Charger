<?php
// index.php - CellForge landing page

require_once __DIR__ . '/app/bootstrap.php';

// This title appears in the browser tab:
$pageTitle = 'CellForge Battery Portal';

require __DIR__ . '/templates/header.php';
?>

        <section>
          <h1 class="hero-title">
            Forge better packs from reclaimed cells with <span>CellForge</span>.
          </h1>
          <p class="hero-text">
            CellForge is your central hub for 18650 testing: from Brett’s original
            Arduino Nano 4× charger/discharger to new ESP32-based rigs, barcode
            tracking, and a full cell database with photos and history.
          </p>

          <ul class="hero-list">
            <li>
              <div class="hero-list-icon">1</div>
              <div>Start with a clean web portal layout &amp; project structure.</div>
            </li>
            <li>
              <div class="hero-list-icon">2</div>
              <div>Add secure accounts, charger registration, and a cell database.</div>
            </li>
            <li>
              <div class="hero-list-icon">3</div>
              <div>Connect your Nano/ESP32 firmware for live slot data and analytics.</div>
            </li>
          </ul>

          <div class="button-row">
            <a href="#" class="btn btn-primary">
              <span class="btn-icon">🧪</span>
              <span>Portal coming soon</span>
            </a>
            <a href="#" class="btn btn-ghost">
              <span class="btn-icon">📘</span>
              <span>Project info &amp; build docs</span>
            </a>
          </div>
        </section>

        <aside class="right-panel">
          <div>
            <div class="right-heading">Roadmap</div>
            <div class="status-badges">
              <div class="status-pill">
                <strong>Step 1</strong>
                <span>Landing &amp; layout</span>
              </div>
              <div class="status-pill">
                <strong>Step 2</strong>
                <span>Auth &amp; database</span>
              </div>
              <div class="status-pill">
                <strong>Step 3</strong>
                <span>Live API + devices</span>
              </div>
            </div>
          </div>

          <div class="mini-grid">
            <div class="mini-card">
              <div class="mini-label">Chargers</div>
              <div class="mini-value">0</div>
              <div class="mini-sub">Registration comes after login system.</div>
            </div>
            <div class="mini-card">
              <div class="mini-label">Cells tracked</div>
              <div class="mini-value">0</div>
              <div class="mini-sub">Photos &amp; SLS-style database later.</div>
            </div>
            <div class="mini-card">
              <div class="mini-label">API status</div>
              <div class="mini-value">Offline</div>
              <div class="mini-sub">No endpoints wired up yet.</div>
            </div>
            <div class="mini-card">
              <div class="mini-label">Firmware link</div>
              <div class="mini-value">Pending</div>
              <div class="mini-sub">Will target ESP32 + Nano/ESP8266.</div>
            </div>
          </div>
        </aside>

<?php
require __DIR__ . '/templates/footer.php';
