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
set xtics (1000, 2500, 5000, 10000, 15000)

set style line 1 dt 1 lc rgb "black" pt 4
set style line 2 dt 2 lc rgb "black" pt 8
set style line 3 dt 4 lc rgb "black" pt 1
set style line 4 dt 5 lc rgb "black" pt 2


# Set top and bottom limits for Y axis
xleftlimit=900
xrightlimit=18000

ytoplimit=70
ybotlimit=0.08

outfile = working_directory."../../results/manticore/plots/hotspot/plot_openclamd_hotspot.eps"
set output outfile

exptitle=basetitle."OpenCL Amd)"
set title exptitle

# plot columns to show from results"s csv 
data_dir=working_directory."../../results/manticore/stats/hotspot/openclamd/"
plot [xleftlimit:xrightlimit][ybotlimit:ytoplimit] \
        data_dir."openclamd_ref_sync_null_0_4_500_1_gfx900:xnack-_AMD-Accelerated-Parallel-Processing.csv" using 1:2 with linespoints ls 1 ps 2 lw 2 title "Ref. Sync.", \
        data_dir."openclamd_ref_async_null_0_4_500_1_gfx900:xnack-_AMD-Accelerated-Parallel-Processing.csv" using 1:2 with linespoints ls 2 ps 2 lw 2 title "Ref. Async.", \
        data_dir."openclamd_ctrl_sync_off_0_4_500_1_gfx900:xnack-_AMD-Accelerated-Parallel-Processing.csv" using 1:2 with linespoints ls 3 ps 2 lw 2 title "Ctrl. Sync. Queue Off.", \
        data_dir."openclamd_ctrl_async_off_0_4_500_1_gfx900:xnack-_AMD-Accelerated-Parallel-Processing.csv" using 1:2 with linespoints ls 4 ps 2 lw 2 title "Ctrl. Async. Queue Off."