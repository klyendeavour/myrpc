aux_source_directory(./ SRC)

set(CMAKE_BUILD_TYPE "Debug")
set(EXECUTABLE_OUTPUT_PATH ${EXE_PATH})
add_executable(provider ${SRC} ../user.pb.cc)
target_link_libraries(provider myrpc protobuf)
link_directories(/usr/local/protobuf/include/google)