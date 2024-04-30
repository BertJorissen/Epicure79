#! /bin/bash

cd "$(dirname "${BASH_SOURCE[0]}")"
cd ../..

reps=5
procs=(2 3 4)
res_path=./exp/res_hso/gorgon/raw
mkdir -p $res_path/hso/cuda/procs

alpha="0.2"
levels="5"
solves="500"
warps="3"
cuda_dev="0"
video_path="/home/sergioalo/8k_ppm/gold"
frames="40"

if [ ! $append ]; then
	legend="arch, version, policy, alpha, levels, solves, warps, frames, procs, platform, devices, sum, res, main_clock, exec_clock"
	echo $legend >$res_path/hso/cuda/procs/ref.log
fi

for i in $(seq $reps); do
	for p in "${procs[@]}"; do
		echo "$(date +"%T") Progress update: rep->${i}/${N_REPS} procs->${p}"

		echo -n "cuda, ref, async, $alpha, $levels, $solves, $warps, $frames, $p, null, " >>$res_path/hso/cuda/procs/ref.log
		echo "srun --exclusive -Q -p dark-night -w gorgon --gres=gpu:4 -n $p hwloc-bind --cpubind node:7 build/examples/HSOpticalFlow/HSOpticalFlow_Cuda_Ref_Async_Multi $alpha $levels $solves $warps $cuda_dev $frames $video_path >>$res_path/hso/cuda/procs/ref.log"
		srun --exclusive -Q -p dark-night -w gorgon --gres=gpu:4 -n $p hwloc-bind --cpubind node:7 build/examples/HSOpticalFlow/HSOpticalFlow_Cuda_Ref_Async_Multi $alpha $levels $solves $warps $cuda_dev $frames $video_path >>$res_path/hso/cuda/procs/ref.log
	done
done
