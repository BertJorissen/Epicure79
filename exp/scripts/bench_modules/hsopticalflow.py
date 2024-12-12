from collections import Counter
import config
import subprocess as sp
from typing import TextIO
import os
import matplotlib.pyplot as plt
import matplotlib
import utils


def get_legend():
	return "version, policy, affinity, alpha, levels, solves, warps, frames, device_args, sum, res, main_clock, exec_clock"


def run(config: config.Config, output: TextIO, version: str):
	# config sanity checks
	assert (len(config.procs) == 1)
	proc = config.procs[0]

	# common HSOpticalFlow values
	path = os.path.join(utils.get_ctrl_dir(), "build", "examples", "HSOpticalFlow")
	srun = f"srun --export=ALL,CTRL_POLICY=ASYNC --exclusive -Q -p dark-night -K -t 20 -w {proc.node}"
	# frames_list = [2]
	frames_list = [2, 5, 10, 20, 40]
	alpha = "0.2"
	levels = "5"
	solves = "500"
	warps = "3"
	video_path = "/home/sergioalo/8k_ppm/gold"

	if version == "ctrl":
		assert (proc.devices[-1].arch == "cpu")
		hwloc_bind = ""
		app_path = f"{path}/HSOpticalFlow_Ctrl_Multi"
		dev_args = config.path
	elif version == "ref":
		# allow configs with one device + cpu for extra host tasks
		if not (len(proc.devices) == 1 or (len(proc.devices) == 2 and proc.devices[1].arch == "cpu")):
			# TODO print warning? delete file somehow?
			return
		dev = proc.devices[0]
		hwloc_bind = f"hwloc-bind --cpubind node:{proc.affinity} --membind node:{proc.affinity}"
		app_path = f"{path}/HSOpticalFlow_{utils.arch_formatter[dev.arch]}_Ref_Multi"
		dev_args = dev.get_ref_params()
	else:
		raise ValueError(f"Unknown version for HSOpticalFlow benchmark {version}")

	# run command and write to output file id successful
	for frames in frames_list:
		res = sp.run(f"{srun} {hwloc_bind} {app_path} {alpha} {levels} {solves} {warps} {frames} {video_path} {dev_args}",
		             shell = True,
		             capture_output = True,
		             text = True)

		print(res.stderr, end = "")
		if res.returncode == 0:
			output.write(f"{version}, async, {proc.affinity}, {alpha}, {levels}, {solves}, {warps}, {frames}, {res.stdout}")


acc_refs = {
    "v100": "ctrl_async_1_0.2_5_500_3_CUDA-Tesla-V100-PCIE-32GB#CPU-1-1-2-OFF.csv",
    "wx9100": "ctrl_async_0_0.2_5_500_3_OpenCL-GPU-gfx900:xnack--AMD-Accelerated-Parallel-Processing#CPU-1-0-1-OFF.csv",
    # "gfx900": "ctrl_async_0_0.2_5_500_3_OpenCL-GPU-gfx900:xnack--AMD-Accelerated-Parallel-Processing#CPU-1-0-1-OFF.csv",
    # "wx9100": "ctrl_async_0_0.2_5_500_3_HIP-Radeon-Pro-WX-9100#CPU-1-0-1-OFF.csv",
    "a100": "ctrl_async_0_0.2_5_500_3_CUDA-NVIDIA-A100-80GB-PCIe#CPU-1-0-1-OFF.csv",
    "rtx4500": "ctrl_async_4_0.2_5_500_3_CUDA-NVIDIA-RTX-4500-Ada-Generation#CPU-1-4-5-OFF.csv",
    "leonardo": "ctrl_async_0_0.2_5_500_3_CUDA-NVIDIA-A100-SXM-64GB#CPU-1-0-1-OFF.csv",
    "leo-a100": "ctrl_async_0_0.2_5_500_3_CUDA-NVIDIA-A100-SXM-64GB#CPU-1-0-1-OFF.csv",
}


def hso_plot_label(fstat_name: str):
	name_list = fstat_name.split("_")
	l_version = name_list[0].title()
	l_devs = name_list[-1].strip(".csv").split("#")
	if l_version == "Ref":
		# use device arch instead of "ref"
		l_version = l_devs[0]
	else:
		# remove cpu dev for norm
		l_devs.pop()
	dev_tally = " ".join([f"{v}x{utils.format_devname(k)}" for k, v in Counter(l_devs).items()])
	return f"{l_version} {dev_tally}"


def plot(conf_dir: os.DirEntry):
	plt.rcParams.update({'font.size': 14})

	# title
	plt.title(conf_dir.name.removeprefix("agg_").replace("_", " ").title())
	# conf_name_l = [s.title() for s in conf_dir.name.removeprefix("agg_").split("_")]
	# if len(conf_name_l) > 1:
	# 	conf_name_l[-1] = conf_name_l[-1].upper()
	# plt.title(" ".join(conf_name_l))

	# labels
	plt.xlabel('N log scale (Frames)')
	plt.ylabel('Time log scale (s.)')

	utils.default_line_plot(conf_dir, hso_plot_label)

	plt.legend(fontsize = "small")

	plt.savefig(os.path.join(conf_dir.path, f"{conf_dir.name}_hsopticalflow.pdf"), format = "pdf")
	plt.close()

	if conf_dir.name.startswith("agg") and conf_dir.name.split("_")[-1] in acc_refs.keys():
		acc_plot(conf_dir)


def acc_plot(conf_dir: os.DirEntry):
	plt.rcParams.update({'font.size': 14})

	with open(os.path.join(conf_dir.path, "stats", acc_refs[conf_dir.name.split("_")[-1]]), "r") as f:
		f.readline()
		ref_data = list(zip(*[l.split(", ") for l in f]))[1]
		ref_data = list(map(float, ref_data))

	# title
	plt.title(conf_dir.name.removeprefix("agg_").replace("_", " ").title())
	# conf_name_l = [s.title() for s in conf_dir.name.removeprefix("agg_").split("_")]
	# if len(conf_name_l) > 1:
	# 	conf_name_l[-1] = conf_name_l[-1].removeprefix("Leo-").upper()
	# 	plt.title(f"{conf_name_l[0]} (vs Ctrl {conf_name_l[-1]})")
	# else:
	# 	plt.title(f"{conf_name_l[0]}")

	# labels
	plt.xlabel('N log scale (Frames)')
	plt.ylabel('Speedup')

	# logscale
	plt.xscale("log")

	# Set tick properties
	plt.tick_params(which = 'both', direction = 'in', top = True, right = True)
	plt.tick_params(axis = "x", which = 'minor', bottom = False, top = False, labelbottom = False)

	markers = iter(utils.markers_list)
	for result in sorted(os.scandir(os.path.join(conf_dir.path, "stats")), key = lambda x: x.name):
		if result.name.startswith("ref") or result.name == acc_refs[conf_dir.name.split("_")[-1]]:
			continue
		with open(result, "r") as f_in:
			# legend
			f_in.readline()

			# data: [[x_axis], [y_axis]]
			data = list(zip(*[l.split(", ") for l in f_in]))[:2]

			data[0] = list(map(int, data[0]))
			data[1] = [ref / new for ref, new in zip(ref_data, list(map(float, data[1])))]

			plt.xticks(data[0], list(map(str, data[0])))
			plt.plot(*data, color = "k", linestyle = "-", marker = next(markers), markerfacecolor = 'none', label = hso_plot_label(result.name))

	plt.legend()
	plt.savefig(os.path.join(conf_dir.path, "..", "..", "..", "plots", f"{conf_dir.name}_hsopticalflow_acc.pdf"), format = "pdf")
	plt.close()
