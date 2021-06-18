#!/usr/bin/gnuplot
#
# NOTE: this files creates eps

set terminal postscript eps enhanced color font "Helvetica,18"
set datafile separator ","

# Get path to this script
working_directory = system("dirname ".ARG0)."/"

# Set de los titulos
basetitle="Hotspot (Manticore + "

# Set labels
set xlabel "N log scale (Matrix size = N*N)"
set ylabel "Time log scale (sec)"

set key left top

set logscale y 10
set logscale x 10

set autoscale fix

# Set ticks
set xtics (1000, 2500, 5000, 7500, 10000)

set style line 1 lt 1 lc rgb "black" pt 4
set style line 2 lt 2 lc rgb "black" pt 8
set style line 3 lt 3 lc rgb "black" pt 1
set style line 4 lt 4 lc rgb "black" pt 2
set style line 5 lt 5 lc rgb "black" pt 6

# Set top and bottom limits for Y axis
xleftlimit=900
xrightlimit=10500

ytoplimit=200
ybotlimit=0.1

outfile = working_directory."../../results/manticore/plots/hotspot/plot_cpu_hotspot.eps"
set output outfile

exptitle=basetitle."CPU)"
set title exptitle

# plot columns to show from results"s csv 
data_dir=working_directory."../../results/manticore/stats/hotspot/cpu/"
plot [xleftlimit:xrightlimit][ybotlimit:ytoplimit] \
        data_dir."cpu_ref_sync_null_0_500_1_48_null_null.csv" using 1:2 with linespoints ls 1 ps 2 lw 2 title "Ref. Sync.", \
        data_dir."cpu_ctrl_sync_on_0_500_1_48_1_off.csv" using 1:2 with linespoints ls 2 ps 2 lw 2 title "Ctrl. Sync. No Copy. Queue On.", \
        data_dir."cpu_ctrl_sync_on_0_500_1_48_1_on.csv" using 1:2 with linespoints ls 3 ps 2 lw 2 title "Ctrl. Sync. Copy. Queue On.", \
        data_dir."cpu_ctrl_async_on_0_500_1_48_1_off.csv" using 1:2 with linespoints ls 4 ps 2 lw 2 title "Ctrl. Async. No Copy. Queue On.", \
        data_dir."cpu_ctrl_async_on_0_500_1_48_1_on.csv" using 1:2 with linespoints ls 5 ps 2 lw 2 title "Ctrl. Async. Copy. Queue On.", \
        data_dir."cpu_ctrl_sync_off_0_500_1_48_1_off.csv" using 1:2 with linespoints ls 2 ps 2 lw 2 title "Ctrl. Sync. No Copy. Queue Off.", \
        data_dir."cpu_ctrl_sync_off_0_500_1_48_1_on.csv" using 1:2 with linespoints ls 3 ps 2 lw 2 title "Ctrl. Sync. Copy. Queue Off.", \
        data_dir."cpu_ctrl_async_off_0_500_1_48_1_off.csv" using 1:2 with linespoints ls 4 ps 2 lw 2 title "Ctrl. Async. No Copy. Queue Off.", \
        data_dir."cpu_ctrl_async_off_0_500_1_48_1_on.csv" using 1:2 with linespoints ls 5 ps 2 lw 2 title "Ctrl. Async. Copy. Queue Off."