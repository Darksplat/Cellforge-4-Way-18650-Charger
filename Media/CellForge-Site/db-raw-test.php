<?php
require_once __DIR__ . '/app/config.php';

ini_set('display_errors', '1');
ini_set('display_startup_errors', '1');
error_reporting(E_ALL);

$dsn = sprintf(
    'mysql:host=%s;dbname=%s;charset=%s',
    $dbConfig['host'],
    $dbConfig['dbname'],
    $dbConfig['charset']
);

try {
    $pdo = new PDO($dsn, $dbConfig['user'], $dbConfig['pass']);
    echo 'OK: Connected to database ' . htmlspecialchars($dbConfig['dbname'], ENT_QUOTES, 'UTF-8');
} catch (PDOException $e) {
    echo 'ERROR: ' . htmlspecialchars($e->getMessage(), ENT_QUOTES, 'UTF-8');
}
