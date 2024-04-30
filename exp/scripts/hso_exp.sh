#!/bin/bash

# get on controllers dir
cd "$(dirname "${BASH_SOURCE[0]}")"
cd ../..

while getopts ":v:m:n:hc:o:f:s:a" opt; do
	case $opt in
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
			machine=$OPTARG
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
		c)
			compiler=$OPTARG
			echo "Using compiler: $OPTARG"
			;;
		o)
			res_dirname=$OPTARG
			echo "Storing results on $OPTARG"
			;;
		f)
			ctrl_conf_file=$OPTARG
			echo "Using conf file $ctrl_conf_file for ctrl"
			;;
		s)
			streams=$OPTARG
			echo "Using $streams extra streams in ctrl."
			;;
		a)
			append=true
			;;
		h)
			echo "--------Controllers experimentation script--------"
			echo "This script launches a experimentation of the benchmarks and architectures selected on the nodes selected."
			echo "Usage: bash run_exp.sh [OPTIONS]"
			echo "	-h				Show this help."
			echo "	-o output		Results folder name. Default results."
			echo "	-v version		Select versions to test. Valid values: ctrl, ref."
			echo "					By default executes both."
			echo "	-m machine		Machine to execute experimentation on."
			echo "					A <machine>.config file must be present in order to use it. Check manticore.config as an example of config file format."
			echo "					Default is manticore."
			echo "	-c compiler		Especify a compiler to use. Default is gcc. NOTE: care for set environments, oneapi and aocc are loaded in this script but oneapi env can override aocc env."
			echo "	-f conf file	Path to conf file to use for ctrl. Default ./examples/Device_Selection_Files/dev_all_1proc_exp"
			echo "	-s streams		Number of extra kernel streams to use in ctrl (see EXTRA_STREAMS macro in ctrl)."
			echo "	-a 				Append on output files to existing results."
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
versions=(${versions[@]:-"ref" "ctrl"})
machine=${machine:-"manticore"}
N_REPS=${N_REPS:-"30"}
res_dirname=${res_dirname:-"results"}
ctrl_conf_file=${ctrl_conf_file:-"./examples/Device_Selection_Files/dev_all_1proc_exp"}
streams=${streams:-"1"}
compiler=${compiler:-"gcc"}

case $machine in
	gorgon)
		host_aff="7"
		case "$compiler" in
			icc)
				arch_flags="-march=core-avx2"
				;;
			aocc | gcc)
				arch_flags="-march=znver3"
				;;
			*) ;;
		esac
		;;
	manticore)
		host_aff="0"
		case "$compiler" in
			icc | gcc)
				arch_flags="-march=skylake-avx512"
				;;
			aocc)
				echo "ERROR compiler not compatible with this machine"
				;;
			*) ;;
		esac
		;;
esac

case "$compiler" in
	icc)
		. /opt/intel/oneapi/setvars.sh
		;;
	aocc)
		. /opt/AMD/aocc-compiler-3.1.0/setenv_AOCC.sh
		;;
	*) ;;
esac

. ./env.sh

SLURM_FLAGS="--exclusive -Q -p dark-night -t 10 -w $machine --gres=gpu:4"
res_path=./exp/${res_dirname}/${machine}/raw

frames_list=(2 5 10 20 40)
alpha="0.2"
levels="5"
solves="500"
warps="3"
dev="0"
platform="1" # 0 is nvidia 1 is amd
video_path="/home/sergioalo/8k_ppm/gold"
export CTRL_POLICY=ASYNC

mkdir -p $res_path/hso/cuda/iter
mkdir -p $res_path/hso/openclamd/iter

if [ ! $append ]; then
	legend="arch, version, policy, alpha, levels, solves, warps, frames, platform, devices, sum, res, main_clock, exec_clock"
	echo $legend >$res_path/hso/cuda/iter/ctrl.log
	echo $legend >$res_path/hso/cuda/iter/ref.log
	echo $legend >$res_path/hso/openclamd/iter/ctrl.log
	echo $legend >$res_path/hso/openclamd/iter/ref.log
fi

# compile
bash compile.sh -a cuda,opencl,cpu -e --cc $compiler --flags "${arch_flags},-DEXTRA_STREAMS=${streams}"

for i in $(seq $N_REPS); do
	echo "$(date +"%T") Progress update: rep->${i}/${N_REPS}"
	for frames in "${frames_list[@]}"; do
		# ctrl
		if [[ " ${versions[@]} " =~ " ctrl " ]]; then
			# echo -n "cuda, ctrl, async, $alpha, $levels, $solves, $warps, $frames, null, " >>$res_path/hso/cuda/iter/ctrl.log
			# srun $SLURM_FLAGS build/examples/HSOpticalFlow/HSOpticalFlow_Ctrl_Multi $alpha $levels $solves $warps $frames $video_path $ctrl_conf_file >>$res_path/hso/cuda/iter/ctrl.log

			# echo -n "openclamd, ctrl, async, $alpha, $levels, $solves, $warps, $frames, oclamd, " >>$res_path/hso/openclamd/iter/ctrl.log
			# srun $SLURM_FLAGS build/examples/HSOpticalFlow/HSOpticalFlow_Ctrl_Multi $alpha $levels $solves $warps $frames $video_path $ctrl_conf_file >>$res_path/hso/openclamd/iter/ctrl.log

			echo -n "multi, ctrl, async, $alpha, $levels, $solves, $warps, $frames, multi, " >>$res_path/hso/openclamd/iter/ctrl.log
			srun $SLURM_FLAGS build/examples/HSOpticalFlow/HSOpticalFlow_Ctrl_Multi $alpha $levels $solves $warps $frames $video_path $ctrl_conf_file >>$res_path/hso/openclamd/iter/ctrl.log
		fi
		# cuda & opencl
		if [[ " ${versions[@]} " =~ " ref " ]]; then
			# echo -n "cuda, ref, sync, $alpha, $levels, $solves, $warps, $frames, null, " >>$res_path/hso/cuda/iter/ref.log
			# srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff build/examples/HSOpticalFlow/HSOpticalFlow_Cuda_Ref_Sync_Multi $alpha $levels $solves $warps $dev $frames $video_path >>$res_path/hso/cuda/iter/ref.log

			# echo -n "cuda, ref, async, $alpha, $levels, $solves, $warps, $frames, null, " >>$res_path/hso/cuda/iter/ref.log
			# srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff build/examples/HSOpticalFlow/HSOpticalFlow_Cuda_Ref_Async_Multi $alpha $levels $solves $warps $dev $frames $video_path >>$res_path/hso/cuda/iter/ref.log

			echo -n "openclamd, ref, sync, $alpha, $levels, $solves, $warps, $frames, " >>$res_path/hso/openclamd/iter/ref.log
			srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff build/examples/HSOpticalFlow/HSOpticalFlow_OpenCLGPU_Ref_Sync_Multi $alpha $levels $solves $warps $dev $platform $frames $video_path >>$res_path/hso/openclamd/iter/ref.log

			echo -n "openclamd, ref, async, $alpha, $levels, $solves, $warps, $frames, " >>$res_path/hso/openclamd/iter/ref.log
			srun $SLURM_FLAGS hwloc-bind --cpubind node:$host_aff build/examples/HSOpticalFlow/HSOpticalFlow_OpenCLGPU_Ref_Sync_Multi_img $alpha $levels $solves $warps $dev $platform $frames $video_path >>$res_path/hso/openclamd/iter/ref.log
		fi
	done
done
echo "$(date +"%T") Experimentation complete"
