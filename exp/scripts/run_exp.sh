#!/bin/bash

# get on controllers dir
cd "$( dirname "${BASH_SOURCE[0]}" )"
cd ../..

while getopts ":a:b:v:q:m:n:h" opt; do
  case $opt in
	a) 
		archs=(${OPTARG//,/ })
		for arch in "${archs[@]}"; do
			if [ $arch != "cpu" ] && [ $arch != "cuda" ] && [ $arch != "opencl" ] && [ $arch != "openclamd" ]; then
				echo "unknown arch $arch. Use -h for help."
				exit
			fi
		done
		echo "executing for archs $OPTARG"
	;;
	b)
		benchmarks=(${OPTARG//,/ })
		for bench in "${benchmarks[@]}"; do
			if [ $bench != "hotspot" ] && [ $bench != "matrix_pow" ] && [ $bench != "sobel_yuv" ]; then
				echo "unknown benchmark $bench. Use -h for help."
				exit
			fi
		done
		echo "executing benchmarks $OPTARG"
	;;
	v)
		versions=(${OPTARG//,/ })
		for v in "${versions[@]}"; do
			if [ $v != "ref" ] && [ $v != "ctrl" ]; then
				echo "unknown version $v. Use -h for help."
				exit
			fi
		done
		echo "executing versions $OPTARG"
	;;
	q)
		queues=(${OPTARG//,/ })
		for q in "${queues[@]}"; do
			if [ $q != "on" ] && [ $q != "off" ]; then
				echo "unknown queue option $q. Use -h for help."
				exit
			fi
		done
		echo "queues will be: $OPTARG"
	;;
	m)
		machines=(${OPTARG//,/ })
		for m in "${machines[@]}"; do
			echo "./exp/scripts/machine_configs/${m}.config"
			if [[ ! -f "./exp/scripts/machine_configs/${m}.config" ]]; then
				echo "Config file for machine $m not found. Use -h for help."
				exit
			fi
		done
		echo "executing on $OPTARG"
	;;
	n)
		N_REPS=$OPTARG
		echo "reps = $N_REPS"
	;;
	h)
		echo "--------Controllers experimentation script--------"
		echo "This script launches a experimentation of the benchmarks and architectures selected on the nodes selected."
		echo "Usage: bash run_exp.sh [OPTIONS]"
		echo "	-h				Show this help."
		echo "	-a archs		Select architectures to test."
		echo "					Comma separated. Valid values: cuda, cpu, opencl, openclamd."
		echo "					By default executes everything."
		echo "	-b benches		Select benchmarks to execute."
		echo "					Comma separated. Valid values: hotspot, matrix_pow, sobel_yuv."
		echo "					By default executes everything."
		echo " 	-v version		Select versions to test. Valid values: ctrl, ref."
		echo "					By default executes both."
		echo "	-q queues		Select if queues should be active or not. Valid values: on, off. If not specified default is execute both."
		echo "	-m machines		Machines to execute experimentation on."
		echo "					Comma separated. A <machine>.config file must be present in order to use it. Check manticore.config as an example of config file format."
		echo "					If a machine selected is not compatible with some of the architectures selected those architectures will be skipped for that specific machine."
		echo "					Default is manticore."
		echo "	-n reps			Number of repetitions to be done by this script. Default is 30."
		exit
	;;
	\?) 
		echo "Invalid option -$OPTARG" >&2
		echo "Use '-h' for help."
		exit
	;;
  esac
done

# default values for args
archs=(${archs[@]:-"cpu" "cuda" "opencl" "openclamd"})
benchmarks=(${benchmarks[@]:-"hotspot" "matrix_pow" "sobel_yuv"})
versions=(${versions[@]:-"ref" "ctrl"})
queues=(${queues[@]:-"on" "off"})
machines=(${machines[@]:-"manticore"})
N_REPS=${N_REPS:-"30"}


# load modules and get on frontend dir
. ./env.sh

# common hotspot values
HOTSPOT_PATH=./build/examples/hotspot
hotspot_iters=500
hotspot_iter_per_cpy=1

# common matpow values
MATPOW_PATH=./build/examples/Matrix_Power_Of
matpow_iters=40

# common sobel values
SOBEL_PATH=./build/examples/Sobel_YUV
sobel_in=/home/video/BigBuckBunny_1920_1080_24fps_1500fs.yuv
sobel_out=/tmp/out.yuv
sobel_ref=/tmp/reference_sobel_out.yuv
sobel_width=1920
sobel_height=1080
sobel_frames=100
sobel_baselines=("FTF" "MTF" "MTM" "FTM")

for machine in "${machines[@]}"; do
	# default flags for srun
	SLURM_DEFAULT_FLAGS="--exclusive -Q -p dark-night -w $machine"

	# result output dir
	RESULT_PATH=./exp/results/${machine}/raw
	mkdir -p $RESULT_PATH

	for arch in "${archs[@]}"; do
		. ./exp/scripts/machine_configs/${machine}.config
		case $arch in
			cpu)
				# values for CPU
				arch_name="Cpu"
				extra_params_default="$n_threads "
				extra_args_default="$n_threads, "

				hotspot_height=""
				hotspot_height_str=""

				hotspot_legend="arch, version, policy, queues, host_aff, size, iters, iters_per_copy, n_threads, device, mem_transfers, sum, result, main_clock, exec_clock"
				matpow_legend="arch, version, policy, queues, host_aff, size, iters, n_threads, device, mem_transfers,"
				sobel_legend="arch, version, policy, queues, host_aff, baseline, width, height, frames, n_threads, device, mem_transfers, main_clock, exec_clock"
			;;
			cuda)
				# values for CUDA
				arch_name="Cuda"
				extra_params="$device"
				extra_args=""

				hotspot_height=4
				hotspot_height_str="4, "

				hotspot_legend="arch, version, policy, queues, host_aff, size, height, iters, iters_per_copy, device, sum, result, main_clock, exec_clock"
				matpow_legend="arch, version, policy, queues, host_aff, size, iters, device,"
				sobel_legend="arch, version, policy, queues, host_aff, baseline, width, height, frames, device, main_clock, exec_clock,"
			;;
			opencl)
				# values for opencl
				arch_name="OpenCL_Gpu"
				extra_params="$device $platform"
				extra_args=""

				hotspot_height=4
				hotspot_height_str="4, "
				
				hotspot_legend="arch, version, policy, queues, host_aff, size, height, iters, iters_per_copy, device, platform, sum, result, main_clock, exec_clock"
				matpow_legend="arch, version, policy, queues, host_aff, size, iters, device, platform,"
				sobel_legend="arch, version, policy, queues, host_aff, baseline, width, height, frames, device, platform, main_clock, exec_clock"
			;;
			openclamd)
				# values for openclamd
				arch_name="OpenCL_Gpu"
				extra_params="$device $platform"
				extra_args=""

				hotspot_height=4
				hotspot_height_str="4, "
				
				hotspot_legend="arch, version, policy, queues, host_aff, size, height, iters, iters_per_copy, device, platform, sum, result, main_clock, exec_clock"
				matpow_legend="arch, version, policy, queues, host_aff, size, iters, device, platform,"
				sobel_legend="arch, version, policy, queues, host_aff, baseline, width, height, frames, device, platform, main_clock, exec_clock"
			;;
		esac

		#write legends on files
		if [[ " ${benchmarks[@]} " =~ " hotspot " ]]; then
			mkdir -p $RESULT_PATH/hotspot/$arch
			for v in "${versions[@]}"; do
				echo $hotspot_legend > $RESULT_PATH/hotspot/$arch/${v}.log
			done
		fi
		if [[ " ${benchmarks[@]} " =~ " matrix_pow " ]]; then
			mkdir -p $RESULT_PATH/matrix_pow/$arch
			for v in "${versions[@]}"; do                
				echo -n $matpow_legend > $RESULT_PATH/matrix_pow/$arch/${v}.log
				for i in $(seq 1 $matpow_iters); do
					echo -n " iter${i}_sum, iter${i}_res," >> $RESULT_PATH/matrix_pow/$arch/${v}.log
				done
				echo "main_clock, exec_clock" >> $RESULT_PATH/matrix_pow/$arch/${v}.log
			done
		fi
		if [[ " ${benchmarks[@]} " =~ " sobel_yuv " ]]; then
			mkdir -p $RESULT_PATH/sobel_yuv/$arch
			for v in "${versions[@]}"; do                                
				echo $sobel_legend > $RESULT_PATH/sobel_yuv/$arch/${v}.log
			done
		fi

		#ctrl benchmarks
		if [[ " ${versions[@]} " =~ " ctrl " ]]; then
			for queue in "${queues[@]}"; do
				bash compile.sh -a $arch -e -q $queue 

				for reps in $(seq 1 $N_REPS); do
					for policy in {0,1}; do
						if [ $policy == 0 ]; then
							policy_str=sync
						elif [ $policy == 1 ]; then
							policy_str=async
						fi

						if [ $arch != "cpu" ]; then
							transfers=(0)
						fi
						for mem_transfers in "${transfers[@]}"; do
							if [ $mem_transfers == 0 ]; then
								mem_transfers_str=off
							elif [ $mem_transfers == 1 ]; then
								mem_transfers_str=on
							fi
							
							if [ $arch == "cpu" ]; then
								extra_args="$extra_args_default$device, $mem_transfers_str, "
								extra_params="$extra_params_default $device $mem_transfers"
							fi

							# HOTSPOT
							if [[ " ${benchmarks[@]} " =~ " hotspot " ]]; then
								echo "$(date +"%T") Progress update: executing $arch hotspot transfers->$mem_transfers_str policy->$policy_str rep->${reps}/${N_REPS} queues->$queue version->ctrl"
								for size in ${HOTSPOT_SIZES[@]}; do
									echo -n "$arch, ctrl, $policy_str, $queue, $host_aff, $size, $hotspot_height_str$hotspot_iters, $hotspot_iter_per_cpy, $extra_args" >> $RESULT_PATH/hotspot/$arch/ctrl.log
									srun $SLURM_FLAGS $HOTSPOT_PATH/Hotspot_${arch_name}_Ctrl $size $hotspot_height $hotspot_iters $hotspot_iter_per_cpy $extra_params $policy $host_aff >> $RESULT_PATH/hotspot/$arch/ctrl.log
								done
							fi

							# MATPOW
							if [[ " ${benchmarks[@]} " =~ " matrix_pow " ]]; then
								echo "$(date +"%T") Progress update: executing $arch matpow transfers->$mem_transfers_str policy->$policy_str rep->${reps}/${N_REPS} queues->$queue version->ctrl"
								for size in ${MATPOW_SIZES[@]}; do
									echo -n "$arch, ctrl, $policy_str, $queue, $host_aff, $size, $matpow_iters, $extra_args" >> $RESULT_PATH/matrix_pow/$arch/ctrl.log
									srun $SLURM_FLAGS $MATPOW_PATH/Matrix_Power_Of_${arch_name}_Ctrl $size $matpow_iters $extra_params $policy $host_aff >> $RESULT_PATH/matrix_pow/$arch/ctrl.log
								done
							fi

							# SOBEL
							if [[ " ${benchmarks[@]} " =~ " sobel_yuv " ]]; then
								echo "$(date +"%T") Progress update: executing $arch sobel transfers->$mem_transfers_str policy->$policy_str rep->${reps}/${N_REPS} queues->$queue version->ctrl"
								for base in ${sobel_baselines[@]}; do
									echo -n "$arch, ctrl, $policy_str, $queue, $host_aff, $base, $sobel_width, $sobel_height, $sobel_frames, $extra_args" >> $RESULT_PATH/sobel_yuv/$arch/ctrl.log
									srun $SLURM_FLAGS $SOBEL_PATH/Sobel_YUV_${arch_name}_Ctrl_$base $sobel_width $sobel_height $sobel_frames $sobel_in $sobel_out $extra_params $policy $host_aff >> $RESULT_PATH/sobel_yuv/$arch/ctrl.log
									if [[ $(srun $SLURM_DEFAULT_FLAGS diff $sobel_ref $sobel_out) ]]; then
										echo "ERROR: missmatch in sobel output on: srun $SLURM_FLAGS $SOBEL_PATH/Sobel_YUV_${arch_name}_Ctrl_$base $sobel_width $sobel_height $sobel_frames $sobel_in $sobel_out $extra_params $policy $host_aff"
									fi
									srun $SLURM_DEFAULT_FLAGS rm -f $sobel_out
								done
							fi
						done
					done
				done
			done
		fi

		# reference benchmarks
		if [[ ! " ${versions[@]} " =~ " ctrl " ]]; then 
			bash compile.sh -a $arch -e
		fi
		if [[ " ${versions[@]} " =~ " ref " ]]; then
			if [ $arch == "cpu" ]; then
				policies=(Sync)
				extra_args="${extra_args_default}null, null, "
				extra_params="$extra_params_default"
			else
				policies=(Sync Async)
			fi
			for reps in $(seq 1 $N_REPS) ; do
				for policy in "${policies[@]}"; do
					# HOTSPOT
					if [[ " ${benchmarks[@]} " =~ " hotspot " ]]; then
						echo "$(date +"%T") Progress update: executing $arch hotspot policy->$policy rep->${reps}/${N_REPS} version->ref"
						for size in ${HOTSPOT_SIZES[@]}; do
							echo -n "$arch, ref, ${policy,,}, null, $host_aff, $size, $hotspot_height_str$hotspot_iters, $hotspot_iter_per_cpy, $extra_args" >> $RESULT_PATH/hotspot/$arch/ref.log
							srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff --membind node:$host_aff $HOTSPOT_PATH/Hotspot_${arch_name}_Ref_$policy $size $hotspot_height $hotspot_iters $hotspot_iter_per_cpy $extra_params >> $RESULT_PATH/hotspot/$arch/ref.log
						done
					fi

					# MATPOW
					if [[ " ${benchmarks[@]} " =~ " matrix_pow " ]]; then
						echo "$(date +"%T") Progress update: executing $arch matpow policy->$policy rep->${reps}/${N_REPS} version->ref"
						for size in ${MATPOW_SIZES[@]}; do
							echo -n "$arch, ref, ${policy,,}, null, $host_aff, $size, $matpow_iters, $extra_args" >> $RESULT_PATH/matrix_pow/$arch/ref.log
							srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff --membind node:$host_aff $MATPOW_PATH/Matrix_Power_Of_${arch_name}_Ref_$policy $size $matpow_iters $extra_params >> $RESULT_PATH/matrix_pow/$arch/ref.log
						done
					fi

					# SOBEL
					if [[ " ${benchmarks[@]} " =~ " sobel_yuv " ]]; then
						echo "$(date +"%T") Progress update: executing $arch sobel policy->$policy rep->${reps}/${N_REPS} version->ref"
						for base in ${sobel_baselines[@]}; do
							echo -n "$arch, ref, ${policy,,}, null, $host_aff, $base, $sobel_width, $sobel_height, $sobel_frames, $extra_args" >> $RESULT_PATH/sobel_yuv/$arch/ref.log
							srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff --membind node:$host_aff $SOBEL_PATH/Sobel_YUV_${arch_name}_Ref_${policy}_$base $sobel_width $sobel_height $sobel_frames $sobel_in $sobel_out $extra_params >> $RESULT_PATH/sobel_yuv/$arch/ref.log
							if [[ $(srun $SLURM_FLAGS diff $sobel_ref $sobel_out) ]]; then
								echo "ERROR: mismatch in sobel output on: srun $SLURM_FLAGS $SOBEL_PATH/Sobel_YUV_${arch_name}_Ref_${policy}_$base $sobel_width $sobel_height $sobel_frames $sobel_in $sobel_out" 
							fi
							srun $SLURM_FLAGS rm -f $sobel_out
						done
					fi
				done
			done
		fi
	done
done
echo "Experimentation finished."