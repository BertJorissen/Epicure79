from dataclasses import dataclass
from functools import partial
from typing import Callable, TextIO
import bench_modules.hotspot as hotspot
import bench_modules.matrix_pow as matrix_pow
import bench_modules.sobel_yuv as sobel_yuv
import bench_modules.hsopticalflow as hsopticalflow
import bench_modules.hsopticalflow_1f as hsopticalflow_1f
import config
import os


@dataclass
class Benchmark:
	legend: str
	run: Callable[[config.Config, TextIO, str], None]
	plot: Callable[[os.DirEntry], None]
	group_param: str = ""


bench_dict = {
    "hotspot_s": Benchmark(hotspot.get_legend(), partial(hotspot.run, mode = "size"), partial(hotspot.plot, mode = "size"), "size"),
    "hotspot_i": Benchmark(hotspot.get_legend(), partial(hotspot.run, mode = "iter"), partial(hotspot.plot, mode = "iter"), "iter"),
    "matrix_pow_s": Benchmark(matrix_pow.get_legend(), partial(matrix_pow.run, mode = "size"), partial(matrix_pow.plot, mode = "size"), "size"),
    "matrix_pow_i": Benchmark(matrix_pow.get_legend(), partial(matrix_pow.run, mode = "iter"), partial(matrix_pow.plot, mode = "iter"), "iter"),
    "sobel_yuv_b": Benchmark(sobel_yuv.get_legend(), partial(sobel_yuv.run, mode = "baseline"), sobel_yuv.plot_b, "baseline"),
    "sobel_yuv_f": Benchmark(sobel_yuv.get_legend(), partial(sobel_yuv.run, mode = "frames"), sobel_yuv.plot_f, "frames"),
    "hsopticalflow": Benchmark(hsopticalflow.get_legend(), hsopticalflow.run, hsopticalflow.plot, "frames"),
    "hsopticalflow_1f": Benchmark(hsopticalflow_1f.get_legend(), hsopticalflow_1f.run, hsopticalflow_1f.plot, "version"),
}
