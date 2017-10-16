reset
set ylabel 'time(sec)'
set style fill solid
set title 'performance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'

plot [:][:0.150]'output.txt' using 2:xtic(1) with histogram title 'original', \
'' using 3:xtic(1) with histogram title 'eliminating structure size'  , \
'' using 4:xtic(1) with histogram title 'memory pool'  , \
'' using 5:xtic(1) with histogram title 'hash'  , \
'' using ($0-0.10):($2+0.0100):2 with labels title ' ', \
'' using ($0+0.07):($3+0.0090):3 with labels title ' ',  \
'' using ($0+0.17):($4+0.0050):4 with labels title ' ', \
'' using ($0+0.40):($5+0.0020):5 with labels title ' ', \