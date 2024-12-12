/**
 * @file ctrl_print_info.h
 * @brief Helper function to print information of all ctrl devices
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include "Ctrl.h"

// TODO @sergioalo should this be here or be part of ctrl library?
// TODO @sergioalo should exp mode be controlled via compilation macro or param flag?
// TODO @sergioalo consider printing host affinity

/**
 * @brief Pretty print information abult ctrl devices
 *
 * Must be called inside __ctrl_block__
 */
static inline void Ctrl_PrintInfo() {
	for (int i = 0; i < Ctrl_GetNCtrls(); i++) {
		Ctrl_Info info = Ctrl_GetInfo(Ctrl_Get(i));

		#ifdef _CTRL_EXAMPLES_EXP_MODE_
		if (!strcmp(info.type, "CUDA") || !strcmp(info.type, "HIP")) {
			printf("%s-%s", info.type, info.device_name);
		} else if (!strcmp(info.type, "OpenCL_GPU")) {
			printf("%s-%s-%s", "OpenCL-GPU", info.device_name, info.platform_name);
		} else if (!strcmp(info.type, "FPGA")) {
			printf("%s-%s-%s", info.type, info.device_name, info.platform_name);
		} else if (!strcmp(info.type, "CPU")) {
			printf("%s-%d-%d-%d-%s", info.type, info.n_threads, info.numa_range_min, info.numa_range_max, info.mem_transfers ? "ON" : "OFF");
		} else {
			fprintf(stderr, "[Ctrl_PrintInfo] ERROR: unknown ctrl type %s\n", info.type);
			exit(EXIT_FAILURE);
		}

		// separator between devices
		if (i < Ctrl_GetNCtrls() - 1) {
			printf("#");
		}

		#else // _CTRL_EXAMPLES_EXP_MODE_
		printf("\n\n CTRL TYPE: %s", info.type);
		printf("\n DEVICE: %s", info.device_name);

		if (!strcmp(info.type, "CUDA") || !strcmp(info.type, "HIP")) continue;

		if (!strcmp(info.type, "OpenCL_GPU")) {
			printf("\n PLATFORM: %s", info.platform_name);
		} else if (!strcmp(info.type, "FPGA")) {
			printf("\n PLATFORM: %s", info.platform_name);
		} else if (!strcmp(info.type, "CPU")) {
			printf("\n KERNEL THREADS: %d", info.n_threads);
			printf("\n KERNEL NUMA RANGE: %d-%d", info.numa_range_min, info.numa_range_max);
			printf("\n MEM_MOVES: %s", info.mem_transfers ? "ON" : "OFF");
		} else {
			fprintf(stderr, "[Ctrl_PrintInfo] ERROR: unknown ctrl type %s\n", info.type);
			exit(EXIT_FAILURE);
		}
		#endif // _CTRL_EXAMPLES_EXP_MODE_
	}
	#ifdef _CTRL_EXAMPLES_EXP_MODE_
	printf(", ");
	#endif // _CTRL_EXAMPLES_EXP_MODE_
}
