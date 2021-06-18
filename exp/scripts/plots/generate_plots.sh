#!/bin/bash
cd "$( dirname "${BASH_SOURCE[0]}" )"
mkdir -p ../../results/medusa/plots/hotspot
mkdir -p ../../results/medusa/plots/matrix_pow
mkdir -p ../../results/medusa/plots/sobel
mkdir -p ../../results/manticore/plots/hotspot
mkdir -p ../../results/manticore/plots/matrix_pow
mkdir -p ../../results/manticore/plots/sobel
for i in *.gp
do 
  ( gnuplot "$i" )
done