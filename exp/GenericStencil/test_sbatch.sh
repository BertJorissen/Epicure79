#!/bin/bash
#SBATCH -n 5
#SBATCH --gpus=4
#SBATCH -p dark-night
#SBATCH -t 2
#SBATCH --exclusive

# Script for testing the correct results of parallel Hitmap and reference applications
#
# (c) 2015 Arturo Gonzalez-Escribano
#

# 1. EXPERIMENT INFORMATION
# 1.1. APP NAME AND DEFAULT EXECUTABLES LIST
APP_NAME="GenericStencil"
EXE_NAMES="../../build/ParallelStencilSkeleton/ParallelStencilSkeleton"

# 1.2. APP/EXPERIMENT SPECIFIC PARAMETERS
PROCS=(1 2 3 4 5 2 3 4 5) # 6 6 7 7)
SIZES_1=(151250000 151250000 151250000 151250000 15125000 302500000 453750000 605000000 756250000)
SIZES_2_0=(27500 27500 27500 27500 27500 55000 82500 55000 137500 27500 82500 27500 192500 27500 110000)
SIZES_2_1=(27500 27500 27500 27500 27500 27500 27500 55000 27500 27500 55000 27500 27500 27500 110000)
SIZES_3_0=(900 900 900 900 900 1800 2700 1800 4500 900 2700 900 6300 900 1800)
SIZES_3_1=(900 900 900 900 900 900 900 1800 900 900 1800 900 900 900 1800)
SIZES_3_2=(900 900 900 900 900 900 900 900 900 900 900 900 900 900 1800)
ITERATIONS=(10)
STENCILS=("1dc2" "1dnc4" "2d4" "2d9" "2dnc9" "2df5" "3d27")

mkdir out
mkdir profile

# 1.3. MPI RUN COMMAND
if [ -z "$MPI_RUN" ]
then
	MPI_RUN="srun -Q "
fi

# 2. WRITE HEADER
echo
echo EXPERIMENTATION SCRIPT
echo ----------------------
echo "APPLICATION: $APP_NAME"

# 3. CHECK PARAMETERS
# 3.1. HELP
#if [ $# -lt 1 ]
#then
#	echo
#	echo -e "\tUsage: $0 [all | <executable_names>]"
#	echo
#	exit 1
#fi

# 3.2. TEST SPECIFIC EXECUTABLES INSTEAD OF THE PREDEFINED LIST
if [ $# -gt 0 ] && [ $1 != all ]
then
	EXE_NAMES=$@
fi
echo "EXECUTABLES: $EXE_NAMES"
echo

# 3.3. CHECK THAT EXECUTABLES HAVE BEEN GENERATED
for name in $EXE_NAMES
do
	OK=y
	if [ ! -x $name ]
	then
		echo "Error: Executable not found -- $name"
		OK=n
	fi
done
if [ $OK != y ]
then
	echo
	echo -e "\tBefore using this script generate the executables"
	echo
	exit 1
fi

# 4. TE/STING EXPERIMENT
function doTest() {
	# EXECUTE PROGRAM
	dims=$(echo $5 | cut -c1)
	for j in $(seq 1 10); do
		#CUDA_PROFILE="nvprof -f -o profile/$5.$3.$2.$j.%q{SLURM_PROCID}.nvprof"
		CUDA_PROFILE=""
		#CUDA_PROFILE_="nvprof -f -o profile/_$5.$3.$2.$j.%q{SLURM_PROCID}.nvprof"
		CUDA_PROFILE_=""

		if [ $dims == "1" ]
		then
			echo $2 $5 ${SIZES_1[$3]} 1 $4 - $j
			$MPI_RUN -n $2 $CUDA_PROFILE ./$1 $5 ${SIZES_1[$3]} 1 $4 > out/$5.$3.$2.$j.out 2> /dev/null
	
			# EXECUTION ERRORS
			if [ 0 != $? ]
			then
				echo ERROR EXECUTING: -n $2 $5 $3 $4
				echo \(Error $?\)
				continue
			fi

			echo $2 _$5 ${SIZES_1[$3]} 1 $4 - $j
			$MPI_RUN -n $2 $CUDA_PROFILE_ ./$1 _$5 ${SIZES_1[$3]} 1 $4 > out/_$5.$3.$2.$j.out 2> /dev/null
		elif [ $dims == "2" ]
		then
			echo $2 $5 ${SIZES_2_0[$3]} ${SIZES_2_1[$3]} 1 $4 - $j
			$MPI_RUN -n $2 $CUDA_PROFILE ./$1 $5 ${SIZES_2_0[$3]} ${SIZES_2_1[$3]} 1 $4 > out/$5.$3.$2.$j.out 2> /dev/null

			# EXECUTION ERRORS
			if [ 0 != $? ]
			then
					echo ERROR EXECUTING: -n $2 $5 $3 $4
					continue
			fi

			echo $2 _$5 ${SIZES_2_0[$3]} ${SIZES_2_1[$3]} 1 $4 - $j
			$MPI_RUN -n $2 $CUDA_PROFILE_ ./$1 _$5 ${SIZES_2_0[$3]} ${SIZES_2_1[$3]} 1 $4 > out/_$5.$3.$2.$j.out 2> /dev/null
		elif [ $dims == "3" ]
		then
			echo $2 $5 ${SIZES_3_0[$3]} ${SIZES_3_1[$3]} ${SIZES_3_2[$3]} 1 $4 - $j
			$MPI_RUN -n $2 $CUDA_PROFILE ./$1 $5 ${SIZES_3_0[$3]} ${SIZES_3_1[$3]} ${SIZES_3_2[$3]} 1 $4 > out/$5.$3.$2.$j.out 2> /dev/null

			# EXECUTION ERRORS
			if [ 0 != $? ]
			then
					echo ERROR EXECUTING: -n $2 $5 $3 $4
					continue
			fi

			echo $2 _$5 ${SIZES_3_0[$3]} ${SIZES_3_1[$3]} ${SIZES_3_2[$3]} 1 $4 - $j
			$MPI_RUN -n $2 $CUDA_PROFILE_ ./$1 _$5 ${SIZES_3_0[$3]} ${SIZES_3_1[$3]} ${SIZES_3_2[$3]} 1 $4 > out/_$5.$3.$2.$j.out 2> /dev/null
		fi
	done

	# EXECUTION ERRORS
	if [ 0 != $? ]
	then
			echo ERROR EXECUTING: -n $2 _$5 $3 $4
			continue
	fi
}

# 5. LOOPS FOR TESTS
for exe in $EXE_NAMES
do
	echo
	echo "Testing: $exe"
	echo "------------------------"
	for stencil in ${STENCILS[@]}
	do
		for iter in ${ITERATIONS[@]}
		do
			for (( i=0; i<${#PROCS[@]}; i++ ))
			do
			dims=$(echo $stencil | cut -c1)
			size=${SIZES_2[i]}
			if [ $dims == "1" ]
			then
				size=${SIZES_1[i]}
			elif [ $dims == "3" ]
			then
				size=${SIZES_3[i]}
			fi
				doTest $exe ${PROCS[i]} $i $iter $stencil
			done
		done
	done
done

echo ""
echo "Done"

# 6. CLEAN
rm -f $RESULT_FILE
rm -f check.err

