#!/bin/bash
cd "$( dirname "${BASH_SOURCE[0]}" )"

machines=($(ls ../../results))
for m in "${machines[@]}"; do
	benchmarks=($(ls ../../results/$m/stats))
	for b in "${benchmarks[@]}"; do
		archs=($(ls ../../results/$m/stats/$b))
		mkdir -p ../../results/$m/plots/$b
		for a in "${archs[@]}"; do
			modes=($(ls ../../results/$m/stats/$b/$a))
			for mode in "${modes[@]}"; do
				( gnuplot -e "machine='${m}'; arch='${a}'" "${b}_${mode}.gp" )
			done
		done
	done
done