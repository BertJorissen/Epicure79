from dataclasses import dataclass, field
import os
import statistics as st
from collections import OrderedDict
import errno
from typing import Callable
import argparse


@dataclass
class Stats:
	mean: float = 0
	median: float = 0
	st_dev: float = 0
	ic_95_low: float = 0
	ic_95_high: float = 0
	min: float = 0
	max: float = 0
	overhead: float = 0
	overlap: float = 0


@dataclass
class Data:
	results: tuple[str, ...]
	main_times: list[float]
	exec_times: list[float]
	stats: Stats = field(default_factory = Stats)


def confidence_interval(data: list[float], confidence = 0.95) -> tuple[float, float]:
	dist = st.NormalDist.from_samples(data)
	z = st.NormalDist().inv_cdf((1 + confidence) / 2.)
	h = dist.stdev * z / ((len(data) - 1)**.5)
	return dist.mean - h, dist.mean + h


def comparative_metric(name: str, key: tuple[str, ...], grouped_dict: dict[tuple, dict[str, Data]], comp_param: str, comp_values: tuple[str, ...],
                       func: Callable[[float, float], float]):
	""" 
	calculate comparative metric for an entry
		name: name of the metric
		key: key of the entry we want to calculate the metric for
		grouped_dict: dict containing all data
		comp_param: parameter that changes between entries
		comp_values: values that comp_param can be
		func: function to calculate metric
	"""
	param_index = legend_no_size.index(comp_param)
	if key[param_index] == comp_values[0]:
		counterpart = key[:param_index] + (comp_values[1],) + key[param_index + 1:]
		if comp_param == "version":
			if key[legend_no_size.index("arch")] == "cpu":
				mem_transfer_index = legend_no_size.index("mem_transfers")
				device_index = legend_no_size.index("device")
				policy_index = legend_no_size.index("policy")
				counterpart = counterpart[:policy_index] + ("sync",) + counterpart[policy_index + 1:]
				counterpart = counterpart[:mem_transfer_index] + ("null",) + counterpart[mem_transfer_index + 1:]
				counterpart = counterpart[:device_index] + ("null",) + counterpart[device_index + 1:]
		if counterpart in grouped_dict:
			for inner_key in grouped_dict[key].keys():
				grouped_dict[key][inner_key].stats.__dict__[name] = func(grouped_dict[key][inner_key].stats.mean,
				                                                         grouped_dict[counterpart][inner_key].stats.mean)
	else:
		for inner_value in grouped_dict[key].values():
			# first version has the metric, the other has 0
			inner_value.stats.__dict__[name] = 0


def create_path(filename: str):
	""" create directories if they don't already exist """
	if not os.path.exists(os.path.dirname(filename)):
		try:
			os.makedirs(os.path.dirname(filename))
		except OSError as exc:  # Guard against race condition
			if exc.errno != errno.EEXIST:
				raise


def read_files(mode: os.DirEntry) -> tuple[dict[tuple, Data], list[str]]:
	""" Read files and load them into data_dict """
	legend: list[str] = []
	data_dict: dict[tuple, Data] = dict()
	for version in os.scandir(mode.path):
		with open(version.path, "r") as f_in:
			legend = f_in.readline().strip().split(", ")
			key_length = len([field for field in legend if "clock" not in field and "res" not in field and "sum" not in field])
			res_length = len([field for field in legend if "res" in field or "sum" in field])
			# add this file's data to the dict
			for l in [l.split(", ") for l in f_in.readlines()]:
				key = tuple(l[0:key_length])
				if key in data_dict:
					if data_dict[key].results != tuple(l[key_length:key_length + res_length]):
						print(f"Warning results differ on {key}")
					data_dict[key].main_times.append(float(l[-2]))
					data_dict[key].exec_times.append(float(l[-1]))
				else:
					data_dict[key] = Data(results = tuple(l[key_length:key_length + res_length]),
					                      main_times = [float(l[-2])],
					                      exec_times = [float(l[-1])])
	return data_dict, legend


def calc_stats(data_dict: dict[tuple, Data]):
	for value in data_dict.values():
		q1, _, q3 = st.quantiles(value.exec_times, n = 4)
		iqr = q3 - q1
		value.exec_times = [item for item in value.exec_times if item > q1 - (1.5 * iqr) and item < q3 + (1.5 * iqr)]
		ic_95 = confidence_interval(value.exec_times)
		value.stats = Stats(mean = st.mean(value.exec_times),
		                    median = st.median(value.exec_times),
		                    st_dev = st.stdev(value.exec_times),
		                    ic_95_low = ic_95[0],
		                    ic_95_high = ic_95[1],
		                    min = min(value.exec_times),
		                    max = max(value.exec_times))


def group_entries(data_dict: dict[tuple, Data], x_index: int) -> dict[tuple, dict[str, Data]]:
	""" Group entries belonging in the same plot line together """
	grouped_dict: dict[tuple, dict[str, Data]] = dict()
	for k, v in data_dict.items():
		value = k[x_index]
		new_key = k[:x_index] + k[x_index + 1:]
		if new_key in grouped_dict:
			grouped_dict[new_key][value] = v
		else:
			grouped_dict[new_key] = {value: v}
	return grouped_dict


def output_results(grouped_dict: dict[tuple, dict[str, Data]]):
	""" Output the results to csv files """
	for key, value in grouped_dict.items():
		out_file = os.path.join(machine.path, "stats", bench.name, arch.name, mode.name, "_".join([s.replace(" ", "-") for s in key]) + ".csv")
		create_path(out_file)
		with open(out_file, "w") as f_out:
			stats: list[str] = list(Stats().__dict__.keys())
			f_out.write(x_parameter)
			for stat in stats:
				f_out.write(", " + stat)
			f_out.write("\n")
			for inner_key, inner_value in value.items():
				f_out.write(inner_key)
				for stat in stats:
					f_out.write(f", {inner_value.stats.__dict__[stat]:.4f}")
				f_out.write("\n")


parser = argparse.ArgumentParser(description = "Utility to check restults and calculate statistics of Controller experimentation")
parser.add_argument("res_path", type = str, help = "Path to results directory")
parser.add_argument("-t", "--test", action = "store_true", help = "Only check results, don't calculate stats")
args = parser.parse_args()

# NOTE: care for non result fields that contain string res or sum and same for clock
for machine in os.scandir(args.res_path):
	for bench in os.scandir(os.path.join(machine.path, "raw")):
		for arch in os.scandir(bench.path):
			for mode in os.scandir(arch.path):
				data_dict, legend = read_files(mode)

				# parameter that will be on the x axis on the plot
				if "sobel" in bench.name or "hso" in bench.name:
					x_parameter = "baseline" if "size" in mode.name else "frames"
				else:
					x_parameter = mode.name

				x_index = legend.index(x_parameter)
				legend_no_size = legend[:x_index] + legend[x_index + 1:]

				# eliminate outliers and calculate stats, these are added to the existing dict
				if not args.test:
					calc_stats(data_dict)

				# group data_dict entries whose keys only differ on the x axis parameter
				grouped_dict = group_entries(data_dict, x_index)

				# check results among diferent versions (ctrl, ref) and policies (sync, async)
				ref_res_pair = list(grouped_dict.items())[0]
				for key, value in grouped_dict.items():
					for x_axis_value in value.keys():
						if ref_res_pair[1][x_axis_value].results != value[x_axis_value].results:
							print(f"Warning results differ on {ref_res_pair[0]} and {key} on size {x_axis_value}")

				# calculate comparative metrics, these involve more than 2 versions eg: overhead, overlap...
				if not args.test:
					for key, value in grouped_dict.items():
						comparative_metric("overhead", key, grouped_dict, "version", ("ctrl", "ref"), lambda x, y: ((x / y) - 1.0) * 100.0)
						if "cpu" not in key or "ctrl" in key:
							comparative_metric("overlap", key, grouped_dict, "policy", ("async", "sync"), lambda x, y: (1.0 - (x / y)) * 100.0)

				# create output files and write calculated stats to them
				if not args.test:
					output_results(grouped_dict)
