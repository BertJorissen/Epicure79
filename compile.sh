#!/bin/bash

declare -A archs
archs["CPU"]="OFF"
archs["CUDA"]="OFF"
archs["OPENCL"]="OFF"
archs["HIP"]="OFF"
archs["FPGA"]="OFF"

declare -A libs
libs["CUBLAS"]="OFF"
libs["MKL"]="OFF"
libs["MAGMA"]="OFF"

CMAKE_FLAGS=""
while :; do
	case $1 in
		-h | -\? | --help)
			echo "--------Controllers compiling script--------"
			echo "This script compiles hitmap if it isnt already, deletes everything in 'build' directory and compiles controllers with the options specified."
			echo "Usage: bash compile.sh OPTIONS"
			echo "	-a|--arch archs			Select ctrl architectures to support."
			echo "							Comma separated. Valid values: CUDA, HIP, CPU, OPENCL, FPGA."
			echo "							If not specified uses defaults specified in cmake."
			echo "	-e|--exp				Compile in 'experimentation mode' (easier to process output from benchmarks)."
			echo "	-d|--debug				Debug mode. Compile with -O0 -g and extra error checking and info."
			echo "	-c|--clean				Allways clean and recompile hitmap."
			echo "	-p|--profile			Enable marks for host tasks for profiling on CUDA and OpenCL AMD."
			echo "	--cc compiler			Use a diferent compiler."
			echo "	-l|--libs libs			Select blas libs to support. Comma separated. Valid values are cublas, mkl, magma."
			echo "							If not specified uses defaults specified in cmake."
			echo "	-f|--flags				Specify extra flags for compiler.Comma separated to specify multiple extra flags."
			exit
			;;
		-a | --arch)
			# Takes an option argument; ensure it has been specified.
			if [ "$2" ]; then
				archs_arg=(${2//,/ })
				for arch in "${archs_arg[@]}"; do
					arch=${arch^^}
					# check if arch exists as a key in archs array
					if [ ! "${archs[$arch]+abc}" ]; then
						echo "ERROR: Arch $arch not found."
						exit
					fi
					echo "Compiling for $arch"
					archs["$arch"]="ON"
					if [ "$arch" == "FPGA" ]; then
						. /opt/intel/oneapi/setvars.sh
					fi
				done
				shift
			else
				echo 'ERROR: "--arch" requires a non-empty option argument.'
				exit
			fi
			;;
		-e | --exp)
			echo "Compiling experimentation examples"
			CMAKE_FLAGS+="-DUSE_EXPERIMENTATION_EXAMPLES:BOOL=ON "
			;;
		-d | --debug)
			echo "Compiling with debug"
			CMAKE_FLAGS+="-DCTRL_DEBUG=ON "
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
				echo 'ERROR: "--arch" requires a non-empty option argument.'
				exit
			fi
			;;
		-l | --libs)
			if [ "$2" ]; then
				libs_arg=(${2//,/ })
				for lib in "${libs_arg[@]}"; do
					lib=${lib^^}
					# check if lib exists as a key in libs array
					if [ ! "${libs[$lib]+abc}" ]; then
						echo "ERROR: lib $lib not supported."
						exit
					fi
					echo "Compiling with $lib"
					libs["$lib"]="ON"
					if [[ "$lib" == "MKL" || "$lib" == "MAGMA" ]]; then
						. /opt/intel/oneapi/setvars.sh
					fi
				done
				shift
			else
				echo 'ERROR: "--libs" requires a non-empty option argument.'
				exit
			fi
			;;
		-f | --flags)
			if [ "$2" ]; then
				c_flags=(${2//,/ })
				echo "Using flags: ${c_flags[@]}"
				CMAKE_FLAGS+="-DCMAKE_C_FLAGS='${c_flags[@]}' "
				CMAKE_FLAGS+="-DCMAKE_CXX_FLAGS='${c_flags[@]}' "
				CMAKE_FLAGS+="-DCMAKE_CUDA_FLAGS='${c_flags[@]}' "
				shift
			else
				echo 'ERROR: "--flags" requires a non-empty option argument.'
				exit
			fi
			;;
		--)
			# End of all options.
			shift
			break
			;;
		-?*)
			echo "Unknown option: $1"
			echo "Use '-h' or '--help' for help."
			exit
			;;
		*)
			# Default case: No more options, so break out of the loop.
			break
			;;
	esac
	shift
done

if [ "$archs_arg" ]; then
	for a in "${!archs[@]}"; do
		CMAKE_FLAGS+="-DSUPPORT_$a:BOOL=${archs[$a]} "
	done
fi
if [ "$libs_arg" ]; then
	for l in "${!libs[@]}"; do
		CMAKE_FLAGS+="-D$l:BOOL=${libs[$l]} "
	done
fi

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

eval "cmake $CMAKE_FLAGS .."

make -j 12
cd ..
echo "... Done!"
