import matplotlib
import config
import subprocess as sp
from typing import TextIO
import os
import matplotlib.pyplot as plt
import utils


def get_legend():
	return "version, policy, affinity, alpha, levels, solves, warps, device_args, sum, res, main_clock, exec_clock"


def run(config: config.Config, output: TextIO, version: str):
	# config sanity checks
	assert (len(config.procs) == 1)
	proc = config.procs[0]
	assert (len(proc.devices) == 1)
	dev = proc.devices[0]

	# common HSOpticalFlow values
	path = os.path.join(utils.get_ctrl_dir(), "build", "examples", "HSOpticalFlow")
	srun = f"srun --export=ALL,CTRL_POLICY=ASYNC,SYCL_PI_CUDA_ENABLE_IMAGE_SUPPORT=1,ONEAPI_DEVICE_SELECTOR=cuda:{dev.get_ref_params()} --exclusive -Q -p dark-night -K -t 20 -w {proc.node}"
	alpha = "0.2"
	levels = "5"
	solves = "500"
	warps = "3"
	f_src = "/home/sergioalo/8k_ppm/gold/001.ppm"
	f_tgt = "/home/sergioalo/8k_ppm/gold/002.ppm"
	f_out = "/tmp/hso_out.flo"

	if version.startswith("ctrl"):
		hwloc_bind = ""
		app_path = f"{path}/HSOpticalFlow_Ctrl"
		if version.endswith("_nopin"):
			app_path += "_nopin"
		dev_args = config.path
	elif version.startswith("ref"):
		hwloc_bind = f"hwloc-bind --cpubind node:{proc.affinity} --membind node:{proc.affinity}"
		app_path = f"{path}/HSOpticalFlow_{utils.arch_formatter[dev.arch]}_Ref"
		if not version.endswith("_nopin"):
			app_path += "_pin"
		dev_args = dev.get_ref_params()
	elif version == "sycl":
		hwloc_bind = f"hwloc-bind --cpubind node:{proc.affinity} --membind node:{proc.affinity}"
		app_path = "/frontend/sergioalo/oneAPI-samples/DirectProgramming/C++SYCL/StructuredGrids/guided_HSOpticalFlow_SYCLMigration/build/bin/02_sycl_migrated_optimized"
	else:
		raise ValueError(f"Unknown version for HSOpticalFlow benchmark {version}")

	cmd = f"{srun} {hwloc_bind} {app_path}"
	if version != "sycl":
		cmd += f" {alpha} {levels} {solves} {warps} {f_src} {f_tgt} {f_out} {dev_args}"

	# run command and write to output file id successful
	res = sp.run(cmd, shell = True, capture_output = True, text = True)

	print(res.stderr, end = "")
	if res.returncode == 0:
		output.write(f"{version}, async, {proc.affinity}, {alpha}, {levels}, {solves}, {warps}, {res.stdout}")


def plot(conf_dir: os.DirEntry):
	plt.rcParams.update({'font.size': 14})

	# title
	plt.title(conf_dir.name.removeprefix("agg_").replace("_", " ").title())
	# conf_name_l = [s.title() for s in conf_dir.name.removeprefix("agg_").split("_")]
	# if len(conf_name_l) > 1:
	# 	conf_name_l[-1] = conf_name_l[-1].upper()
	# plt.title(" ".join(conf_name_l))

	# Set labels
	plt.ylabel("Time (s.)")

	# Set tick properties
	plt.tick_params(which = 'both', direction = 'in', top = True, right = True)
	plt.tick_params(axis = 'x', which = 'both', bottom = False, top = False)
	name_translate = {
	    "ctrl": "Ctrl",
	    "ref": "Cuda",
	    "sycl": "Sycl",
	    "ref_pin": "Cuda Pin",
	    "ctrl_nopin": "Ctrl",
	}

	for result in sorted(os.scandir(os.path.join(conf_dir.path, "stats")), key = lambda x: x.name):
		with open(result, "r") as f_in:
			# legend
			f_in.readline()
			# data: [[x_axis], [y_axis]]
			data = list(zip(*[l.split(", ") for l in f_in]))[:2]
			data[1] = list(map(float, data[1]))

			x = list(range(len(data[0])))
			data[0] = [name_translate[name] for name in data[0]]
			plt.bar(x, data[1], 0.8)
			plt.xticks(x, data[0])

			# Set x axis range
			plt.xlim(-0.5, len(data[0]) - 0.5)

	# Set grid and ticks
	plt.grid(axis = 'y', which = 'both')
	# plt.legend()
	plt.savefig(os.path.join(conf_dir.path, f"{conf_dir.name}_hsopticalflow_1f.pdf"), format = "pdf")
	plt.close()
