#!/usr/bin/gnuplot
#
# NOTE: this files creates eps

set terminal postscript eps enhanced color font "Helvetica,18"
set datafile separator ","

# Get path to this script
working_directory = system("dirname ".ARG0)."/"

# Set de los titulos
basetitle="Sobel (Manticore + "

# Set labels
set xlabel "Baseline"
set ylabel "Time (sec)"

# set key left top
set key outside
set key right top

set autoscale fix

# Multiple bars
set style data histograms

# filled bars
set boxwidth 0.8
set style fill solid 1.00 

# Rotate X labels and get rid of the small striped at the top (nomirror)
set xtics nomirror rotate by -45

# horizontal lines
set tic scale 0
set grid ytics

set style line 1 lt rgb "#003f5c"
set style line 2 lt rgb "#58508d"
set style line 3 lt rgb "#bc5090"
set style line 4 lt rgb "#ff6361"
set style line 5 lt rgb "#ffa600"

# Set limits for X and Y axis
set xrange [0.5:4.5]
set yrange [0.2 to 2]

# set y tics distance
set ytics 0.5

outfile = working_directory."../../results/manticore/plots/sobel/plot_cpu_sobel.eps"
set output outfile

exptitle=basetitle."CPU)"
set title exptitle

# plot columns to show from results"s csv 
data_dir=working_directory."../../results/manticore/stats/sobel_yuv/cpu/"

plot data_dir."cpu_ref_sync_null_0_1920_1080_100_48_null_null.csv" using 2:xtic(1) ls 1 title "Ref. Sync.", \
    data_dir."cpu_ctrl_sync_on_0_1920_1080_100_48_1_off.csv" using 2 ls 2 title "Ctrl. Sync. No Copy. Queue On.", \
    data_dir."cpu_ctrl_sync_on_0_1920_1080_100_48_1_on.csv" using 2 ls 3 title "Ctrl. Sync. Copy. Queue On.", \
    data_dir."cpu_ctrl_async_on_0_1920_1080_100_48_1_off.csv" using 2 ls 4 title "Ctrl. Async. No Copy. Queue On.", \
    data_dir."cpu_ctrl_async_on_0_1920_1080_100_48_1_on.csv" using 2 ls 5 title "Ctrl. Async. Copy. Queue On.", \
    data_dir."cpu_ctrl_sync_off_0_1920_1080_100_48_1_off.csv" using 2 ls 2 title "Ctrl. Sync. No Copy. Queue Off.", \
    data_dir."cpu_ctrl_sync_off_0_1920_1080_100_48_1_on.csv" using 2 ls 3 title "Ctrl. Sync. Copy. Queue Off.", \
    data_dir."cpu_ctrl_async_off_0_1920_1080_100_48_1_off.csv" using 2 ls 4 title "Ctrl. Async. No Copy. Queue Off.", \
    data_dir."cpu_ctrl_async_off_0_1920_1080_100_48_1_on.csv" using 2 ls 5 title "Ctrl. Async. Copy. Queue Off."