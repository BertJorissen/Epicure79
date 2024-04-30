#!/bin/bash

# get on controllers dir
cd "$(dirname "${BASH_SOURCE[0]}")"
cd ../..

. ./env.sh

machine="manticore"
SLURM_FLAGS="--exclusive -Q -p dark-night -w $machine --gres=gpu:4"
ctrl_conf_dir="./examples/Device_Selection_Files"
res_path=./exp/results/${machine}/raw
reps=30

sizes=(1024 1536 2048 4096 6144 8192)
iter=20
pol=1 # async

mkdir -p $res_path/chain_matmult/all/size
mkdir -p $res_path/chain_matmult/cuda/size
mkdir -p $res_path/chain_matmult/opencl/size
mkdir -p $res_path/chain_matmult/openclamd/size
legend="arch, version, policy, size, iter, device, platform, sum, res, main_clock, exec_clock"
echo $legend >$res_path/chain_matmult/all/size/ctrl.log
echo $legend >$res_path/chain_matmult/cuda/size/ref.log
echo $legend >$res_path/chain_matmult/opencl/size/ref.log
echo $legend >$res_path/chain_matmult/openclamd/size/ref.log

# compile
bash compile.sh -a cuda,opencl,cpu -e --cc gcc --flags -march=skylake-avx512

for i in $(seq $reps); do
	echo "$(date +"%T") Progress update: rep->${i}/${reps}"
	for s in "${sizes[@]}"; do
		# ctrl
		echo -n "all, ctrl, async, $s, $iter, all, all, " >>$res_path/chain_matmult/all/size/ctrl.log
		srun $SLURM_FLAGS ./build/examples/Chain_MatMult/Chain_MatMult_Multidev_Ctrl $s $iter $pol $ctrl_conf_dir/dev_all_1proc_exp >>$res_path/chain_matmult/all/size/ctrl.log
		# cuda
		echo -n "cuda, ref, async, $s, $iter, " >>$res_path/chain_matmult/cuda/size/ref.log
		srun $SLURM_FLAGS ./build/examples/Chain_MatMult/Chain_MatMult_Cuda_Ref_Async $s $iter 0 >>$res_path/chain_matmult/cuda/size/ref.log
		# ocl nv
		echo -n "openclnv, ref, async, $s, $iter, " >>$res_path/chain_matmult/opencl/size/ref.log
		srun $SLURM_FLAGS ./build/examples/Chain_MatMult/Chain_MatMult_OpenCL_Gpu_Ref_Async $s $iter 0 0 >>$res_path/chain_matmult/opencl/size/ref.log
		# ocl amd
		echo -n "openclamd, ref, async, $s, $iter, " >>$res_path/chain_matmult/openclamd/size/ref.log
		srun $SLURM_FLAGS ./build/examples/Chain_MatMult/Chain_MatMult_OpenCL_Gpu_Ref_Async $s $iter 0 1 >>$res_path/chain_matmult/openclamd/size/ref.log
	done
done
