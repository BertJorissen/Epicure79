#!/usr/bin/gnuplot
#
# NOTE: minimum version 5.2

set terminal postscript eps enhanced color font "Helvetica,18"
set datafile separator ","

# Get path to this script
working_directory = system("dirname ".ARG0)."/"

if(arch eq "cpu"){
	titlearch="CPU"

	# Set default params
	if(machine eq "manticore"){
		params="0_40_48" 
	}else{ if(machine eq "medusa"){
		params="0_40_16" 
	}}
}else{ if(arch eq "cuda"){
	titlearch="CUDA"

	# Set default params
	if(machine eq "manticore"){
		params="1_40_NVIDIA-Tesla-V100-PCIE-32GB" 
	}else { if(machine eq "medusa"){
		params="1_40_NVIDIA-GeForce-GTX-TITAN-X" 
	}}
}else{ if(arch eq "opencl"){
	titlearch="OpenCL"

	# Set default params
	if(machine eq "manticore"){
		params="1_40_NVIDIA-Tesla-V100-PCIE-32GB_NVIDIA-CUDA" 
	}else { if(machine eq "medusa"){
		params="1_40_NVIDIA-GeForce-GTX-TITAN-X_NVIDIA-CUDA" 
	}}
}else{ if(arch eq "openclamd"){
	titlearch="OpenCL Amd"

	# Set default params
	params="0_40_gfx900:xnack-_AMD-Accelerated-Parallel-Processing" 
}else{
	print "invalid arch"
	exit
}}}}#thx gnuplot for the shitty formatting :D

# Set titles
basetitle="Matrix Pow (".machine." + ".titlearch.")"
set title basetitle

outfile = working_directory."../../results/".machine."/plots/matrix_pow/plot_".arch."_matpow_size.eps"
set output outfile

# Set labels
set xlabel "N log scale (Matrix size = N*N)"
set ylabel "Time log scale (sec)"

set key left top

set logscale y 10
set logscale x 10

set autoscale fix

set style line 1 lt 1 lc rgb "black" pt 4
set style line 2 lt 2 lc rgb "black" pt 8
set style line 3 lt 3 lc rgb "black" pt 1
set style line 4 lt 4 lc rgb "black" pt 2
set style line 5 lt 5 lc rgb "black" pt 6

# plot columns to show from results' csv 
data_dir=working_directory."../../results/".machine."/stats/matrix_pow/".arch."/size/"
#TODO @sergioalo add check for queues
if(arch eq "cpu"){
	# offset for x axis (set offsets doesn't work )’
	stats data_dir."cpu_ref_sync_null_".params."_null_null.csv" using 1:2 name "ref" nooutput
	set xrange [ref_min_x*0.9 : ref_max_x*1.1]

	plot data_dir."cpu_ref_sync_null_".params."_null_null.csv" using 1:2:xtic(1) with linespoints ls 1 ps 2 lw 2 title "Ref. Sync.", \
			data_dir."cpu_ctrl_sync_off_".params."_1_off.csv" using 1:2 with linespoints ls 2 ps 2 lw 2 title "Ctrl. Sync. No Copy. Queue Off.", \
			data_dir."cpu_ctrl_sync_off_".params."_1_on.csv" using 1:2 with linespoints ls 3 ps 2 lw 2 title "Ctrl. Sync. Copy. Queue Off.", \
			data_dir."cpu_ctrl_async_off_".params."_1_off.csv" using 1:2 with linespoints ls 4 ps 2 lw 2 title "Ctrl. Async. No Copy. Queue Off.", \
			data_dir."cpu_ctrl_async_off_".params."_1_on.csv" using 1:2 with linespoints ls 5 ps 2 lw 2 title "Ctrl. Async. Copy. Queue Off."
}else{
	# offset for x axis (set offsets doesn't work )’
	stats data_dir.arch."_ref_sync_null_".params.".csv" using 1:2 name "ref" nooutput
	set xrange [ref_min_x*0.9 : ref_max_x*1.1]

	plot data_dir.arch."_ref_sync_null_".params.".csv" using 1:2:xtic(1) with linespoints ls 1 ps 2 lw 2 title "Ref. Sync.", \
		data_dir.arch."_ref_async_null_".params.".csv" using 1:2 with linespoints ls 2 ps 2 lw 2 title "Ref. Async.", \
		data_dir.arch."_ctrl_sync_off_".params.".csv" using 1:2 with linespoints ls 3 ps 2 lw 2 title "Ctrl. Sync. Queue Off.", \
		data_dir.arch."_ctrl_async_off_".params.".csv" using 1:2 with linespoints ls 4 ps 2 lw 2 title "Ctrl. Async. Queue Off."

	# comparisons with sycl
	if(machine eq "manticore"){
		outfile = working_directory."../../results/".machine."/plots/matrix_pow/plot_".arch."_matrix_pow_size_comp.eps"
		set output outfile

		set key spacing 1 font ",14"

		sycl_dir=working_directory."../../fran/graficas/plotMm/"
		if((arch eq "cuda") || (arch eq "opencl")){
			plot data_dir.arch."_ref_sync_null_".params.".csv" using 1:2:xtic(1) with linespoints ls 1 ps 2 lw 2 title "Ref. Sync.", \
				data_dir.arch."_ref_async_null_".params.".csv" using 1:2 with linespoints ls 2 ps 2 lw 2 title "Ref. Async.", \
				data_dir.arch."_ctrl_sync_off_".params.".csv" using 1:2 with linespoints ls 3 ps 2 lw 2 title "Ctrl. Sync. Queue Off.", \
				data_dir.arch."_ctrl_async_off_".params.".csv" using 1:2 with linespoints ls 4 ps 2 lw 2 title "Ctrl. Async. Queue Off.", \
				sycl_dir."12" using 1:2 title "HIP-CUDA-S" with linespoints lw 02 lc rgb "black" ,\
				sycl_dir."22" using 1:2 title "HIP-CUDA-AS" with linespoints lw 02 lc rgb "black" ,\
				sycl_dir."32" using 1:2 title "INTEL-CUDA-S" with linespoints lw 02 lc rgb "black" ,\
				sycl_dir."42" using 1:2 title "INTEL-CUDA-AS" with linespoints lw 02 lc rgb "black"

		}else{ if(arch eq "openclamd"){
			plot data_dir.arch."_ref_sync_null_".params.".csv" using 1:2:xtic(1) with linespoints ls 1 ps 2 lw 2 title "Ref. Sync.", \
				data_dir.arch."_ref_async_null_".params.".csv" using 1:2 with linespoints ls 2 ps 2 lw 2 title "Ref. Async.", \
				data_dir.arch."_ctrl_sync_off_".params.".csv" using 1:2 with linespoints ls 3 ps 2 lw 2 title "Ctrl. Sync. Queue Off.", \
				data_dir.arch."_ctrl_async_off_".params.".csv" using 1:2 with linespoints ls 4 ps 2 lw 2 title "Ctrl. Async. Queue Off.", \
				sycl_dir."52" title "HIP-ROCM-S" with linespoints lw 02 lc rgb "black" ,\
				sycl_dir."62" title "HIP-ROCM-AS" with linespoints lw 02 lc rgb "black"
	}}}
}