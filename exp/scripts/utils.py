import errno
import os
from typing import Callable
import matplotlib.pyplot as plt


def create_path(filename: str):
	""" create directories if they don't already exist """
	if not os.path.exists(os.path.dirname(filename)):
		try:
			os.makedirs(os.path.dirname(filename))
		except OSError as exc:  # Guard against race condition
			if exc.errno != errno.EEXIST:
				raise


# Arch name translator, useful for composing executable names of ctrl examples
arch_formatter = {
    "cpu": "Cpu",
    "cuda": "Cuda",
    "opencl": "OpenCL_Gpu",
    "fpga": "FPGA",
    "hip": "Hip",
}

markers_list = [
    "o",
    "s",
    "+",
    "x",
    "^",
    "v",
    "<",
    ">",
    "1",
    "2",
    "3",
    "4",
    "8",
    "p",
    "*",
    "h",
    "D",
]


def format_devname(devname: str) -> str:
	match devname.split("-"):
		case ["CPU", th]:
			return f"{th}th"
		case ["CPU", th, numa_s, numa_e, mmoves]:
			return f"{th}th {numa_s}-{numa_e} {mmoves}"
		case ["CUDA", "Tesla", "V100", "PCIE", "32GB"]:
			return "V100"
		case ["CUDA", "NVIDIA", "A100", "80GB", "PCIe"]:
			return "A100"
		case ["CUDA", "NVIDIA", "RTX", "4500", "Ada", "Generation"]:
			return "RTX4500"
		case ["CUDA", "NVIDIA", "A100", "SXM", "64GB"]:
			return "A100"
		case ["OpenCL", "GPU", "gfx900:xnack", "", "AMD", "Accelerated", "Parallel", "Processing"]:
			return "WX9100"
		case ["OpenCL", "GPU", "gfx1100", "AMD", "Accelerated", "Parallel", "Processing"]:
			return "W7800"
		case ["HIP", "AMD", "Radeon", "PRO", "W7800"]:
			return "W7800"
		case ["HIP", "Radeon", "Pro", "WX", "9100"]:
			return "WX9100"
		case _:
			raise ValueError(f"Unknown device name {devname}")


def default_plot_label(fstat_name: str) -> str:
	l_name = fstat_name.split("_")
	# TODO get indexes of this based on bench legend
	version = l_name[1].title()
	policy = l_name[2].title()
	l_devs = l_name[-1].strip(".csv").split("#")
	assert len(l_devs) == 1
	return f"{version} {policy} {format_devname(l_devs[0])}"


def default_line_plot(directory: os.DirEntry, get_label: Callable[[str], str] = default_plot_label):
	"""
	Plots a logarithmic line plot from all the files in directory.
	Title and axis titles are not set. 
	Caller is responsible from closing the plot.
	Files in directry are assumed to be csv with legend with integers in first column and floats in the second.
	File labels are determined by get_label function that takes the name of the csv file of that plot line.
	"""
	# logscale
	plt.yscale("log")
	plt.xscale("log")

	# Set tick properties
	plt.tick_params(which = 'both', direction = 'in', top = True, right = True)
	plt.tick_params(axis = "x", which = 'minor', bottom = False, top = False, labelbottom = False)

	markers = iter(markers_list)
	for result in sorted(os.scandir(directory.path), key = lambda x: x.name):
		with open(result, "r") as f_in:
			# legend
			f_in.readline()

			# data: [[x_axis], [y_axis]]
			data = list(zip(*[l.split(", ") for l in f_in]))[:2]

			data[0] = list(map(int, data[0]))
			data[1] = list(map(float, data[1]))

			plt.xticks(data[0], list(map(str, data[0])))
			plt.plot(*data, color = "k", linestyle = "-", marker = next(markers), markerfacecolor = 'none', label = get_label(result.name))

	plt.legend()


def get_ctrl_dir():
	return os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
