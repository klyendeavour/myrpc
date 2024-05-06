# 设置cmake的最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
#项目myrpc
project(myrpc)

# 生成debug版本，可以进行gdb调试
set(CMAKE_BUILD_TYPE "Debug")


#生成的可执行文件的路径
set(EXE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#生成的动态库存放路径
set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

#头文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/example)
#子目录
add_subdirectory(example/callee)
add_subdirectory(example/caller)
add_subdirectory(src)