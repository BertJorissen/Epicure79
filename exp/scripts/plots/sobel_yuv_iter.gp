#!/usr/bin/gnuplot
#
# NOTE: minimum version 5.2

set terminal postscript eps enhanced color font "Helvetica,18"
set datafile separator ","

# Set labels
set xlabel "Frames log scale"
set ylabel "Time log scale (sec)"

set key left top
# set key outside
# set key right top

set autoscale fix

set logscale y 10
set logscale x 10

# set line styles
set style line 1 lt 1 lc rgb "black" pt 4
set style line 2 lt 2 lc rgb "black" pt 8
set style line 3 lt 3 lc rgb "black" pt 1
set style line 4 lt 4 lc rgb "black" pt 2
set style line 5 lt 5 lc rgb "black" pt 6

do for [base in "FTF FTM MTF MTM"] {
	if(arch eq "cpu"){
		titlearch="CPU"

		# Set default params
		if(machine eq "manticore"){
			params="0_".base."_1920_1080_48" 
			numa_range="1-2"
		}else{ if(machine eq "medusa"){
			params="0_".base."_1920_1080_16" 
			numa_range="1-2"
		}else{ if(machine eq "gorgon"){
			params="1_".base."_1920_1080_32" 
			numa_range="2-3"
		}}}
	}else{ if(arch eq "cuda"){
		titlearch="CUDA"

		# Set default params
		if(machine eq "manticore"){
			params="1_".base."_1920_1080_NVIDIA-Tesla-V100-PCIE-32GB" 
		}else { if(machine eq "medusa"){
			params="1_".base."_1920_1080_NVIDIA-GeForce-GTX-TITAN-X" 
		}else { if(machine eq "gorgon"){
			params="1_".base."_1920_1080_NVIDIA-Graphics-Device" 
		}}}
	}else{ if(arch eq "opencl"){
		titlearch="OpenCL"

		# Set default params
		if(machine eq "manticore"){
			params="1_".base."_1920_1080_NVIDIA-Tesla-V100-PCIE-32GB_NVIDIA-CUDA" 
		}else { if(machine eq "medusa"){
			params="1_".base."_1920_1080_NVIDIA-GeForce-GTX-TITAN-X_NVIDIA-CUDA" 
		}else { if(machine eq "gorgon"){
			params="1_".base."_1920_1080_NVIDIA-Graphics-Device_NVIDIA-CUDA" 
		}}}
	}else{ if(arch eq "openclamd"){
		titlearch="OpenCL Amd"

		# Set default params
		params="0_".base."_1920_1080_gfx900:xnack-_AMD-Accelerated-Parallel-Processing" 
	}else{ if(arch eq "hip"){
		titlearch="HIP"

		# Set default params
		params="0_".base."_1920_1080_Radeon-Pro-WX-9100" 
	}else{
		print "invalid arch"
		exit
	}}}}}#thx gnuplot for the shitty formatting :D

	# Set titles
	basetitle="Sobel YUV ".base." (".machine." + ".titlearch.")"
	set title basetitle

	outfile = result_dir.machine."/plots/sobel_yuv/plot_".arch."_sobel_yuv_iter_".base.".eps"
	set output outfile

	# plot columns to show from results' csv 
	data_dir=result_dir.machine."/stats/sobel_yuv/".arch."/iter/"


	if(arch eq "cpu"){
		# offset for x axis (set offsets doesn't work )’
		stats data_dir."cpu_ref_sync_".params."_null_null.csv" using 1:2 name "ref" nooutput
		set xrange [ref_min_x*0.9 : ref_max_x*1.1]

		plot data_dir."cpu_ref_sync_".params."_null_null.csv" using 1:2:xtic(1) with linespoints ls 1 title "Ref. Sync.", \
			data_dir."cpu_ctrl_sync_".params."_".numa_range."_off.csv" using 1:2 with linespoints ls 2 title "Ctrl. Sync. No Copy.", \
			data_dir."cpu_ctrl_sync_".params."_".numa_range."_on.csv" using 1:2 with linespoints ls 3 title "Ctrl. Sync. Copy.", \
			data_dir."cpu_ctrl_async_".params."_".numa_range."_off.csv" using 1:2 with linespoints ls 4 title "Ctrl. Async. No Copy.", \
			data_dir."cpu_ctrl_async_".params."_".numa_range."_on.csv" using 1:2 with linespoints ls 5 title "Ctrl. Async. Copy."
	}else{
		# offset for x axis (set offsets doesn't work )’
		stats data_dir.arch."_ref_sync_".params.".csv" using 1:2 name "ref" nooutput
		set xrange [ref_min_x*0.9 : ref_max_x*1.1]

		plot data_dir.arch."_ref_sync_".params.".csv" using 1:2:xtic(1) with linespoints ls 1 title "Ref. Sync.", \
			data_dir.arch."_ref_async_".params.".csv" using 1:2 with linespoints ls 3 title "Ref. Async.", \
			data_dir.arch."_ctrl_sync_".params.".csv" using 1:2 with linespoints ls 2 title "Ctrl. Sync.", \
			data_dir.arch."_ctrl_async_".params.".csv" using 1:2 with linespoints ls 4 title "Ctrl. Async."
	}
} 