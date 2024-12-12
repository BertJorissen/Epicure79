import config
import subprocess as sp
from typing import TextIO, cast
import os
import matplotlib.pyplot as plt
import utils


def get_legend():
	return "arch, version, policy, host_aff, size, height, iter, iters_per_copy, device_args, sum, result, main_clock, exec_clock"


def run(conf: config.Config, output: TextIO, version: str, mode: str):
	# config sanity checks
	assert (len(conf.procs) == 1)
	proc = conf.procs[0]
	assert (len(proc.devices) == 1)
	dev = proc.devices[0]

	# common hotspot values
	path = os.path.join(utils.get_ctrl_dir(), "build", "examples", "Hotspot")
	srun = f"srun --exclusive -Q -p dark-night -K -t 5 -w {proc.node}"
	def_size = 10000
	def_iters = 500
	iters_list = [100, 200, 300, 400, 500, 600]
	if dev.arch == "cpu":
		sizes_list = [1000, 2500, 5000, 7500, 10000]
		height = ""
		iter_per_cpy = "4"
	else:
		sizes_list = [1000, 2500, 5000, 10000, 15000]
		height = "4"
		iter_per_cpy = "1"

	affinity = str(proc.affinity)
	for policy in ["Sync", "Async"]:
		if version == "ctrl":
			hwloc_bind = ""
			app_path = f"{path}/Hotspot_{utils.arch_formatter[dev.arch]}_Ctrl"
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
			app_path = f"{path}/Hotspot_{utils.arch_formatter[dev.arch]}_Ref_{policy}"
			policy_int = ""
			dev_args = dev.get_ref_params()
		else:
			raise ValueError(f"Unknown version for hotspot benchmark {version}")

		# run command and write to output file id successful
		if mode == "size":
			for size in sizes_list:
				res = sp.run(f"{srun} {hwloc_bind} {app_path} {size} {height} {def_iters} {iter_per_cpy} {policy_int} {dev_args}",
				             shell = True,
				             capture_output = True,
				             text = True)

				print(res.stderr, end = "")
				if res.returncode == 0:
					output.write(f"{dev.arch}, {version}, {policy.lower()}, {affinity}, {size}, {height}, {def_iters}, {iter_per_cpy}, {res.stdout}")

		if mode == "iter":
			for iters in iters_list:
				res = sp.run(f"{srun} {hwloc_bind} {app_path} {def_size} {height} {iters} {iter_per_cpy} {policy_int} {dev_args}",
				             shell = True,
				             capture_output = True,
				             text = True)

				print(res.stderr, end = "")
				if res.returncode == 0:
					output.write(f"{dev.arch}, {version}, {policy.lower()}, {affinity}, {def_size}, {height}, {iters}, {iter_per_cpy}, {res.stdout}")


def plot(conf_dir: os.DirEntry, mode: str):
	conf_name = conf_dir.name.replace("_", " ").upper()
	# title
	plt.title(f"Hotspot ({conf_name})")

	# labels
	plt.xlabel(f"N log scale ({mode.title()})")
	plt.ylabel("Time log scale (s.)")

	utils.default_line_plot(conf_dir)

	plt.savefig(os.path.join(conf_dir.path, "..", "..", "..", "plots", f"{conf_dir.name}_hotspot_{mode}.pdf"), format = "pdf")
	plt.close()
