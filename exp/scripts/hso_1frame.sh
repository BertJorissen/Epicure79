#!/bin/bash

# get on controllers dir
cd "$(dirname "${BASH_SOURCE[0]}")"
cd ../..

slurm_flags="-w manticore --exclusive"

# default values for args
versions=(${versions[@]:-"ref" "ctrl"})
N_REPS=${N_REPS:-"10"}
res_dirname=${res_dirname:-"hso_sycl_res"}
ctrl_conf_file=${ctrl_conf_file:-"./examples/Device_Selection_Files/dev_cuda_exp"}
compiler=${compiler:-"gcc"}

res_path=./exp/${res_dirname}/manticore/raw
mkdir -p $res_path/hso/cuda/iter

legend="arch, version, policy, alpha, levels, solves, warps, device, sum, res, main_clock, exec_clock"
echo $legend >$res_path/hso/cuda/iter/res.log

alpha="0.2"
levels="5"
solves="500"
warps="3"
dev="0"
f_src="examples/HSOpticalFlow/input.big/frame10.ppm"
f_tgt="examples/HSOpticalFlow/input.big/frame11.ppm"
f_out="examples/HSOpticalFlow/output/FlowGPU.flo"

export SYCL_PI_CUDA_ENABLE_IMAGE_SUPPORT=1

for i in $(seq $N_REPS); do
	echo "$(date +"%T") Progress update: rep->${i}/${N_REPS}"

	echo -n "cuda, ref, nopin, $alpha, $levels, $solves, $warps, " >>$res_path/hso/cuda/iter/res.log
	srun $slurm_flags ./build/examples/HSOpticalFlow/HSOpticalFlow_Cuda_Ref $alpha $levels $solves $warps $dev $f_src $f_tgt $f_out >>$res_path/hso/cuda/iter/res.log

	echo -n "cuda, ref, pin, $alpha, $levels, $solves, $warps, " >>$res_path/hso/cuda/iter/res.log
	srun $slurm_flags ./build/examples/HSOpticalFlow/HSOpticalFlow_Cuda_Ref_pin $alpha $levels $solves $warps $dev $f_src $f_tgt $f_out >>$res_path/hso/cuda/iter/res.log

	echo -n "cuda, ctrl, pin, $alpha, $levels, $solves, $warps, " >>$res_path/hso/cuda/iter/res.log
	srun $slurm_flags ./build/examples/HSOpticalFlow/HSOpticalFlow_Ctrl $alpha $levels $solves $warps $f_src $f_tgt $f_out $ctrl_conf_file >>$res_path/hso/cuda/iter/res.log

	echo -n "cuda, ctrl, nopin, $alpha, $levels, $solves, $warps, " >>$res_path/hso/cuda/iter/res.log
	srun $slurm_flags ./build/examples/HSOpticalFlow/HSOpticalFlow_Ctrl_nopin $alpha $levels $solves $warps $f_src $f_tgt $f_out $ctrl_conf_file >>$res_path/hso/cuda/iter/res.log

	echo -n "cuda, sycl, pin, $alpha, $levels, $solves, $warps, " >>$res_path/hso/cuda/iter/res.log
	srun $slurm_flags /frontend/sergioalo/oneAPI-samples/DirectProgramming/C++SYCL/StructuredGrids/guided_HSOpticalFlow_SYCLMigration/build/bin/02_sycl_migrated_optimized >>$res_path/hso/cuda/iter/res.log
done
echo "$(date +"%T") Experimentation complete"
