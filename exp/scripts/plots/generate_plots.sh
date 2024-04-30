#!/bin/bash

res_path=${1?"Usage: $0 res_path"}
script_dir="$(dirname "${BASH_SOURCE[0]}")"

machines=($(ls $res_path))
for m in "${machines[@]}"; do
	benchmarks=($(ls $res_path/$m/stats))
	for b in "${benchmarks[@]}"; do
		archs=($(ls $res_path/$m/stats/$b))
		mkdir -p $res_path/$m/plots/$b
		for a in "${archs[@]}"; do
			modes=($(ls $res_path/$m/stats/$b/$a))
			for mode in "${modes[@]}"; do
				(gnuplot -e "machine='${m}'; arch='${a}'; result_dir='${res_path}'" "$script_dir/${b}_${mode}.gp")
			done
		done
	done
done
