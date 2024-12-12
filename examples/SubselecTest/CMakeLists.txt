cmake_minimum_required (VERSION 3.20)

if (SUPPORT_FPGA)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DKERNELTYPE=TASK -DCTRL_KERNEL_PATH=${CTRL_KERNELS_PATH_FPGA} -DREF_KERNEL_PATH=${REF_KERNELS_PATH_FPGA}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKERNELTYPE=TASK -DCTRL_KERNEL_PATH=${CTRL_KERNELS_PATH_FPGA} -DREF_KERNEL_PATH=${REF_KERNELS_PATH_FPGA}")
endif(SUPPORT_FPGA)

if (CTRL_CMAKE_VERBOSE_EXAMPLES_SUBSELECTEST)
	message(STATUS "TEST_INCLUDE DIRS = ${CTRL_INCLUDE_DIRS}")
	message(STATUS "TEST_LIBS = ${CTRL_LIBS}")
	message(STATUS "TEST_CTRL_C vars = ${CMAKE_C_FLAGS}")
	message(STATUS "TEST_CTRL_CXX vars = ${CMAKE_CXX_FLAGS}")
	message(STATUS "TEST_CTRL_CUDA vars = ${CMAKE_CUDA_FLAGS}")
endif (CTRL_CMAKE_VERBOSE_EXAMPLES_SUBSELECTEST)

include_directories(${CTRL_INCLUDE_DIRS})

if (SUPPORT_CUDA)
	add_executable(SubselecTest_Ctrl src/SubselecTest_Ctrl.cu)
elseif(SUPPORT_HIP)
	add_executable(SubselecTest_Ctrl src/SubselecTest_Ctrl.cpp)
else (SUPPORT_CUDA)
	add_executable(SubselecTest_Ctrl src/SubselecTest_Ctrl.c)
endif(SUPPORT_CUDA)

if (SUPPORT_FPGA)
	if(COMPILE_KERNELS)
		option(COMPILE_KERNEL_SUBSELECT "COMPILE_KERNEL_SUBSELECT" ON)
	else(COMPILE_KERNELS)
		option(COMPILE_KERNEL_SUBSELECT "COMPILE_KERNEL_SUBSELECT" OFF)
	endif(COMPILE_KERNELS)

	execute_process(COMMAND ${CMAKE_C_COMPILER} ${KERNELS_PATH}/FPGA/parser_ctrlkernel.c -o ${KERNELS_PATH}/FPGA/parser_ctrlkernel)

	execute_process(COMMAND ${KERNELS_PATH}/FPGA/parser_ctrlkernel host ${CTRL_KERNELS_PATH_FPGA}/Some_Kernel/Some_Kernel_Ctrl.cl ${CTRL_KERNELS_PATH_FPGA}/Some_Kernel/Some_Kernel_Ctrl.c)
	execute_process(COMMAND ${KERNELS_PATH}/FPGA/parser_ctrlkernel host ${CTRL_KERNELS_PATH_FPGA}/Big_Kernel/Big_Kernel_Ctrl.cl ${CTRL_KERNELS_PATH_FPGA}/Big_Kernel/Big_Kernel_Ctrl.c)

	execute_process(COMMAND rm ${KERNELS_PATH}/FPGA/parser_ctrlkernel)

	include_directories(src/ ${KERNELS_PATH})

	target_sources(SubselecTest_Ctrl PRIVATE ${CTRL_KERNELS_PATH_FPGA}/Some_Kernel/Some_Kernel_Ctrl.c ${CTRL_KERNELS_PATH_FPGA}/Big_Kernel/Big_Kernel_Ctrl.c)
endif(SUPPORT_FPGA)
target_link_libraries(SubselecTest_Ctrl ${CTRL_LIBS})


add_executable(SubselectTest_Ref src/SubselecTest_Ref.c)
target_link_libraries(SubselectTest_Ref ${CTRL_LIBS})