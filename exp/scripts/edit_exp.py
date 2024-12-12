import argparse
import os
import utils
import shutil


def join(args):
	for bench in os.scandir(os.path.join(args.res_path, "stats")):
		for conf in set(args.input) & set(os.listdir(bench.path)):
			for stat_res in os.scandir(os.path.join(bench.path, conf)):
				out_path = os.path.join(args.res_path, "stats", bench.name, args.output, stat_res.name)
				utils.create_path(out_path)
				os.symlink(os.path.relpath(stat_res, os.path.dirname(out_path)), out_path)


def clean(args):
	if os.path.exists(plot_dir := os.path.join(args.res_path, "plots")):
		shutil.rmtree(plot_dir)

	if os.path.exists(stat_dir := os.path.join(args.res_path, "stats")):
		shutil.rmtree(stat_dir)


# TODO add to existing conf
# TODO subtract from config
# TODO delete bench
# TODO delete conf

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description = "Utility to edit a Controller experimentation")
	parser.add_argument("res_path", type = str, help = "Path to results directory")

	# create sub-parser
	sub_parsers = parser.add_subparsers(help = "sub-command help", required = True)

	parser_join = sub_parsers.add_parser(
	    "join", help = "Join multiple configs to create an aggregate. Symlinks are used so aggregate configs update accordingly.")
	parser_join.add_argument("-i", "--input", nargs = "+", type = str, required = True, help = "Input configs")
	parser_join.add_argument("-o", "--output", type = str, required = True, help = "Output config name")
	parser_join.set_defaults(func = join)

	parser_clean = sub_parsers.add_parser("clean", help = "Clean everything from all configs except from the raw logs")
	parser_clean.set_defaults(func = clean)

	args = parser.parse_args()
	args.func(args)
