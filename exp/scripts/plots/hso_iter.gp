#!/usr/bin/gnuplot
#
# NOTE: minimum version 5.2

# set terminal png
set terminal pdfcairo enhanced color font "Helvetica,18"
set datafile separator ","

unset key

# set autoscale fix

# filled bars
set boxwidth 0.8
set style fill solid

# # Rotate X labels and get rid of the small striped at the top (nomirror)
# set xtics nomirror rotate by -45

# # horizontal lines
# set ytic scale 1.5
# set xtic scale 0
# set grid ytics


# set x axis range
set xrange [0.5:4.5]
set xtics nomirror

# # Set offsets
# set offsets 0, 0, graph 0.05, graph 0.05

# # # set y tics distance
# # set ytics 0.05
# # set mytics 5
# # set line styles
set style line 1 lt rgb "#003f5c"
set style line 2 lt rgb "#7a5195"
set style line 3 lt rgb "#ef5675"
set style line 4 lt rgb "#ffa600"

# plot columns to show from results' csv 
data_dir=result_dir.machine."/stats/hso/".arch."/iter/"

# Set titles
basetitle="HSOpticalFlow (A100)"
set title basetitle

outfile = result_dir.machine."/plots/hso/plot_".arch."_hso_sycl.pdf"
set output outfile

# Set labels
set xlabel "Versions"
set ylabel "Time (sec)"

set yrange [1:4.1]


plot data_dir."data.csv" using 0:2:4:xtic(1) with boxes lc rgb var,\
	data_dir."data.csv" using 0:($2+0.1):2 with labels

basetitle="HSOpticalFlow (V100)"
set title basetitle

outfile = result_dir.machine."/plots/hso/plot_".arch."_hso_sycl_mant.pdf"
set output outfile

set yrange [2:15]

plot data_dir."data.csv" using 0:3:4:xtic(1) with boxes lc rgb var,\
	data_dir."data.csv" using 0:($3+0.5):3 with labels


