#!/usr/bin/gnuplot
#
# NOTE: minimum version 5.2

set terminal postscript eps enhanced color font "Helvetica,18"
set datafile separator ","

# set line styles
set style line 1 lt rgb "#003f5c"
set style line 2 lt rgb "#7a5195"
set style line 3 lt rgb "#ef5675"
set style line 4 lt rgb "#ffa600"

if(arch eq "cpu"){
	titlearch="CPU"
	
	# Set line styles
	set style line 1 lt rgb "#003f5c"
	set style line 2 lt rgb "#58508d"
	set style line 3 lt rgb "#bc5090"
	set style line 4 lt rgb "#ff6361"
	set style line 5 lt rgb "#ffa600"

	# Set default params
	if(machine eq "manticore"){
		params="0_1920_1080_100_48" 
		numa_range="1-2"
	}else{ if(machine eq "medusa"){
		params="0_1920_1080_100_16" 
		numa_range="1-2"
	}else{ if(machine eq "gorgon"){
		params="1_1920_1080_100_32" 
		numa_range="2-3"
	}}}
}else{ if(arch eq "cuda"){
	titlearch="CUDA"

	# Set default params
	if(machine eq "manticore"){
		params="1_1920_1080_100_NVIDIA-Tesla-V100-PCIE-32GB" 
	}else { if(machine eq "medusa"){
		params="1_1920_1080_100_NVIDIA-GeForce-GTX-TITAN-X" 
	}else { if(machine eq "gorgon"){
		params="1_1920_1080_100_NVIDIA-Graphics-Device" 
	}}}
}else{ if(arch eq "opencl"){
	titlearch="OpenCL"

	# Set default params
	if(machine eq "manticore"){
		params="1_1920_1080_100_NVIDIA-Tesla-V100-PCIE-32GB_NVIDIA-CUDA" 
	}else { if(machine eq "medusa"){
		params="1_1920_1080_100_NVIDIA-GeForce-GTX-TITAN-X_NVIDIA-CUDA" 
	}else { if(machine eq "gorgon"){
		params="1_1920_1080_100_NVIDIA-Graphics-Device_NVIDIA-CUDA" 
	}}}
}else{ if(arch eq "openclamd"){
	titlearch="OpenCL Amd"

	# Set default params
	params="0_1920_1080_100_gfx900:xnack-_AMD-Accelerated-Parallel-Processing" 
}else{ if(arch eq "hip"){
	titlearch="HIP"

	# Set default params
	params="0_1920_1080_100_Radeon-Pro-WX-9100" 
}else{
	print "invalid arch"
	exit
}}}}}#thx gnuplot for the shitty formatting :D

# Set titles
basetitle="Sobel YUV (".machine." + ".titlearch.")"
set title basetitle

outfile = result_dir.machine."/plots/sobel_yuv/plot_".arch."_sobel_yuv_size.eps"
set output outfile

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
set ytic scale 1.5
set xtic scale 0
set grid ytics


# set x axis range
set xrange [0.5:4.5]

# Set offsets
set offsets 0, 0, graph 0.05, graph 0.05

# set y tics distance
set ytics 0.05
set mytics 5

# plot columns to show from results' csv 
data_dir=result_dir.machine."/stats/sobel_yuv/".arch."/size/"
if(arch eq "cpu"){
	plot data_dir."cpu_ref_sync_".params."_null_null.csv" using 2:xtic(1) ls 1 title "Ref. Sync.", \
		data_dir."cpu_ctrl_sync_".params."_".numa_range."_off.csv" using 2 ls 2 title "Ctrl. Sync. No Copy.", \
		data_dir."cpu_ctrl_sync_".params."_".numa_range."_on.csv" using 2 ls 3 title "Ctrl. Sync. Copy.", \
		data_dir."cpu_ctrl_async_".params."_".numa_range."_off.csv" using 2 ls 4 title "Ctrl. Async. No Copy.", \
		data_dir."cpu_ctrl_async_".params."_".numa_range."_on.csv" using 2 ls 5 title "Ctrl. Async. Copy."
}else{
	plot data_dir.arch."_ref_sync_".params.".csv" using 2:xtic(1) ls 1 title "Ref. Sync.", \
		data_dir.arch."_ref_async_".params.".csv" using 2 ls 3 title "Ref. Async.", \
		data_dir.arch."_ctrl_sync_".params.".csv" using 2 ls 2 title "Ctrl. Sync.", \
		data_dir.arch."_ctrl_async_".params.".csv" using 2 ls 4 title "Ctrl. Async."
}