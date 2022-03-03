#!/bin/bash

CMAKE_FLAGS=""
while :; do
	case $1 in
		-h | -\? | --help)
			echo "--------Controllers compiling script--------"
			echo "This script compiles hitmap if it isnt already, deletes everything in 'build' directory and compiles controllers with the options specified."
			echo "Usage: bash compile.sh OPTIONS"
			echo "	-a|--arch archs			Select ctrl architectures to support."
			echo "							Comma separated. Valid values: cuda, cpu, opencl, openclamd, fpga."
			echo "							By default compiles for cuda, cpu and opencl."
			echo "	-e|--exp				Compile in 'experimentation mode' (easier to process output from benchmarks)."
			echo "	-q|--queues				Enable queues."
			echo "	-d|--debug				Debug mode. Compile with -O0 -g and extra error checking and info."
			echo "	-c|--clean				Allways clean and recompile hitmap."
			echo "	-p|--profile			Enable marks for host tasks for profiling on CUDA and OpenCL AMD."
			echo "	--cc compiler			Use a diferent compiler."
			echo "	-l|--libs libs			Select blas libs to support. Comma separated. Valid values are cublas, mkl, magma"
			echo "	-f|--flags				Specify extra flags for compiler.Comma separated to specify multiple extra flags."
			echo
			echo "Notes: "
			echo "	'opencl' and 'openclamd' architectures cannot be active at the same time. If both are passed at the same time 'openclamd' will be active."
			exit
			;;
		-a | --arch) # Takes an option argument; ensure it has been specified.
			if [ "$2" ]; then
				archs=(${2//,/ })
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
				shift
			else
				echo 'ERROR: "--arch" requires a non-empty option argument.'
			fi
			;;
		-e | --exp)
			echo "Compiling experimentation examples"
			CMAKE_FLAGS+="-DUSE_EXPERIMENTATION_EXAMPLES:BOOL=ON "
			;;
		-q | --queues)
			echo "Compiling with queues active"
			CMAKE_FLAGS+="-DCTRL_QUEUE=ON "
			;;
		-d | --debug)
			echo "Compiling with debug"
			CMAKE_FLAGS+="-DCTRL_DEBUG=ON -DCTRL_DEBUGGER=ON "
			;;
		-c | --clean)
			recompile_hitmap=1
			;;
		-p | --profile)
			echo "Compiling with profiling"
			CMAKE_FLAGS+="-DPROFILING_ENABLED=ON "
			;;
		--cc)
			if [ "$2" ]; then
				echo "using $2 compiler"
				case "$2" in
					icc)
						CMAKE_FLAGS+="-DCMAKE_C_COMPILER=icc "
						. /opt/intel/oneapi/setvars.sh
						icc --version
						;;
					aocc)
						CMAKE_FLAGS+="-DCMAKE_C_COMPILER=clang "
						. /opt/AMD/aocc-compiler-3.1.0/setenv_AOCC.sh
						clang --version
						;;
					*)
						CMAKE_FLAGS+="-DCMAKE_C_COMPILER=$2 "
						$2 --version
						;;
				esac
				shift
			else
				die 'ERROR: "--arch" requires a non-empty option argument.'
			fi
			;;
		-l | --libs)
			if [ "$2" ]; then
				libs=(${2//,/ })
				for lib in "${libs[@]}"; do
					case $lib in
						cublas)
							echo "Compiling with cublas"
							CMAKE_FLAGS+="-DCUBLAS:BOOL=ON "
							;;
						mkl)
							echo "Compiling with mkl"
							. /opt/intel/oneapi/setvars.sh
							CMAKE_FLAGS+="-DMKL:BOOL=ON "
							;;
						magma)
							echo "Compiling with Magma"
							. /opt/intel/oneapi/setvars.sh
							CMAKE_FLAGS+="-DMAGMA:BOOL=ON "
							;;
						*)
							echo "Lib $lib not supported."
							exit
							;;
					esac
				done
				shift
			else
				echo 'ERROR: "--libs" requires a non-empty option argument.'
			fi
			;;
		-f | --flags)
			if [ "$2" ]; then
				c_flags=(${2//,/ })
				echo "Using flags: ${c_flags[@]}"
				CMAKE_FLAGS+="-DCMAKE_C_FLAGS='${c_flags[@]}' "
				CMAKE_FLAGS+="-DCMAKE_CUDA_FLAGS='${c_flags[@]}' "
				shift
			else
				die 'ERROR: "--flags" requires a non-empty option argument.'
			fi
			;;
		--) # End of all options.
			shift
			break
			;;
		-?*)
			echo "Unknown option: $1"
			echo "Use '-h' or '--help' for help."
			exit
			;;
		*) # Default case: No more options, so break out of the loop.
			break ;;
	esac
	shift
done

# move to controllers dir
cd "$(dirname "${BASH_SOURCE[0]}")"

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
	eval "cmake $CMAKE_FLAGS .."
else
	eval "cmake -DSUPPORT_CUDA:BOOL=ON -DSUPPORT_CPU:BOOL=ON -DSUPPORT_OPENCL_GPU:BOOL=ON -DSUPPORT_FPGA:BOOL=ON $CMAKE_FLAGS .."
fi

make -j 12
cd ..
echo "... Done!"
