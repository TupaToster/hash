set output 'combo.png'
set terminal png
set style data histograms
set grid
plot 'rolHash.hist', 'asciiSumHash.hist', 'crc32Hash.hist'