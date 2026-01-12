<?php

function getSecondLifeImageUrl(array $cell): string
{
    $brand = trim((string)($cell['brand'] ?? ''));
    $model = trim((string)($cell['model'] ?? ''));

    if ($brand === '' || $model === '') {
        return 'cell_images/placeholder.png';
    }

    $brandEnc = rawurlencode($brand);
    $modelEnc = rawurlencode($model);

    // Browser-loaded (server fetch is blocked)
    return "https://secondlifestorage.com/images/cell_database/{$brandEnc}/{$modelEnc}.jpg";
}
