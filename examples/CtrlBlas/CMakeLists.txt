cmake_minimum_required (VERSION 3.20)

set(CTRL_INCLUDE_DIRS ${CTRL_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/CtrlBlas/)

# Add blas examples
add_subdirectory(Matrix_Add)
add_subdirectory(gemver)
add_subdirectory(Matrix_Pow)
#add_subdirectory(Sobel_YUV)
