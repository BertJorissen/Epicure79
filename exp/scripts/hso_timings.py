v100 = [
    0.024723,
    0.020246,
    0.020466,
    0.022008,
    0.022747,
    0.022095,
    0.044646,
    0.044942,
    0.045042,
    0.165887,
    0.165926,
    0.166021,
    0.679594,
    0.679795,
    0.679921,
]

a100 = [
    0.009179,
    0.008144,
    0.008359,
    0.008125,
    0.008638,
    0.008262,
    0.023924,
    0.023945,
    0.023914,
    0.084849,
    0.084989,
    0.084917,
    0.329201,
    0.329487,
    0.330010,
]

rtx4500 = [
    0.009290,
    0.006376,
    0.008216,
    0.005949,
    0.007341,
    0.007021,
    0.052469,
    0.052419,
    0.052465,
    0.299983,
    0.298681,
    0.299577,
    1.186336,
    1.186779,
    1.186487,
]

gfx1100 = [
    0.014942,
    0.016027,
    0.015338,
    0.020728,
    0.020554,
    0.020852,
    0.072153,
    0.071949,
    0.071903,
    0.363155,
    0.363352,
    0.365319,
    1.318498,
    1.319515,
    1.312999,
]

gfx900 = [
    0.035169,
    0.034395,
    0.026057,
    0.053013,
    0.051517,
    0.053433,
    0.136761,
    0.137288,
    0.136144,
    0.508861,
    0.505815,
    0.505853,
    1.834659,
    1.803274,
    1.832087,
]

a100_leo = [
    0.005717,
    0.007916,
    0.005062,
    0.006714,
    0.008072,
    0.006706,
    0.026713,
    0.026764,
    0.026728,
    0.095708,
    0.095796,
    0.095826,
    0.370873,
    0.370955,
    0.370809,
]

mant_cpu = [
    0.084338,
    0.073207,
    0.073114,
    0.178623,
    0.181383,
    0.180994,
    0.566475,
    0.556775,
    0.553172,
    2.672451,
    2.661364,
    2.742754,
    9.960166,
    8.989161,
    8.617821,
]


def timing_metrics(devs: list[list[float]], weights: list[int]):
	print("Totals: ")
	for i, d in enumerate(devs):
		print(f"\t{i}: {sum(d)}")

	cuts = []
	partials = []
	curr = 0
	print("Partials: ")
	for i, w in enumerate(weights):
		cuts.append(curr)
		partials.append(sum(devs[i][curr:curr + w]))
		print(f"\t{i}: {curr}-{curr+w} {partials[-1]}")
		curr += w

	print(f"Theoric max speedup: {sum([min([sum(d2) for d2 in devs])/sum(d) for d in devs])}")
	print(f"Expected speedup: {min(sum(d) for d in devs)/max(partials)}")
	# only works for 2 devs
	# print(f"Expected speedup2: {(min(sum(d) for d in devs)*39)/(max(partials)*38+sum(partials))}")


print("2x v100")
timing_metrics([v100, v100], [13, 2])
print()

print("2x gfx900")
timing_metrics([gfx900, gfx900], [13, 2])
print()

print("1x gfx900 1x v100")
timing_metrics([gfx900, v100], [12, 3])
print()

print("2x rtx4500")
timing_metrics([rtx4500, rtx4500], [13, 2])
print()

# 107
print("rtx4500 gfx1100")
timing_metrics([rtx4500, gfx1100], [13, 2])
print()

# 43.77
print("rtx4500 a100")
timing_metrics([rtx4500, a100], [11, 4])
print()

# 44.48
print("gfx1100 a100")
timing_metrics([gfx1100, a100], [11, 4])
print()

# 44.2
print("rtx4500 rtx4500 a100")
timing_metrics([rtx4500, rtx4500, a100], [10, 2, 3])
print()

# 46.4
print("gfx1100 rtx4500 a100")
timing_metrics([gfx1100, rtx4500, a100], [10, 2, 3])
print()

print("2x leo a100")
timing_metrics([a100_leo, a100_leo], [13, 2])
print()

print("4x leo a100")
timing_metrics([a100_leo, a100_leo, a100_leo, a100_leo], [12, 1, 1, 1])
print()

# print("1x gfx900 1x v100")
# foo([gfx900, v100], [11, 4])
# print()

# print("1x v100 1x gfx900")
# foo([v100, gfx900], [14, 1])
# print()

# print("1x cpu 1x v100 ")
# foo([mant_cpu, v100], [9, 6])
# print()

# # 112.5
# print("gfx1100 rtx4500")
# foo([gfx1100, rtx4500], [13, 2])
# print()

# # 49.4
# print("a100 rtx4500")
# foo([a100, rtx4500], [14, 1])
# print()

# # 53.8
# print("a100 gfx1100")
# foo([a100, gfx1100], [14, 1])
# print()

# # 46.9
# print("rtx4500 a100")
# foo([rtx4500, a100], [10, 5])
# print()

# # 44.59
# print("rtx4500 a100")
# foo([rtx4500, a100], [12, 3])
# print()

# # 47
# print("gfx1100 a100")
# foo([gfx1100, a100], [10, 5])
# print()

# # 58.5
# print("gfx1100 a100")
# foo([gfx1100, a100], [12, 3])
# print()

# # 47,34
# print("rtx4500 gfx1100 a100")
# foo([rtx4500, gfx1100, a100], [10, 2, 3])
# print()

# print("rtx4500 a100")
# foo([rtx4500, a100], [12, 3])
# print()

# def wi_ratios(d1: list[float], d2: list[float]):
# 	for t1, t2 in zip(d1, d2):
# 		print(f"{t1 / t2:.4f}")

# print("cpu,v100")
# wi_ratios(mant_cpu, v100)
# print()
# print("cpu,gfx900")
# wi_ratios(mant_cpu, gfx900)
# print()
# print("gfx900,v100")
# wi_ratios(gfx900, v100)

# print(f"{sum(a100) = }")
# print(f"{sum(a100[-3:]) = }")
# print(f"{sum(a100[-4:]) = }")
# print(f"{sum(a100[-5:]) = }")
# print(f"{sum(rtx4500) = }")
# print(f"{sum(rtx4500[-5:-3]) = }")
# print(f"{sum(rtx4500[:-5]) = }")

# print(f"{sum(gfx1100) = }")

# print(f"{sum(gfx1100[:-3]) = }")
# print(f"{sum(gfx1100[-5:-3]) = }")
# print(f"{sum(gfx1100[:-4]) = }")
# print(f"{sum(gfx1100[:-5]) = }")
