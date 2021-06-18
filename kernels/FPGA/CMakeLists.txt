# <license>
# 
# Controller v2.1
# 
# This software is provided to enhance knowledge and encourage progress in the scientific
# community. It should be used only for research and educational purposes. Any reproduction
# or use for commercial purpose, public redistribution, in source or binary forms, with or 
# without modifications, is NOT ALLOWED without the previous authorization of the copyright 
# holder. The origin of this software must not be misrepresented; you must not claim that you
# wrote the original software. If you use this software for any purpose (e.g. publication),
# a reference to the software package and the authors must be included.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
# All rights reserved.
# 
# More information on http://trasgo.infor.uva.es/
# 
# </license>
cmake_minimum_required (VERSION 2.6)


set(EXEC_NAME "")
if(FPGA_EMULATION)
	set(COMPILE_OPTION "-march=emulator")
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
		add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}
			COMMAND aoc ${COMPILE_OPTION} -D_CTRL_ARCH_FPGA_ -DBLOCK_SIZE=64 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/Mult_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME}
			DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/Mult_Ctrl.cl
		)
	endif(COMPILE_CTRL)

	if(COMPILE_REF)
		#Ref kernel
		add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.aocx
			COMMAND aoc ${COMPILE_OPTION} -DBLOCK_SIZE=64 ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult_Ref.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.aocx
			DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult_Ref.cl
		)
	endif(COMPILE_REF)

	if(COMPILE_CTRL)
		add_custom_target(Mult_Ctrl_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Mult/${KNAME})
	endif(COMPILE_CTRL)
	if(COMPILE_REF)
		add_custom_target(Mult_Ref_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Mult/Mult${EXEC_NAME}_Ref.aocx)
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

		#Ctrl kernel
		add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}
			COMMAND aoc ${COMPILE_OPTION} -D_CTRL_ARCH_FPGA_ -DAOCL_BOARD_de5net_a7 -DBSIZE=16 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot	${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/Hotspot_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME}
			DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/Hotspot_Ctrl.cl
		)
	endif(COMPILE_CTRL)

	if(COMPILE_REF)
		#Ref kernel
		add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.aocx
			COMMAND aoc ${COMPILE_OPTION} -DAOCL_BOARD_de5net_a7 -DBSIZE=16 ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot_Ref.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.aocx
			DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot_Ref.cl
		)
	endif(COMPILE_REF)

	if(COMPILE_CTRL)
		add_custom_target(Hotspot_Ctrl_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Hotspot/${KNAME})
	endif(COMPILE_CTRL)
	if(COMPILE_REF)
		add_custom_target(Hotspot_Ref_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Hotspot/Hotspot${EXEC_NAME}_Ref.aocx)
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
		add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}
			COMMAND aoc ${COMPILE_OPTION} -D_CTRL_ARCH_FPGA_ -DCOLS=1920 -DROWS=1080 -I${CTRL_HEADER_PATH} -I${CTRL_KERNELS_PATH} -I${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation	${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/Sobel_Operation_Ctrl.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME}
			DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/Sobel_Operation_Ctrl.cl
		)
	endif(COMPILE_CTRL)

	if(COMPILE_REF)
		#Ref kernel
		add_custom_command(OUTPUT ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.aocx
			COMMAND aoc ${COMPILE_OPTION} -DCOLS=1920 -DROWS=1080 ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation_Ref.cl -o ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.aocx
			DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation_Ref.cl
		)
	endif(COMPILE_REF)

	if(COMPILE_CTRL)
		add_custom_target(Sobel_Ctrl_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ctrl/Sobel_Operation/${KNAME})
	endif(COMPILE_CTRL)
	if(COMPILE_REF)
		add_custom_target(Sobel_Ref_Kernel ALL DEPENDS ${CTRL_KERNELS_PATH}/FPGA/Ref/Sobel_Operation/Sobel_Operation${EXEC_NAME}_Ref.aocx)
	endif(COMPILE_REF)
endif(COMPILE_KERNEL_SOBEL)
