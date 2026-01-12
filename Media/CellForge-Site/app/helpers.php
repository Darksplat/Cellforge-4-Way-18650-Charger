<?php

function getCellImageUrl(?string $brand, ?string $model): ?string
{
    if (!$brand || !$model) {
        return null;
    }

    // Normalise strings
    $key = strtolower(trim($brand . '_' . $model));
    $key = preg_replace('/[^a-z0-9_]+/', '_', $key);

    $localDir = __DIR__ . '/../cell_images';
    $localFile = $localDir . '/' . $key . '.jpg';
    $publicUrl = 'cell_images/' . $key . '.jpg';

    // If cached image exists, use it
    if (file_exists($localFile)) {
        return $publicUrl;
    }

    // Known Second Life Storage image pattern (best-effort)
    $remoteUrl = 'https://secondlifestorage.com/cdn-cgi/image/quality=80/'
        . 'https://secondlifestorage.com/images/cell_database/'
        . rawurlencode($brand) . '/'
        . rawurlencode($model) . '.jpg';

    // Polite fetch with timeout
    $context = stream_context_create([
        'http' => [
            'timeout' => 5,
            'header'  => "User-Agent: CellForge/1.0\r\n"
        ]
    ]);

    $imageData = @file_get_contents($remoteUrl, false, $context);
    if ($imageData === false) {
        return null;
    }

    // Save locally
    file_put_contents($localFile, $imageData);

    return $publicUrl;
}
