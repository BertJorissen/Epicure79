import os
import statistics as st
from collections import OrderedDict
import errno

def confidence_interval(data, confidence=0.95):
	dist = st.NormalDist.from_samples(data)
	z = st.NormalDist().inv_cdf((1 + confidence) / 2.)
	h = dist.stdev * z / ((len(data) - 1) ** .5)
	return dist.mean - h, dist.mean + h

""" 
calculate comparative metric for an entry
	name: (str) name of the metric
	key: (tuple) key of the entry we want to calculate the metric for
	grouped_dict: (dict) dict containing all data
	comp_param: (str) parameter that changes between entries
	comp_values: (tuple) values that comp_param can be
	func: (function) function to calculate metric
"""
def comparative_metric(name, key, grouped_dict, comp_param, comp_values, func):
	param_index = legend_no_size.index(comp_param)
	if key[param_index] == comp_values[0]:
		counterpart = key[:param_index] + (comp_values[1],) + key[param_index + 1:]
		if comp_param == "version":
			queues_index = legend_no_size.index("queues")
			counterpart = counterpart[:queues_index] + ("null",) + counterpart[queues_index + 1:]
			if key[legend_no_size.index("arch")] == "cpu":
				mem_transfer_index = legend_no_size.index("mem_transfers")
				device_index = legend_no_size.index("device")
				policy_index = legend_no_size.index("policy")
				counterpart = counterpart[:policy_index] + ("sync",) + counterpart[policy_index + 1:]
				counterpart = counterpart[:mem_transfer_index] + ("null",) + counterpart[mem_transfer_index + 1:]
				counterpart = counterpart[:device_index] + ("null",) + counterpart[device_index + 1:]
		if counterpart in grouped_dict:
			for inner_key in grouped_dict[key].keys():
				grouped_dict[key][inner_key]["stats"][name] = func(
					grouped_dict[key][inner_key]["stats"]["mean"],
					grouped_dict[counterpart][inner_key]["stats"]["mean"])
	else:
		for inner_value in grouped_dict[key].values():
			# first version has the metric, the other has 0
			inner_value["stats"][name] = 0

""" 
create directories if they don't already exist
"""
def create_path(filename):
	if not os.path.exists(os.path.dirname(filename)):
		try:
			os.makedirs(os.path.dirname(filename))
		except OSError as exc: # Guard against race condition
			if exc.errno != errno.EEXIST:
				raise

# NOTE: care for non res fields that contain str res or sum and same for clock
results_raw_path=os.path.join(os.path.dirname(__file__), "../results")
for machine in os.scandir(results_raw_path):
	for bench in os.scandir(os.path.join(machine.path, "raw")):
		for arch in os.scandir(bench.path):
			for mode in os.scandir(arch.path):
				# dict for all the data for one bench and arch key is arguments used for execution, value is another dict with results and a list of times
				data_dict=dict()
				for version in os.scandir(mode.path):
					with open(version.path, "r") as f_in:
						legend=f_in.readline().split(", ")
						key_lenght=len([field for field in legend if "clock" not in field and "res" not in field and "sum" not in field])
						res_lenght=len([field for field in legend if "res" in field or "sum" in field])
						time_lenght=len([field for field in legend if "clock" in field])
						# add this file's data to the dict 
						for l in [l.split(", ") for l in f_in.readlines()]:
							key = tuple(l[0:key_lenght])
							if key in data_dict:
								if data_dict[key]["res"] != tuple(l[key_lenght:key_lenght+res_lenght]):
									print(f"Warning results differ on {key}")
								data_dict[key]["main_clock"].append(float(l[-2]))
								data_dict[key]["exec_clock"].append(float(l[-1]))
							else:
								data_dict[key] = {
									"res": tuple(l[key_lenght:key_lenght+res_lenght]), 
									"main_clock": [float(l[-2])], 
									"exec_clock": [float(l[-1])]
								}

				# parameter that will be on the x axis on the plot
				if "sobel" in bench.name:
					x_axis_parameter = "baseline" if "size" in mode.name else "frames"
				else:
					x_axis_parameter = mode.name
					
				x_axis_index = legend.index(x_axis_parameter)
				legend_no_size=legend[:x_axis_index]+legend[x_axis_index+1:]

				# eliminate outliers and calculate stats, these are added to the existing dict
				for value in data_dict.values():
					q1, q2, q3=st.quantiles(value["exec_clock"], n=4)
					iqr=q3-q1
					value["exec_clock"]=[item for item in value["exec_clock"] if item > q1-(1.5*iqr) and item < q3+(1.5*iqr)]

					ic_95 = confidence_interval(value["exec_clock"])
					value["stats"]=OrderedDict([
						("mean" , st.mean(value["exec_clock"])),
						("median" , st.median(value["exec_clock"])),
						("st_dev" , st.stdev(value["exec_clock"])),
						("ic_95_low" , ic_95[0]),
						("ic_95_high" , ic_95[1]),
						("min" , min(value["exec_clock"])),
						("max" , max(value["exec_clock"])),
					])
				
				# group data_dict entries whose keys only differ on the x axis parameter
				grouped_dict = dict()
				for key, value in data_dict.items():
					x_axis_value = key[x_axis_index]
					new_key = key[:x_axis_index] + key[x_axis_index + 1:]
					if new_key in grouped_dict:
						grouped_dict[new_key][x_axis_value]=value
					else:
						grouped_dict[new_key] = {x_axis_value : value}
				
				# check results among diferent versions (ctrl, ref) and policies (sync, async)
				ref_res_pair = list(grouped_dict.items())[0]
				for key, value in grouped_dict.items():
					for x_axis_value in value.keys():
						if ref_res_pair[1][x_axis_value]["res"] != value[x_axis_value]["res"]:
							print(f"Warning results differ on {ref_res_pair[0]} and {key} on size {x_axis_value}")

				# calculate comparative metrics, these involve more than 2 versions eg: overhead, overlap...
				for key, value in grouped_dict.items():
					comparative_metric("overhead", key, grouped_dict, "version", ("ctrl", "ref"), lambda x, y: ((x / y) - 1.0) * 100.0)
					if "cpu" not in key or "ctrl" in key:
						comparative_metric("overlap", key, grouped_dict, "policy", ("async","sync"), lambda x, y: (1.0 - (x / y)) * 100.0)
					if "ctrl" in key:
						comparative_metric("queue_overhead", key, grouped_dict, "queues", ("on", "off"), lambda x ,y: ((x / y) - 1.0) * 100.0)
				
				# create output files and write calculated stats to them
				for key, value in grouped_dict.items():
					out_file=os.path.join(machine.path, "stats", bench.name, arch.name, mode.name, "_".join([s.replace(" ", "-") for s in key]) + ".csv")
					create_path(out_file)
					with open(out_file, "w") as f_out:
						stats = list(list(value.values())[0]["stats"].keys())
						f_out.write(x_axis_parameter)
						for stat in stats:
							f_out.write(", " + stat)
						f_out.write("\n")
						for inner_key, inner_value in value.items():
							f_out.write(inner_key)
							for stat in stats:
								f_out.write(f", {inner_value['stats'][stat]:.4f}")
							f_out.write("\n")
