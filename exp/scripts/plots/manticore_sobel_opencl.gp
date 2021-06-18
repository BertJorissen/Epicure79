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

# set line styles
set style line 1 lt rgb "#003f5c"
set style line 2 lt rgb "#7a5195"
set style line 3 lt rgb "#ef5675"
set style line 4 lt rgb "#ffa600"

# Set limits for X and Y axis
set xrange [0.5:4.5]
set yrange [0.15 to 0.35]

# set y tics distance
set ytics 0.05

outfile = working_directory."../../results/manticore/plots/sobel/plot_opencl_sobel.eps"
set output outfile

exptitle=basetitle."OpenCL)"
set title exptitle

# plot columns to show from results"s csv 
data_dir=working_directory."../../results/manticore/stats/sobel_yuv/opencl/"

plot data_dir."opencl_ref_sync_null_1_1920_1080_100_NVIDIA-Tesla-V100-PCIE-32GB_NVIDIA-CUDA.csv" using 2:xtic(1) ls 1 title "Ref. Sync.", \
    data_dir."opencl_ref_async_null_1_1920_1080_100_NVIDIA-Tesla-V100-PCIE-32GB_NVIDIA-CUDA.csv" using 2 ls 3 title "Ref. Async.", \
    data_dir."opencl_ctrl_sync_off_1_1920_1080_100_NVIDIA-Tesla-V100-PCIE-32GB_NVIDIA-CUDA.csv" using 2 ls 2 title "Ctrl. Sync. Queues Off.", \
    data_dir."opencl_ctrl_async_off_1_1920_1080_100_NVIDIA-Tesla-V100-PCIE-32GB_NVIDIA-CUDA.csv" using 2 ls 4 title "Ctrl. Async. Queues Off."