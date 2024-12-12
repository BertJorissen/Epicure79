cmake_minimum_required (VERSION 3.20)

set(EXEC_NAME "")

# The hardware mode needs a specific option for Xilinx
if(COMPILE_XILINX)
	set(COMPILE_OPTION "hw")
endif(COMPILE_XILINX)

if(FPGA_EMULATION)
	if(COMPILE_INTEL)
		set(COMPILE_OPTION "-march=emulator")
	endif(COMPILE_INTEL)
	if(COMPILE_XILINX)
		set(COMPILE_OPTION "sw_emu")
	endif(COMPILE_XILINX)
	set(GEN_NAME_COMPMODE "-DFPGA_EMULATION")
	set(EXEC_NAME "_emu")
elseif(FPGA_PROFILING)
	set(COMPILE_OPTION "-profile")
	set(GEN_NAME_COMPMODE "-DFPGA_PROFILING")
	set(EXEC_NAME "_profiling")
	
	message("COMPILING: PROFILING MODE!!!")
endif(FPGA_EMULATION)

unset(CMAKE_CXX_FLAGS)
unset(CMAKE_C_FLAGS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CTRL_ARCH_FPGA -DCTRL_HOST_COMPILE")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -D_CTRL_ARCH_FPGA -DCTRL_HOST_COMPILE")

if(COMPILE_KERNEL_MULT)
	if(COMPILE_CTRL)
		#Compile Ctrl kernel parser that extracts CTRL_KERNEL primitive and generates program to get kernel name
		execute_process(COMMAND ${CMAKE_C_COMPILER} ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel.c -o ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel)

		#Generate program to get kernel name
		execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel kernel ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/Mult_Ctrl.cl ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c)

		#Compile program to get kernel name
		execute_process(COMMAND ${CMAKE_C_COMPILER} ${GEN_NAME_COMPMODE} -D_CTRL_ARCH_FPGA_ -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c -o ${CMAKE_CURRENT_SOURCE_DIR}/gen_name)

		#Execute program that generates kernel name and store it into a variable for further use in build process
		execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_name
			OUTPUT_VARIABLE KNAME)

		execute_process(COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/gen_name ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c)

		#Ctrl kernel
		if(COMPILE_INTEL)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}.aocx
				COMMAND aoc ${COMPILE_OPTION} -D_CTRL_ARCH_FPGA_ -DBLOCK_SIZE=16 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/Mult_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/Mult_Ctrl.cl
			)
		endif(COMPILE_INTEL)
		if(COMPILE_XILINX)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}.xclbin
			COMMAND pcpp --line-directive "" -D_CTRL_ARCH_FPGA_ -DBLOCK_SIZE=16 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH}	${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/Mult_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}-preprocessed.cl
			COMMAND v++ -j 26 -t ${COMPILE_OPTION} --platform xilinx_u280_xdma_201920_3 -c -D_CTRL_ARCH_FPGA_ -DBLOCK_SIZE=64 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}-preprocessed.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}.xo
			COMMAND v++ -j 26 -t ${COMPILE_OPTION} --platform xilinx_u280_xdma_201920_3 -l -D_CTRL_ARCH_FPGA_ -DBLOCK_SIZE=64 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}.xo -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}.xclbin
			COMMAND rm ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}-preprocessed.cl
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/Mult_Ctrl.cl
			)
		endif(COMPILE_XILINX)
	endif(COMPILE_CTRL)

	if(COMPILE_REF)
		#Ref kernel
		if(COMPILE_INTEL)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.aocx
				COMMAND aoc ${COMPILE_OPTION} -DBLOCK_SIZE=16 ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult_Ref.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.aocx
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult_Ref.cl
			)
		endif(COMPILE_INTEL)
		if(COMPILE_XILINX)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.xclbin
				COMMAND v++ -j 26 -t ${COMPILE_OPTION} -DBLOCK_SIZE=16 --platform xilinx_u280_xdma_201920_3 -c ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult_Ref.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.xo
				COMMAND v++ -j 26 -t ${COMPILE_OPTION} -DBLOCK_SIZE=16 --platform xilinx_u280_xdma_201920_3 -l ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.xo -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.xclbin
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult_Ref.cl
			)
		endif(COMPILE_XILINX)
	endif(COMPILE_REF)
	if(COMPILE_CTRL)
		if(COMPILE_INTEL)
			add_custom_target(Mult_Ctrl_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}.aocx)
		endif(COMPILE_INTEL)
		if(COMPILE_XILINX)
			add_custom_target(Mult_Ctrl_Kernel_Xilinx ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}.xclbin)
		endif(COMPILE_XILINX)
	endif(COMPILE_CTRL)
	if(COMPILE_REF)
		if(COMPILE_INTEL)
			add_custom_target(Mult_Ref_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.aocx)
		endif(COMPILE_INTEL)
		if(COMPILE_XILINX)
			add_custom_target(Mult_Ref_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.xclbin)
		endif(COMPILE_XILINX)
	endif(COMPILE_REF)
endif(COMPILE_KERNEL_MULT)


if(COMPILE_KERNEL_HOTSPOT)
	if(COMPILE_CTRL)
		#Compile Ctrl kernel parser that extracts CTRL_KERNEL primitive and generates program to get kernel name
		execute_process(COMMAND ${CMAKE_C_COMPILER} ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel.c -o ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel)

		#Generate program to get kernel name
		execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel kernel ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/Hotspot_Ctrl.cl ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c)

		#Compile program to get kernel name
		execute_process(COMMAND ${CMAKE_C_COMPILER} ${GEN_NAME_COMPMODE} -D_CTRL_ARCH_FPGA_ -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c -o ${CMAKE_CURRENT_SOURCE_DIR}/gen_name)

		#Execute program that generates kernel name and store it into a variable for further use in build process
		execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_name
			OUTPUT_VARIABLE KNAME)

		execute_process(COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/gen_name ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c)

		if(COMPILE_INTEL)
			#Ctrl kernel
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}.aocx
				COMMAND aoc ${COMPILE_OPTION} -D_CTRL_ARCH_FPGA_ -DAOCL_BOARD_de5net_a7 -DBSIZE=16 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot	${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/Hotspot_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}.aocx
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/Hotspot_Ctrl.cl
			)
		endif(COMPILE_INTEL)
		if(COMPILE_XILINX)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}.xclbin
				COMMAND pcpp --line-directive "" -D_CTRL_ARCH_FPGA_ -DAOCL_BOARD_de5net_a7 -DBSIZE=16 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot	${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/Hotspot_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}-preprocessed.cl
				COMMAND v++ -j 26 -t ${COMPILE_OPTION} --platform xilinx_u280_xdma_201920_3 -c ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}-preprocessed.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}.xo
				COMMAND v++ -j 26 -t ${COMPILE_OPTION} --platform xilinx_u280_xdma_201920_3 -l ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}.xo -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}.xclbin
				COMMAND rm ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}-preprocessed.cl
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/Hotspot_Ctrl.cl
			)
		endif(COMPILE_XILINX)
	endif(COMPILE_CTRL)

	if(COMPILE_REF)
		#Ref kernel
		if(COMPILE_INTEL)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.aocx
				COMMAND aoc ${COMPILE_OPTION} -DAOCL_BOARD_de5net_a7 -DBSIZE=16 ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot_Ref.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.aocx
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot_Ref.cl
			)
		endif(COMPILE_INTEL)
		if(COMPILE_XILINX)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.xclbin
				COMMAND v++ -j 26 -t ${COMPILE_OPTION} -DAOCL_BOARD_de5net_a7 -DBSIZE=16 --platform xilinx_u280_xdma_201920_3 -c ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot_Ref.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.xo
				COMMAND v++ -j 26 -t ${COMPILE_OPTION} -DAOCL_BOARD_de5net_a7 -DBSIZE=16 --platform xilinx_u280_xdma_201920_3 -l ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.xo -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.xclbin
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot_Ref.cl
			)
		endif(COMPILE_XILINX)
	endif(COMPILE_REF)

	if(COMPILE_CTRL)
		if(COMPILE_INTEL)
			add_custom_target(Hotspot_Ctrl_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}.aocx)
		endif(COMPILE_INTEL)
		
		if(COMPILE_XILINX)
			add_custom_target(Hotspot_Ctrl_Kernel_Xilinx ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}.xclbin)
		endif(COMPILE_XILINX)
	endif(COMPILE_CTRL)
	if(COMPILE_REF)
		if(COMPILE_INTEL)
			add_custom_target(Hotspot_Ref_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.aocx)
		endif(COMPILE_INTEL)

		if(COMPILE_XILINX)
			add_custom_target(Hotspot_Ref_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.xclbin)
		endif(COMPILE_XILINX)
	endif(COMPILE_REF)
endif(COMPILE_KERNEL_HOTSPOT)


if(COMPILE_KERNEL_SOBEL)
	if(COMPILE_CTRL)
		#Compile Ctrl kernel parser that extracts CTRL_KERNEL primitive and generates program to get kernel name
		execute_process(COMMAND ${CMAKE_C_COMPILER} ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel.c -o ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel)

		#Generate program to get kernel name
		execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel kernel ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/Sobel_Operation_Ctrl.cl ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c)

		#Compile program to get kernel name
		execute_process(COMMAND ${CMAKE_C_COMPILER} ${GEN_NAME_COMPMODE} -D_CTRL_ARCH_FPGA_ -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c -o ${CMAKE_CURRENT_SOURCE_DIR}/gen_name)

		#Execute program that generates kernel name and store it into a variable for further use in build process
		execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_name
			OUTPUT_VARIABLE KNAME)

		execute_process(COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/gen_name ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c)

		#Ctrl kernel
		if(COMPILE_INTEL)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}.aocx
				COMMAND aoc ${COMPILE_OPTION} -D_CTRL_ARCH_FPGA_ -DCOLS=1920 -DROWS=1080 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation	${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/Sobel_Operation_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/Sobel_Operation_Ctrl.cl
			)
		endif(COMPILE_INTEL)
		if(COMPILE_XILINX)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}.xclbin
				COMMAND pcpp --line-directive "" -D_CTRL_ARCH_FPGA_ -DCOLS=1920 -DROWS=1080 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/Sobel_Operation_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}-preprocessed.cl
				COMMAND v++ -j 26 -t ${COMPILE_OPTION} --platform xilinx_u280_xdma_201920_3 -c ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}-preprocessed.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}.xo
				COMMAND v++ -j 26 -t ${COMPILE_OPTION} --platform xilinx_u280_xdma_201920_3 -l ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}.xo -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}.xclbin
				COMMAND rm ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}-preprocessed.cl
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/Sobel_Operation_Ctrl.cl
			)
		endif(COMPILE_XILINX)
	endif(COMPILE_CTRL)

	if(COMPILE_REF)
		#Ref kernel
		if(COMPILE_INTEL)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.aocx
				COMMAND aoc ${COMPILE_OPTION} -DCOLS=1920 -DROWS=1080 ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation_Ref.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.aocx
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation_Ref.cl
			)
		endif(COMPILE_INTEL)
		if(COMPILE_XILINX)
			add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.xclbin
				COMMAND v++ -j 26 -t ${COMPILE_OPTION} -DCOLS=1920 -DROWS=1080 --platform xilinx_u280_xdma_201920_3 -c ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation_Ref.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.xo
				COMMAND v++ -j 26 -t ${COMPILE_OPTION} -DCOLS=1920 -DROWS=1080 --platform xilinx_u280_xdma_201920_3 -l ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.xo -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.xclbin
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation_Ref.cl
			)
		endif(COMPILE_XILINX)
	endif(COMPILE_REF)

	if(COMPILE_CTRL)
		if(COMPILE_INTEL)
			add_custom_target(Sobel_Ctrl_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}.aocx)
		endif(COMPILE_INTEL)
		if(COMPILE_XILINX)
			add_custom_target(Sobel_Ctrl_Kernel_Xilinx ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}.xclbin)
		endif(COMPILE_XILINX)
	endif(COMPILE_CTRL)
	if(COMPILE_REF)
		if(COMPILE_INTEL)
			add_custom_target(Sobel_Ref_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.aocx)
		endif(COMPILE_INTEL)
		if(COMPILE_XILINX)
			add_custom_target(Sobel_Ref_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.xclbin)
		endif(COMPILE_XILINX)
	endif(COMPILE_REF)
endif(COMPILE_KERNEL_SOBEL)

if(COMPILE_KERNEL_SUBSELECT)
	if(COMPILE_CTRL)
		#Big Kernel
		#Compile Ctrl kernel parser that extracts CTRL_KERNEL primitive and generates program to get kernel name
		execute_process(COMMAND ${CMAKE_C_COMPILER} ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel.c -o ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel)

		#Generate program to get kernel name
		execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel kernel ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Big_Kernel/Big_Kernel_Ctrl.cl ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c)

		#Compile program to get kernel name
		execute_process(COMMAND ${CMAKE_C_COMPILER} ${GEN_NAME_COMPMODE} -D_CTRL_ARCH_FPGA_ -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Big_Kernel ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c -o ${CMAKE_CURRENT_SOURCE_DIR}/gen_name)

		#Execute program that generates kernel name and store it into a variable for further use in build process
		execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_name
				OUTPUT_VARIABLE KNAME)

		execute_process(COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/gen_name ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c)

		#Ctrl kernel
		add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Big_Kernel/${KNAME}
				COMMAND aoc ${COMPILE_OPTION} -D_CTRL_ARCH_FPGA_ -I${CTRL_EXPERIMENTATION_PATH}/SubselecTest/src -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Big_Kernel	${CTRL_KERNELS_PATH}/FPGA/Ctrl/Big_Kernel/Big_Kernel_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Big_Kernel/${KNAME}
				DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Big_Kernel/Big_Kernel_Ctrl.cl
		)

		#Some Kernel
		#Compile Ctrl kernel parser that extracts CTRL_KERNEL primitive and generates program to get kernel name
		execute_process(COMMAND ${CMAKE_C_COMPILER} ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel.c -o ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel)

		#Generate program to get kernel name
		execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel kernel ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Some_Kernel/Some_Kernel_Ctrl.cl ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c)

		#Compile program to get kernel name
		execute_process(COMMAND ${CMAKE_C_COMPILER} ${GEN_NAME_COMPMODE} -D_CTRL_ARCH_FPGA_ -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Some_Kernel ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c -o ${CMAKE_CURRENT_SOURCE_DIR}/gen_name)

		#Execute program that generates kernel name and store it into a variable for further use in build process
		execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_name
				OUTPUT_VARIABLE KNAME)

		execute_process(COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/gen_name ${CMAKE_CURRENT_SOURCE_DIR}/parser_ctrlkernel ${CMAKE_CURRENT_SOURCE_DIR}/ctrl_kernel.c)

		#Ctrl kernel
		add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Some_Kernel/${KNAME}
						COMMAND aoc ${COMPILE_OPTION} -D_CTRL_ARCH_FPGA_ -I${CTRL_EXPERIMENTATION_PATH}/SubselecTest/src -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Some_Kernel	${CTRL_KERNELS_PATH}/FPGA/Ctrl/Some_Kernel/Some_Kernel_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Some_Kernel/${KNAME}
						DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Some_Kernel/Some_Kernel_Ctrl.cl
				)
	endif(COMPILE_CTRL)
endif(COMPILE_KERNEL_SUBSELECT)
