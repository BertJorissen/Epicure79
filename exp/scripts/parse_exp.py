from dataclasses import dataclass, field
import os
import statistics as st
from typing import Callable, Self
import argparse
from benchmark import bench_dict
import utils


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

	def calc_overhead(self, other: Self):
		self.overhead = ((self.mean / other.mean) - 1.0) * 100.0

	def calc_overlap(self, other: Self):
		self.overlap = (1.0 - (self.mean / other.mean)) * 100.0


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


def comparative_metric(key: tuple[str, ...], data_dict: dict[tuple, Data], legend: list[str], comp_param: str, comp_values: tuple[str, ...],
                       func: Callable[[Stats, Stats], None]):
	""" 
	calculate comparative metric for an entry
		name: name of the metric
		key: key of the entry we want to calculate the metric for
		data_dict: dict containing all data
		comp_param: parameter that changes between entries
		comp_values: values that comp_param can be
		func: function to calculate metric
	"""
	param_index = legend.index(comp_param)
	# first version has the metric, the other has 0
	if key[param_index] == comp_values[0]:
		counterpart = (*key[:param_index], comp_values[1], *key[param_index + 1:])
		# handle cpu differences, ref has no async and device only has number of threads
		if comp_param == "version" and key[legend.index("arch")] == "cpu":
			device_index = legend.index("device_args")
			policy_index = legend.index("policy")
			counterpart = (*counterpart[:policy_index], "sync", *counterpart[policy_index + 1:])
			# only keep the arch and nthreads part of the device
			counterpart = (*counterpart[:device_index], "_".join(key[device_index].split("_")[:2]), *counterpart[device_index + 1:])
		if counterpart in data_dict:
			func(data_dict[key].stats, data_dict[counterpart].stats)


def read_files(config: os.DirEntry, legend: str) -> dict[tuple, Data]:
	""" Read files and load them into data_dict """
	data_dict: dict[tuple, Data] = dict()
	for version in os.scandir(config.path):
		with open(version.path, "r") as f_in:
			if (curr_legend := f_in.readline().strip()) != legend:
				print(f"Warning {version.path} legend differs from expected.\nExpected {legend}\n but was {curr_legend}")
			# FIXME this assumes that the legend goes key-res-mtime-extime and the names of some fields
			key_length = len([field for field in legend.split(", ") if "clock" not in field and "res" not in field and "sum" not in field])
			res_length = len([field for field in legend.split(", ") if "res" in field or "sum" in field])
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
	return data_dict


def calc_stats(data_dict: dict[tuple, Data]):
	for key, value in data_dict.items():
		q1, _, q3 = st.quantiles(value.exec_times, n = 4)
		iqr = q3 - q1
		times_cpy = value.exec_times[:]
		value.exec_times = [item for item in value.exec_times if item > q1 - (1.5 * iqr) and item < q3 + (1.5 * iqr)]
		ic_95 = confidence_interval(value.exec_times)
		value.stats = Stats(mean = st.mean(value.exec_times),
		                    median = st.median(value.exec_times),
		                    st_dev = st.stdev(value.exec_times),
		                    ic_95_low = ic_95[0],
		                    ic_95_high = ic_95[1],
		                    min = min(value.exec_times),
		                    max = max(value.exec_times))
		if len(outliers := set(times_cpy) - set(value.exec_times)) > 0:
			id_o = sorted([(o, times_cpy.index(o)) for o in outliers], key = lambda x: x[1])
			print(f"{key} has {len(outliers)} outliers {id_o}, mean={value.stats.mean:.4f}, st_dev={value.stats.st_dev:.4f}")


def group_entries(data_dict: dict[tuple, Data], legend: list[str], group_param: str) -> dict[tuple, dict[str, Data]]:
	""" 
	Group entries belonging in the same plot line together
	If group_param is empty string no grouping is done
	"""
	group_index = legend.index(group_param) if group_param != "" else -1
	grouped_dict: dict[tuple, dict[str, Data]] = dict()
	for k, v in data_dict.items():
		value = k[group_index] if group_index > -1 else ""
		new_key = k[:group_index] + k[group_index + 1:] if group_index > -1 else k
		if new_key in grouped_dict:
			grouped_dict[new_key][value] = v
		else:
			grouped_dict[new_key] = {value: v}
	return grouped_dict


def output_results(grouped_dict: dict[tuple, dict[str, Data]], group_param: str, res_path: str, bench: str, config: str):
	""" Output the results to csv files """
	for key, value in grouped_dict.items():
		out_file = os.path.join(res_path, "stats", bench, config, "_".join([s.replace(" ", "-") for s in key]) + ".csv")
		utils.create_path(out_file)
		with open(out_file, "w") as f_out:
			if group_param != "":
				f_out.write(f"{group_param}, ")
			f_out.write(", ".join(Stats().__dict__.keys()))
			f_out.write("\n")
			for inner_key, inner_value in value.items():
				if inner_key != "":
					f_out.write(f"{inner_key}, ")
				f_out.write(", ".join(f"{stat:.4f}" for stat in inner_value.stats.__dict__.values()))
				f_out.write("\n")


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description = "Utility to check restults and calculate statistics of Controller experimentation")
	parser.add_argument("res_path", type = str, help = "Path to results directory")
	parser.add_argument("-t", "--test", action = "store_true", help = "Only check results, don't calculate stats")
	args = parser.parse_args()

	# NOTE: care for non result fields that contain string res or sum and same for clock
	for bench in os.scandir(os.path.join(args.res_path, "raw")):
		legend = bench_dict[bench.name].legend.split(", ")
		for config in os.scandir(bench.path):
			data_dict = read_files(config, bench_dict[bench.name].legend)

			# eliminate outliers and calculate stats, these are added to the existing dict
			if not args.test:
				calc_stats(data_dict)

			# check results among diferent versions (ctrl, ref) and policies (sync, async)
			# FIXME this is currently broken because it compares different sized matrices
			# ref_res_pair = list(data_dict.items())[0]
			# for key, value in data_dict.items():
			# 	if ref_res_pair[1].results != value.results:
			# 		print(f"Warning results differ on {ref_res_pair[0]} and {key}")

			# calculate comparative metrics, these involve more than 2 versions eg: overhead, overlap...
			# FIXME if arch is not in legend this breaks, find device equivalence
			# if not args.test:
			# 	for key, value in data_dict.items():
			# 		comparative_metric(key, data_dict, legend, "version", ("ctrl", "ref"), Stats.calc_overhead)
			# 		if "cpu" not in key or "ctrl" in key:
			# 			comparative_metric(key, data_dict, legend, "policy", ("async", "sync"), Stats.calc_overlap)

			# group data_dict entries whose keys only differ on the x axis parameter
			grouped_dict = group_entries(data_dict, legend, bench_dict[bench.name].group_param)

			# create output files and write calculated stats to them
			if not args.test:
				output_results(grouped_dict, bench_dict[bench.name].group_param, args.res_path, bench.name, config.name)
