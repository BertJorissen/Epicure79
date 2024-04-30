Controller device-selection files
====
This document describes the format of the runtime config files of Controller programs that select the devices associated to the Controller objets on each node and process in distributed environments.


Summary: Launching MPI processes and the role of the device-selection file
----
The Controllers programs are launched with your usual MPI program launcher (mpirun, srun, etc.)
The nodes selected and the number of processes are controlled with the MPI launcher parameters. At the start of the controllers section of the program, each MPI process automatically retrieves the local node name, and gets an internal rank inside the local node. The process tries to locate in the device-selection file an entry that matches the node name and the local rank. If it is found, the entry describes each Controller object that should be created in that process, with which backend, and the specific device that should be attached to the object. If a matching entry is not found, a fatal error is raised and the program terminates.


Choose the device-selection file
----

The Controllers code block receives as parameter a string with the relative or absolute path to a readable device-selection config file. This configuration is used to automatically create and initialize the Controller objects described in the file for the specific node and the specific process identification (rank) inside the node.

Example: 
```
	__ctrl_block__( "my_config_file" ) {
		...
	}
```

NOTE: The file path string can be supplied as program argument or built as a string by the program.

TODO: Alternatively, use the content of an environment variable as the config_file path.


Accesing and using the Controller objects
----
Inside the Controllers code block a Controller object can be accesed using the function: Ctrl Ctrl_Get( int );

Example:
```
	Ctrl first_device = Ctrl_Get(0);
	Ctrl second_device = Ctrl_Get(1);
```


File format
----
Device-Selection files are organized in lines with one declaration on each line. Blank spaces and tabs at the start of the line are ignored. Keywords and parameters of each declaration are separated by exactly one space. Keywords are case-sensitive.

Declarations:
```
	Node section start: 
		node <machine_name> | *
	Process section start: 
		proc <process_node_rank> [ <numa_affinity> ]
	Device declaration line: 
		( cpu | cuda | hip | opencl | fpga ) <creation_parameters>
		of controller.
```

Initialization procedure
----
When the MPI process executes the Controller code block initializaton, it first queries the node name and reads the corresponding NODE section. In case that there is no section with that machine name, it fallbacks to a NODE section with the wildcard * as name. In case that there is not a valid section for the machine name, an error is issued and the process stops.

The processes in the same node are numbered starting at 0 to obtain a local rank in the node group. The process locates the PROC section with that local rank inside the selected node section. One Controller object is created by the process for each device declaration found in its process section.

Device declarations:
----
A device declaration line contains a keyword indicating the desired Controller backend, and a list of parameters for selecting the specific device. The parameters are specific for each type of backend.

* CPU threads
```
cpu <num_threads> <numa_aff_start>-<numa_aff_end> <memory_copy_flag (0|1)>
```

A Controller is created to manage a set of CPU cores with a number of logical threads equal to num_threads, with affinity set to the physical cores in the NUMA nodes included in the range indicated by the second parameter. The start node parameter is inclusive, the end node parameter is not inclusive. Duplicated memory and data transfers are activated with the thrid parameter. If the memory copies are not activated the kernels launched through this Controller object read/write directly from/to the host memory images.

* NVIDIA GPUs using CUDA
```
cuda <device_id> [ <num_streams> ]
```

A Controller is created to manage an NVIDIA GPU with the given CUDA identifier. Use the command nvidia-smi to obtain information about the devices available in a machine.

* AMD GPUs using HIP
```
hip <platform_id> <device_id> [ <num_streams> ]
```

A Controller is created to manage using HIP the selected device in the chosen platform. Use the command rocm-smi to obtain information about the platforms and devices available in a machine.

* GPU devices using OpenCL
```
opencl <platform_id> <device_id> [ <num_streams> ]
```

A Controller is created to manage with OpenCL the chosen device in the chosen platform. Use the command clinfo to obtain information about the platforms and devices available in a machine.

