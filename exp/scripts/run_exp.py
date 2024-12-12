import argparse
import subprocess as sp
import config
from benchmark import bench_dict
import os
from typing import TextIO
from datetime import datetime
import utils

if __name__ == "__main__":
	parser = argparse.ArgumentParser(
	    description = """Utility to run a experimentation consisting of one or more benchmarks using one or more configurations.
		Configurations are specified via Controller device selection files.""")

	parser.add_argument("-c", "--configs", nargs = "+", type = str, required = True, help = "Config files to use for the benchmarks")
	parser.add_argument("-b", "--benchmarks", nargs = "+", type = str, required = True, help = "Benchmarks to run", choices = bench_dict.keys())
	parser.add_argument("-v", "--versions", nargs = "+", type = str, default = ["ctrl", "ref"], help = "Versions to run. (default: %(default)s)")
	parser.add_argument("--conf_prefix", type = str, default = ".", help = "Path prefix for all Config files")
	parser.add_argument("-o", "--output", type = str, default = "./results", help = "Output directory path. (default: %(default)s)")
	parser.add_argument("-n", "--reps", type = int, default = 30, help = "Number of reps to run")
	parser.add_argument("--cc", type = str, help = "Compiler to use, may be overwritten depending on config, eg HIP needs clang.")
	parser.add_argument("-f", "--flags", nargs = "+", type = str, default = [], help = "Extra flags for compilation. Without the starting '-'")
	args = parser.parse_args()

	args.configs = [os.path.join(args.conf_prefix, cf) for cf in args.configs]

	for conf in args.configs:
		# parse config file
		conf_data = config.Config(conf)

		conf_name = os.path.basename(conf)

		# create folder structure and open output files
		out_files: dict[tuple[str, str], TextIO] = dict()
		for b in args.benchmarks:
			path = os.path.join(args.output, "raw/", b, conf_name)
			for v in args.versions:
				utils.create_path(os.path.join(path, f"{v}.log"))
				out_files[(b, v)] = open(os.path.join(path, f"{v}.log"), "w")
				out_files[(b, v)].write(f"{bench_dict[b].legend}\n")

		# compile stuff
		flags = args.flags + [conf_data.get_march_flags(args.cc)]
		flags = ",".join([f"-{flag}" for flag in flags if flag != ""])
		flags = f"--flags {flags}" if flags != "" else ""
		archs = ",".join({dev.arch for proc in conf_data.procs for dev in proc.devices})
		cc = f"--cc {args.cc}" if args.cc else ""
		sp.run(f"bash {utils.get_ctrl_dir()}/compile.sh -a {archs} -e {cc} {flags}", shell = True)
		for n in range(args.reps):
			for bench in args.benchmarks:
				for version in args.versions:
					print(f"{datetime.now()} Progress update: executing {conf_name = }, {bench = }, {version = }, {n + 1}/{args.reps}", flush = True)
					bench_dict[bench].run(conf_data, out_files[(bench, version)], version)

		for f in out_files.values():
			f.close()
	print(f"{datetime.now()} Experimentation finished")
