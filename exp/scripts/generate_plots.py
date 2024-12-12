import argparse
import os
from benchmark import bench_dict
import utils

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description = "Utility to generate plots from a Controller experimentation")
	parser.add_argument("res_path", type = str, help = "Path to results directory")
	args = parser.parse_args()

	utils.create_path(os.path.join(args.res_path, "plots/"))
	for bench in os.scandir(os.path.join(args.res_path, "stats")):
		for config in os.scandir(bench.path):
			bench_dict[bench.name].plot(config)
