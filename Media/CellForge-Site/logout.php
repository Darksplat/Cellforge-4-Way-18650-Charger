<?php
// logout.php - end the user session and send them back to login

require_once __DIR__ . '/app/bootstrap.php';

// Clear all session data
$_SESSION = [];

// Destroy the session cookie if there is one
if (ini_get('session.use_cookies')) {
    $params = session_get_cookie_params();
    setcookie(
        session_name(),
        '',
        time() - 42000,
        $params['path'],
        $params['domain'],
        $params['secure'],
        $params['httponly']
    );
}

// Finally destroy the session
session_destroy();

// Redirect to login page
header('Location: login.php');
exit;
