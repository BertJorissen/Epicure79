import subprocess as sp

extensions = [
    "c",
    "cc",
    "cl",
    "cpp",
    "cu",
    "h",
    "hpp",
    "cuh",
]

excludes = [
    "extern",
    "build",
    "kernels",
    "examples/Metrics",
    "examples/CtrlBlas/Sobel_YUV",
]

ext_cond = "-o".join(f' -name "*.{e}" ' for e in extensions)
excludes_cond = " ".join(f'-not -path "./{p}/*"' for p in excludes)
pattern = "@file"

find_cmd = rf'find -type f \( {ext_cond} \) {excludes_cond}'
# print(find_cmd)
all_files = set(sp.run(find_cmd, shell = True, capture_output = True, text = True).stdout.splitlines())
containing = set(sp.run(f'grep -rle {pattern} $({find_cmd})', shell = True, capture_output = True, text = True).stdout.splitlines())

print(*(all_files - containing), sep = '\n')
print(len(all_files - containing))
