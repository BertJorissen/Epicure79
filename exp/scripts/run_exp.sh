#!/bin/bash

# get on controllers dir
cd "$(dirname "${BASH_SOURCE[0]}")"
cd ../..

while getopts ":a:b:v:m:n:hM:c:o:" opt; do
	case $opt in
		a)
			archs=(${OPTARG//,/ })
			for arch in "${archs[@]}"; do
				if [ $arch != "cpu" ] && [ $arch != "cuda" ] && [ $arch != "openclnv" ] && [ $arch != "openclamd" ] && [ $arch != "hip" ]; then
					echo "unknown arch $arch. Use -h for help."
					exit
				fi
			done
			echo "Executing for archs: $OPTARG"
			;;
		b)
			benchmarks=(${OPTARG//,/ })
			for bench in "${benchmarks[@]}"; do
				if [ $bench != "hotspot" ] && [ $bench != "matrix_pow" ] && [ $bench != "sobel_yuv" ]; then
					echo "unknown benchmark $bench. Use -h for help."
					exit
				fi
			done
			echo "Executing benchmarks: $OPTARG"
			;;
		v)
			versions=(${OPTARG//,/ })
			for v in "${versions[@]}"; do
				if [ $v != "ref" ] && [ $v != "ctrl" ]; then
					echo "unknown version $v. Use -h for help."
					exit
				fi
			done
			echo "Executing versions: $OPTARG"
			;;
		m)
			machines=(${OPTARG//,/ })
			for m in "${machines[@]}"; do
				if [[ ! -f "./exp/scripts/machine_configs/${m}.config" ]]; then
					echo "Config file for machine $m not found. Use -h for help."
					exit
				fi
			done
			echo "Executing on $OPTARG"
			;;
		n)
			N_REPS=$OPTARG
			echo "reps = $N_REPS"
			;;
		M)
			echo "Experimentation modes: $OPTARG"
			modes=(${OPTARG//,/ })
			for m in "${modes[@]}"; do
				if [ $m != "size" ] && [ $m != "iter" ]; then
					echo "unknown mode option $m. Use -h for help."
					exit
				fi
			done
			;;
		c)
			compiler=$OPTARG
			echo "Using compiler: $OPTARG"
			;;
		o)
			res_dirname=$OPTARG
			echo "Storing results on $OPTARG"
			;;
		h)
			echo "--------Controllers experimentation script--------"
			echo "This script launches a experimentation of the benchmarks and architectures selected on the nodes selected."
			echo "Usage: bash run_exp.sh [OPTIONS]"
			echo "	-h				Show this help."
			echo "	-o output		Results folder name. Default results."
			echo "	-a archs		Select architectures to test."
			echo "					Comma separated. Valid values: cuda, cpu, openclnv, openclamd, hip."
			echo "					By default executes everything."
			echo "	-b benches		Select benchmarks to execute."
			echo "					Comma separated. Valid values: hotspot, matrix_pow, sobel_yuv."
			echo "					By default executes everything."
			echo " 	-v version		Select versions to test. Valid values: ctrl, ref."
			echo "					By default executes both."
			echo "	-m machines		Machines to execute experimentation on."
			echo "					Comma separated. A <machine>.config file must be present in order to use it. Check manticore.config as an example of config file format."
			echo "					If a machine selected is not compatible with some of the architectures selected those architectures will be skipped for that specific machine."
			echo "					Default is manticore."
			echo "	-c compiler		Especify a compiler to use. Default is gcc. NOTE: care for set environments, oneapi and aocc are loaded in this script but oneapi env can override aocc env."
			echo "	-n reps			Number of repetitions to be done by this script. Default is 30."
			echo "	-M modes		Experimetation modes. Comma separated. Determines what will be the variable for this experimentation. Valid values are size, iter. Default is both."
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
archs=(${archs[@]:-"cpu" "cuda" "openclnv" "openclamd" "hip"})
benchmarks=(${benchmarks[@]:-"hotspot" "matrix_pow" "sobel_yuv"})
versions=(${versions[@]:-"ref" "ctrl"})
machines=(${machines[@]:-"manticore"})
modes=(${modes[@]:-"size" "iter"})
N_REPS=${N_REPS:-"30"}
compiler=${compiler:-"gcc"}
res_dirname=${res_dirname:-"results"}

case "$compiler" in
	icc)
		. /opt/intel/oneapi/setvars.sh
		;;
	aocc)
		. /opt/AMD/aocc-compiler-3.1.0/setenv_AOCC.sh
		;;
	*) ;;
esac

# load modules and get on frontend dir
. ./env.sh

# common hotspot values
HOTSPOT_PATH=./build/examples/Hotspot
hotspot_size=10000
hotspot_iters=500
hotspot_iters_list=(100 200 300 400 500 600)

# common matpow values
MATPOW_PATH=./build/examples/Matrix_Power_Of
matpow_size=2048
matpow_iters=40
matpow_iters_list=(20 40 60 80 100 120)

# common sobel values
SOBEL_PATH=./build/examples/Sobel_YUV
sobel_in=/home/video/BigBuckBunny_1920_1080_24fps_1500fs.yuv
sobel_out=/tmp/out.yuv
sobel_ref=/tmp/reference_sobel_out.yuv
sobel_width=1920
sobel_height=1080
sobel_frames=100
sobel_frames_list=(20 30 40 50 60 70 100)
sobel_baselines=("FTF" "MTF" "MTM" "FTM")

declare -A legends
for machine in "${machines[@]}"; do
	case $machine in
		gorgon)
			case "$compiler" in
				icc)
					flags="--flags -march=core-avx2"
					;;
				aocc | gcc)
					flags="--flags -march=znver3"
					;;
				*) ;;
			esac
			;;
		manticore)
			case "$compiler" in
				icc | gcc)
					flags="--flags -march=skylake-avx512"
					;;
				aocc)
					echo "ERROR compiler not compatible with this machine"
					;;
				*) ;;
			esac
			;;
	esac
	# default flags for srun
	SLURM_DEFAULT_FLAGS="--exclusive -Q -p dark-night -K -t 5 -w $machine"

	# result output dir
	RESULT_PATH=./exp/${res_dirname}/${machine}/raw
	mkdir -p $RESULT_PATH

	# ctrl config files path
	ctrl_conf_dir=./examples/Device_Selection_Files

	for arch in "${archs[@]}"; do
		. ./exp/scripts/machine_configs/${machine}.config
		case $arch in
			cpu)
				# values for CPU
				arch_name="Cpu"
				extra_params="$n_threads"
				hotspot_iter_per_cpy=4

				hotspot_height=""
				hotspot_height_str=""

				legends["hotspot"]="arch, version, policy, host_aff, size, iter, iters_per_copy, n_threads, device, mem_transfers, sum, result, main_clock, exec_clock"
				legends["matrix_pow"]="arch, version, policy, host_aff, size, iter, n_threads, device, mem_transfers, sum, res, main_clock, exec_clock"
				legends["sobel_yuv"]="arch, version, policy, host_aff, baseline, width, height, frames, n_threads, device, mem_transfers, main_clock, exec_clock"
				;;
			cuda)
				# values for CUDA
				arch_name="Cuda"
				extra_params="$device"

				hotspot_height=4
				hotspot_height_str="4, "
				hotspot_iter_per_cpy=1

				legends["hotspot"]="arch, version, policy, host_aff, size, height, iter, iters_per_copy, device, sum, result, main_clock, exec_clock"
				legends["matrix_pow"]="arch, version, policy, host_aff, size, iter, device, sum, res, main_clock, exec_clock"
				legends["sobel_yuv"]="arch, version, policy, host_aff, baseline, width, height, frames, device, main_clock, exec_clock"
				;;
			openclnv)
				# values for openclnv
				arch_name="OpenCL_Gpu"
				extra_params="$device $platform"

				hotspot_height=4
				hotspot_height_str="4, "
				hotspot_iter_per_cpy=1

				legends["hotspot"]="arch, version, policy, host_aff, size, height, iter, iters_per_copy, device, platform, sum, result, main_clock, exec_clock"
				legends["matrix_pow"]="arch, version, policy, host_aff, size, iter, device, platform, sum, res, main_clock, exec_clock"
				legends["sobel_yuv"]="arch, version, policy, host_aff, baseline, width, height, frames, device, platform, main_clock, exec_clock"
				;;
			openclamd)
				# values for openclamd
				arch_name="OpenCL_Gpu"
				extra_params="$device $platform"

				hotspot_height=4
				hotspot_height_str="4, "
				hotspot_iter_per_cpy=1

				legends["hotspot"]="arch, version, policy, host_aff, size, height, iter, iters_per_copy, device, platform, sum, result, main_clock, exec_clock"
				legends["matrix_pow"]="arch, version, policy, host_aff, size, iter, device, platform, sum, res, main_clock, exec_clock"
				legends["sobel_yuv"]="arch, version, policy, host_aff, baseline, width, height, frames, device, platform, main_clock, exec_clock"
				;;
			hip)
				# values for HIP
				arch_name="Hip"
				extra_params="$device"

				hotspot_height=4
				hotspot_height_str="4, "
				hotspot_iter_per_cpy=1

				legends["hotspot"]="arch, version, policy, host_aff, size, height, iter, iters_per_copy, device, sum, result, main_clock, exec_clock"
				legends["matrix_pow"]="arch, version, policy, host_aff, size, iter, device, sum, res, main_clock, exec_clock"
				legends["sobel_yuv"]="arch, version, policy, host_aff, baseline, width, height, frames, device, main_clock, exec_clock"
				;;
		esac

		#write legends on files
		for b in "${benchmarks[@]}"; do
			for v in "${versions[@]}"; do
				for m in "${modes[@]}"; do
					mkdir -p $RESULT_PATH/$b/$arch/$m
					echo ${legends[$b]} >$RESULT_PATH/$b/$arch/$m/${v}.log
				done
			done
		done

		case $arch in
			*"opencl"*) archcmp="opencl" ;;
			*) archcmp=$arch ;;
		esac

		bash compile.sh -a $archcmp -e --cc $compiler $flags
		#ctrl benchmarks
		if [[ " ${versions[@]} " =~ " ctrl " ]]; then
			for reps in $(seq 1 $N_REPS); do
				for policy in {0,1}; do
					if [ $policy == 0 ]; then
						policy_str=sync
					elif [ $policy == 1 ]; then
						policy_str=async
					fi

					if [ $arch != "cpu" ]; then
						transfers=("off")
					fi
					for mem_transfers in "${transfers[@]}"; do
						if [ $arch == "cpu" ]; then
							ctrl_conf="${ctrl_conf_dir}/dev_${arch}_${mem_transfers}_exp"
						else
							ctrl_conf="${ctrl_conf_dir}/dev_${arch}_exp"
						fi

						# HOTSPOT
						if [[ " ${benchmarks[@]} " =~ " hotspot " ]]; then
							echo "$(date +"%T") Progress update: executing $arch hotspot transfers->$mem_transfers policy->$policy_str rep->${reps}/${N_REPS} version->ctrl"
							if [[ " ${modes[@]} " =~ " size " ]]; then
								for size in ${HOTSPOT_SIZES[@]}; do
									echo -n "$arch, ctrl, $policy_str, $host_aff, $size, $hotspot_height_str$hotspot_iters, $hotspot_iter_per_cpy, " >>$RESULT_PATH/hotspot/$arch/size/ctrl.log
									srun $SLURM_FLAGS $HOTSPOT_PATH/Hotspot_${arch_name}_Ctrl $size $hotspot_height $hotspot_iters $hotspot_iter_per_cpy $policy $ctrl_conf >>$RESULT_PATH/hotspot/$arch/size/ctrl.log
								done
							fi
							if [[ " ${modes[@]} " =~ " iter " ]]; then
								for niter in ${hotspot_iters_list[@]}; do
									echo -n "$arch, ctrl, $policy_str, $host_aff, $hotspot_size, $hotspot_height_str$niter, $hotspot_iter_per_cpy, " >>$RESULT_PATH/hotspot/$arch/iter/ctrl.log
									srun $SLURM_FLAGS $HOTSPOT_PATH/Hotspot_${arch_name}_Ctrl $hotspot_size $hotspot_height $niter $hotspot_iter_per_cpy $policy $ctrl_conf >>$RESULT_PATH/hotspot/$arch/iter/ctrl.log
								done
							fi
						fi

						# MATPOW
						if [[ " ${benchmarks[@]} " =~ " matrix_pow " ]]; then
							echo "$(date +"%T") Progress update: executing $arch matpow transfers->$mem_transfers policy->$policy_str rep->${reps}/${N_REPS} version->ctrl"
							if [[ " ${modes[@]} " =~ " size " ]]; then
								for size in ${MATPOW_SIZES[@]}; do
									echo -n "$arch, ctrl, $policy_str, $host_aff, $size, $matpow_iters, " >>$RESULT_PATH/matrix_pow/$arch/size/ctrl.log
									srun $SLURM_FLAGS $MATPOW_PATH/Matrix_Power_Of_${arch_name}_Ctrl $size $matpow_iters $policy $ctrl_conf >>$RESULT_PATH/matrix_pow/$arch/size/ctrl.log
								done
							fi
							if [[ " ${modes[@]} " =~ " iter " ]]; then
								for niter in ${matpow_iters_list[@]}; do
									echo -n "$arch, ctrl, $policy_str, $host_aff, $matpow_size, $niter, " >>$RESULT_PATH/matrix_pow/$arch/iter/ctrl.log
									srun $SLURM_FLAGS $MATPOW_PATH/Matrix_Power_Of_${arch_name}_Ctrl $matpow_size $niter $policy $ctrl_conf >>$RESULT_PATH/matrix_pow/$arch/iter/ctrl.log
								done
							fi
						fi

						# SOBEL
						if [[ " ${benchmarks[@]} " =~ " sobel_yuv " ]]; then
							echo "$(date +"%T") Progress update: executing $arch sobel transfers->$mem_transfers policy->$policy_str rep->${reps}/${N_REPS} version->ctrl"
							for base in ${sobel_baselines[@]}; do
								if [[ " ${modes[@]} " =~ " size " ]]; then
									echo -n "$arch, ctrl, $policy_str, $host_aff, $base, $sobel_width, $sobel_height, $sobel_frames, " >>$RESULT_PATH/sobel_yuv/$arch/size/ctrl.log
									srun $SLURM_FLAGS $SOBEL_PATH/Sobel_YUV_${arch_name}_Ctrl_$base $sobel_width $sobel_height $sobel_frames $sobel_in $sobel_out $policy $ctrl_conf >>$RESULT_PATH/sobel_yuv/$arch/size/ctrl.log
									if [[ $(srun $SLURM_DEFAULT_FLAGS diff $sobel_ref $sobel_out) ]]; then
										echo "ERROR: missmatch in sobel output on: srun $SLURM_FLAGS $SOBEL_PATH/Sobel_YUV_${arch_name}_Ctrl_$base $sobel_width $sobel_height $sobel_frames $sobel_in $sobel_out $policy $ctrl_conf"
									fi
									srun $SLURM_DEFAULT_FLAGS rm -f $sobel_out
								fi
								if [[ " ${modes[@]} " =~ " iter " ]]; then
									for niter in "${sobel_frames_list[@]}"; do
										echo -n "$arch, ctrl, $policy_str, $host_aff, $base, $sobel_width, $sobel_height, $niter, " >>$RESULT_PATH/sobel_yuv/$arch/iter/ctrl.log
										srun $SLURM_FLAGS $SOBEL_PATH/Sobel_YUV_${arch_name}_Ctrl_$base $sobel_width $sobel_height $niter $sobel_in $sobel_out $policy $ctrl_conf >>$RESULT_PATH/sobel_yuv/$arch/iter/ctrl.log
										# TODO @sergioalo test output (needs reference output for each niter)
										srun $SLURM_DEFAULT_FLAGS rm -f $sobel_out
									done
								fi
							done
						fi
					done
				done
			done
		fi

		# reference benchmarks
		if [[ " ${versions[@]} " =~ " ref " ]]; then
			if [ $arch == "cpu" ]; then
				policies=(Sync)
				extra_args="$n_threads, null, null, "
			else
				policies=(Sync Async)
				extra_args=""
			fi
			for reps in $(seq 1 $N_REPS); do
				for policy in "${policies[@]}"; do
					# HOTSPOT
					if [[ " ${benchmarks[@]} " =~ " hotspot " ]]; then
						echo "$(date +"%T") Progress update: executing $arch hotspot policy->$policy rep->${reps}/${N_REPS} version->ref"
						if [[ " ${modes[@]} " =~ " size " ]]; then
							for size in ${HOTSPOT_SIZES[@]}; do
								echo -n "$arch, ref, ${policy,,}, $host_aff, $size, $hotspot_height_str$hotspot_iters, $hotspot_iter_per_cpy, $extra_args" >>$RESULT_PATH/hotspot/$arch/size/ref.log
								srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff --membind node:$host_aff $HOTSPOT_PATH/Hotspot_${arch_name}_Ref_$policy $size $hotspot_height $hotspot_iters $hotspot_iter_per_cpy $extra_params >>$RESULT_PATH/hotspot/$arch/size/ref.log
							done
						fi
						if [[ " ${modes[@]} " =~ " iter " ]]; then
							for niter in ${hotspot_iters_list[@]}; do
								echo -n "$arch, ref, ${policy,,}, $host_aff, $hotspot_size, $hotspot_height_str$niter, $hotspot_iter_per_cpy, $extra_args" >>$RESULT_PATH/hotspot/$arch/iter/ref.log
								srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff --membind node:$host_aff $HOTSPOT_PATH/Hotspot_${arch_name}_Ref_$policy $hotspot_size $hotspot_height $niter $hotspot_iter_per_cpy $extra_params >>$RESULT_PATH/hotspot/$arch/iter/ref.log
							done
						fi
					fi

					# MATPOW
					if [[ " ${benchmarks[@]} " =~ " matrix_pow " ]]; then
						echo "$(date +"%T") Progress update: executing $arch matpow policy->$policy rep->${reps}/${N_REPS} version->ref"
						if [[ " ${modes[@]} " =~ " size " ]]; then
							for size in ${MATPOW_SIZES[@]}; do
								echo -n "$arch, ref, ${policy,,}, $host_aff, $size, $matpow_iters, $extra_args" >>$RESULT_PATH/matrix_pow/$arch/size/ref.log
								srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff --membind node:$host_aff $MATPOW_PATH/Matrix_Power_Of_${arch_name}_Ref_$policy $size $matpow_iters $extra_params >>$RESULT_PATH/matrix_pow/$arch/size/ref.log
							done
						fi
						if [[ " ${modes[@]} " =~ " iter " ]]; then
							for niter in ${matpow_iters_list[@]}; do
								echo -n "$arch, ref, ${policy,,}, $host_aff, $matpow_size, $niter, $extra_args" >>$RESULT_PATH/matrix_pow/$arch/iter/ref.log
								srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff --membind node:$host_aff $MATPOW_PATH/Matrix_Power_Of_${arch_name}_Ref_$policy $matpow_size $niter $extra_params >>$RESULT_PATH/matrix_pow/$arch/iter/ref.log
							done
						fi
					fi

					# SOBEL
					if [[ " ${benchmarks[@]} " =~ " sobel_yuv " ]]; then
						echo "$(date +"%T") Progress update: executing $arch sobel policy->$policy rep->${reps}/${N_REPS} version->ref"
						for base in ${sobel_baselines[@]}; do
							if [[ " ${modes[@]} " =~ " size " ]]; then
								echo -n "$arch, ref, ${policy,,}, $host_aff, $base, $sobel_width, $sobel_height, $sobel_frames, $extra_args" >>$RESULT_PATH/sobel_yuv/$arch/size/ref.log
								srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff --membind node:$host_aff $SOBEL_PATH/Sobel_YUV_${arch_name}_Ref_${policy}_$base $sobel_width $sobel_height $sobel_frames $sobel_in $sobel_out $extra_params >>$RESULT_PATH/sobel_yuv/$arch/size/ref.log
								if [[ $(srun $SLURM_FLAGS diff $sobel_ref $sobel_out) ]]; then
									echo "ERROR: mismatch in sobel output on: srun $SLURM_FLAGS $SOBEL_PATH/Sobel_YUV_${arch_name}_Ref_${policy}_$base $sobel_width $sobel_height $sobel_frames $sobel_in $sobel_out"
								fi
								srun $SLURM_FLAGS rm -f $sobel_out
							fi
							if [[ " ${modes[@]} " =~ " iter " ]]; then
								for niter in "${sobel_frames_list[@]}"; do
									echo -n "$arch, ref, ${policy,,}, $host_aff, $base, $sobel_width, $sobel_height, $niter, $extra_args" >>$RESULT_PATH/sobel_yuv/$arch/iter/ref.log
									srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff --membind node:$host_aff $SOBEL_PATH/Sobel_YUV_${arch_name}_Ref_${policy}_$base $sobel_width $sobel_height $niter $sobel_in $sobel_out $extra_params >>$RESULT_PATH/sobel_yuv/$arch/iter/ref.log
									# TODO @sergioalo test output (needs reference output for each niter)
									srun $SLURM_FLAGS rm -f $sobel_out
								done
							fi
						done
					fi
				done
			done
		fi
	done
done
echo "Experimentation finished."
