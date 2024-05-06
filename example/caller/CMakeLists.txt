aux_source_directory(./ SRC)

set(EXECUTABLE_OUTPUT_PATH ${EXE_PATH})
add_executable(consumer ${SRC} ../user.pb.cc)
target_link_libraries(consumer myrpc protobuf)
link_directories(/usr/local/protobuf/include/google)