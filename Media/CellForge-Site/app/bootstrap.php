<?php
// app/bootstrap.php
// Common bootstrap for all CellForge pages.

declare(strict_types=1);

// Show errors while we’re developing.
// Later on a live site you might turn this off.
ini_set('display_errors', '1');
ini_set('display_startup_errors', '1');
error_reporting(E_ALL);

// Start a session (needed later for login).
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

// If you ever need a base URL, you can set it here.
// For now we’ll just leave a comment.
// Example (if you keep it in /cellforge on the server):
// $baseUrl = '/cellforge';

require_once __DIR__ . '/helpers.php';
require_once __DIR__ . '/cell_images.php';

