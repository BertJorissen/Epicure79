import config
import subprocess as sp
from typing import TextIO, cast
import os
import matplotlib.pyplot as plt
import utils


def get_legend():
	return "arch, version, policy, host_aff, baseline, width, height, frames, device_args, main_clock, exec_clock"


def run(conf: config.Config, output: TextIO, version: str, mode: str):
	# config sanity checks
	assert (len(conf.procs) == 1)
	proc = conf.procs[0]
	assert (len(proc.devices) == 1)
	dev = proc.devices[0]

	# common Sobel YUV values
	path = os.path.join(utils.get_ctrl_dir(), "build", "examples", "Sobel_YUV")
	srun = f"srun --exclusive -Q -p dark-night -K -t 5 -w {proc.node}"
	f_in = "/home/datasets/sobel/BigBuckBunny_1920_1080_24fps_1500fs.yuv"
	f_out = "/tmp/out.yuv"
	f_ref = "/home/datasets/sobel/reference_sobel_out.yuv"
	width = 1920
	height = 1080
	def_frames = 100
	frames_list = [20, 30, 40, 50, 60, 70, 100]
	baselines = ["FTF", "MTF", "MTM", "FTM"]

	affinity = str(proc.affinity)
	for policy in ["Sync", "Async"]:
		for base in baselines:
			if version == "ctrl":
				hwloc_bind = ""
				app_path = f"{path}/Sobel_YUV_{utils.arch_formatter[dev.arch]}_Ctrl_{base}"
				# TODO change bench to use env var?
				policy_int = (0 if policy == "Sync" else 1)
				dev_args = conf.path
			elif version == "ref":
				if dev.arch == "cpu":
					if policy == "Async":
						continue
					# use device affinity for cpu ref examples
					dev_cpu = cast(config.CPUDevice, dev)
					# FIXME handle case where the whole machine is given (empty range)
					# NOTE -1 because currently in ctrl range is exclusive on the end whereas hwloc-bind is inclusive
					affinity = f"{dev_cpu.numa[0]}-{dev_cpu.numa[1]-1}"
				hwloc_bind = f"hwloc-bind --cpubind node:{affinity} --membind node:{affinity}"
				app_path = f"{path}/Sobel_YUV_{utils.arch_formatter[dev.arch]}_Ref_{policy}_{base}"
				policy_int = ""
				dev_args = dev.get_ref_params()
			else:
				raise ValueError(f"Unknown version for Sobel YUV benchmark {version}")

			# run command and write to output file id successful
			if mode == "frames":
				for frames in frames_list:
					res = sp.run(f"{srun} {hwloc_bind} {app_path} {width} {height} {frames} {f_in} {f_out} {policy_int} {dev_args}",
					             shell = True,
					             capture_output = True,
					             text = True)

					print(res.stderr, end = "")
					if res.returncode == 0:
						output.write(f"{dev.arch}, {version}, {policy.lower()}, {affinity}, {base}, {width}, {height}, {frames}, {res.stdout}")
						res = sp.run(f"{srun} diff {f_ref} {f_out}", shell = True, capture_output = True, text = True)
						if res.stdout != "":
							print(
							    f"ERROR: missmatch in sobel output on: {srun} {hwloc_bind} {app_path} {width} {height} {frames} {f_in} {f_out} {policy_int} {dev_args}"
							)

			if mode == "baseline":
				res = sp.run(f"{srun} {hwloc_bind} {app_path} {width} {height} {def_frames} {f_in} {f_out} {policy_int} {dev_args}",
				             shell = True,
				             capture_output = True,
				             text = True)

				print(res.stderr, end = "")
				if res.returncode == 0:
					output.write(f"{dev.arch}, {version}, {policy.lower()}, {affinity}, {base}, {width}, {height}, {def_frames}, {res.stdout}")


def plot_f(conf_dir: os.DirEntry):
	conf_name = conf_dir.name.replace("_", " ").upper()
	# title
	plt.title(f"Sobel YUV ({conf_name})")

	# labels
	plt.xlabel('N log scale (Frames)')
	plt.ylabel('Time log scale (s.)')

	utils.default_line_plot(conf_dir)

	plt.savefig(os.path.join(conf_dir.path, "..", "..", "..", "plots", f"{conf_dir.name}_sobelyuv_frames.pdf"), format = "pdf")
	plt.close()


def plot_b(conf_dir: os.DirEntry):
	conf_name = conf_dir.name.replace("_", " ").upper()
	# title
	plt.title(f"Sobel YUV ({conf_name})")
	# Set labels
	plt.xlabel("Baseline")
	plt.ylabel("Time (s.)")

	# Set tick properties
	plt.tick_params(which = 'both', direction = 'in', top = True, right = True)
	plt.tick_params(axis = 'x', which = 'both', bottom = False, top = False)

	n_bars = len(os.listdir(os.path.join(conf_dir.path)))
	bar_width = 1 / (n_bars + 1)
	multiplier = 0 - n_bars / 2 + 0.5

	for result in sorted(os.scandir(conf_dir.path), key = lambda x: x.name):
		with open(result, "r") as f_in:
			# legend
			f_in.readline()
			# data: [[x_axis], [y_axis]]
			data = list(zip(*[l.split(", ") for l in f_in]))[:2]
			data[1] = list(map(float, data[1]))

			x = list(range(len(data[0])))
			# print(f"{result.name} {data = }")
			offset = bar_width * multiplier
			label = result.name.split("_")[1:3]
			plt.bar([v + offset for v in x], data[1], bar_width, label = f"{label[0].title()} {label[1].title()}")
			plt.xticks(x, data[0])

			# Set x axis range
			plt.xlim(-0.5, len(data[0]) - 0.5)
			multiplier += 1

	# Set grid and ticks
	plt.grid(axis = 'y', which = 'both')
	plt.legend()
	plt.savefig(os.path.join(conf_dir.path, "..", "..", "..", "plots", f"{conf_dir.name}_sobelyuv_baseline.pdf"), format = "pdf")
	plt.close()
