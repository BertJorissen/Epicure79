from dataclasses import dataclass, field
from typing import Protocol


class DeviceConfig(Protocol):

	def get_ref_params(self) -> str:
		...

	@property
	def arch(self) -> str:
		...


@dataclass
class CudaDevice:
	dev: int
	streams: int = 1

	def get_ref_params(self) -> str:
		return f"{self.dev}"

	@property
	def arch(self) -> str:
		return "cuda"


@dataclass
class HipDevice:
	dev: int
	streams: int = 1

	def get_ref_params(self) -> str:
		return f"{self.dev}"

	@property
	def arch(self) -> str:
		return "hip"


@dataclass
class OpenCLDevice:
	platform: int
	dev: int
	streams: int = 1

	def get_ref_params(self) -> str:
		return f"{self.dev} {self.platform}"

	@property
	def arch(self) -> str:
		return "opencl"


@dataclass
class CPUDevice:
	threads: int
	numa: tuple[int, ...]
	transfers: int

	def get_ref_params(self) -> str:
		return f"{self.threads}"

	@property
	def arch(self) -> str:
		return "cpu"


@dataclass
class FPGADevice:
	platform: int
	dev: int
	exec_mode: int
	streams: int = 1

	def get_ref_params(self) -> str:
		return f"{self.dev} {self.platform} {self.exec_mode}"

	@property
	def arch(self) -> str:
		return "fpga"


@dataclass
class ProcConfig:
	node: str
	rank: int
	affinity: int = 0
	devices: list[DeviceConfig] = field(default_factory = list)


class Config:
	procs: list[ProcConfig]
	path: str

	def __init__(self, path: str):
		self.path = path
		self.procs = []
		with open(path, "r") as f:
			data = [l.strip() for l in f.read().split("\n") if l.strip() != ""]
			curr_node_name = ""
			for l in data:
				match l.split(" "):
					case ["node", name]:
						curr_node_name = name
					case ["proc", rank, *aff]:
						self.procs.append(ProcConfig(curr_node_name, int(rank), affinity = int(aff[0]) if aff else 0))
					case ["cuda", dev, *streams]:
						self.procs[-1].devices.append(CudaDevice(int(dev), streams = int(streams[0]) if streams else 0))
					case ["hip", dev, *streams]:
						self.procs[-1].devices.append(HipDevice(int(dev), streams = int(streams[0]) if streams else 0))
					case ["opencl", plat, dev, *streams]:
						self.procs[-1].devices.append(OpenCLDevice(int(plat), int(dev), streams = int(streams[0]) if streams else 0))
					case ["fpga", plat, dev, exec_mode, *streams]:
						self.procs[-1].devices.append(FPGADevice(int(plat), int(dev), int(exec_mode), streams = int(streams[0]) if streams else 0))
					case ["cpu", threads, numa, transfers]:
						self.procs[-1].devices.append(CPUDevice(int(threads), tuple(map(int, numa.split("-"))), int(transfers)))
					case _:
						raise ValueError(f"Error: bad config format on {path}")

			if len(self.procs) != len({p.rank for p in self.procs}):
				raise ValueError(f"Error: repeated process rank in config {path}")

	def get_march_flags(self, cc: str) -> str:
		"""Return the apropiate optimization flags based on the node and compiler used."""
		# TODO flags for medusa?
		# TODO clang compiler flags?
		match list({proc.node for proc in self.procs}):
			case ["manticore"]:
				if cc in ("gcc", "icc"):
					return "march=skylake-avx512"
			case ["gorgon"]:
				if cc in ("gcc", "icc"):
					return "march=znver3"
				elif cc == "icc":
					return "march=core-avx2"
		return ""
