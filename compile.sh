#!/bin/bash

CMAKE_FLAGS=""
while getopts ":a:eq:hdcp" opt; do
  case $opt in
	a) 
		archs=(${OPTARG//,/ })
		for arch in "${archs[@]}"; do
			case $arch in 
				cuda)
					echo "Compiling for CUDA"
					CMAKE_FLAGS+="-DSUPPORT_CUDA:BOOL=ON "
					;;
				openclamd)
					echo "Compiling for OpenCl amd"
					CMAKE_FLAGS+="-DSUPPORT_OPENCL_GPU:BOOL=ON -DSUPPORT_OPENCL_GPU_AMD:BOOL=ON "
					;;
				opencl)
					echo "Compiling for OpenCL"
					CMAKE_FLAGS+="-DSUPPORT_OPENCL_GPU:BOOL=ON "
					;;
				cpu)
					echo "Compiling for CPU"
					CMAKE_FLAGS+="-DSUPPORT_CPU:BOOL=ON "
					;;
				fpga)
					echo "Compiling for FPGA"
					CMAKE_FLAGS+="-DSUPPORT_FPGA:BOOL=ON "
					. /opt/intel/oneapi/setvars.sh
					;;
				*)
					echo "Arch $arch not found."
					exit
					;;
			esac
		done
	;;
	e)
		echo "Compiling experimentation examples"
		CMAKE_FLAGS+="-DUSE_EXPERIMENTATION_EXAMPLES:BOOL=ON "
	;;
	q)
		echo "Compiling with queues $OPTARG"
		CMAKE_FLAGS+="-DCTRL_QUEUE=$OPTARG "
	;;
	d)
		echo "Compiling with debug"
		CMAKE_FLAGS+="-DCTRL_DEBUG=ON -DCTRL_DEBUGGER=ON "
	;;
	c)
		recompile_hitmap=1
	;;
	p)
		echo "Compiling with profiling"
		CMAKE_FLAGS+="-DPROFILING_ENABLED=ON "
	;;
	h)
		echo "--------Controllers compiling script--------"
		echo "This script compiles hitmap if it isnt already, deletes everything in 'build' directory and compiles controllers with the options specified."
		echo "Usage: bash compile.sh OPTIONS"
		echo "	-a archs		Select ctrl architectures to support."
		echo "					Comma separated. Valid values: cuda, cpu, opencl, openclamd, fpga."
		echo "					By default compiles for cuda, cpu and opencl."
		echo "	-e				Compile in 'experimentation mode' (easier to process output from benchmarks)."
		echo "	-q queues		Select if queues should be active or not. Valid values: on, off. If not specified default is 'on'"
		echo "	-d				Debug mode. Compile with -O0 -g and extra error checking and info."
		echo "	-c				Allways clean and recompile hitmap."
		echo "	-p				Enable marks for host tasks for profiling on CUDA and OpenCL AMD."
		echo
		echo "Notes: "
		echo "	'opencl' and 'openclamd' architectures cannot be active at the same time. If both are passed at the same time 'openclamd' will be active."
		exit
	;;
	\?) 
		echo "Invalid option -$OPTARG" >&2
		echo "Use '-h' for help."
		exit
	;;
  esac
done

# move to controllers dir
cd "$( dirname "${BASH_SOURCE[0]}" )"

# load modules
. ./env.sh

# check if hitmap is compiled, if not, compile it
echo "Checking extern libs..."
if [ ! -f "extern/hitmap/lib/libhit.a" ] || [ $recompile_hitmap ]; then
	echo "Hitmap not found, compiling..."
	cd extern/hitmap
	make clean
	make -j 12
	cd ../../
fi
echo "... Done!"

echo "Clean and rebuild..."
rm -rf build/
mkdir -p build && cd build

if [ "$archs" ]; then 
	cmake $CMAKE_FLAGS ..
else 
	cmake -DSUPPORT_CUDA:BOOL=ON -DSUPPORT_CPU:BOOL=ON -DSUPPORT_OPENCL_GPU:BOOL=ON -DSUPPORT_FPGA:BOOL=ON $CMAKE_FLAGS ..
fi

make -j 12
cd ..
echo "... Done!"
