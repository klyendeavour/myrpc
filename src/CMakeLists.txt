

#搜寻当前目录下的所有源文件
aux_source_directory(./ SRC)



#把库文件放入指定路径
set(LIBRARY_OUTPUT_PATH ${LIB_PATH})
#生成动态库 myrpc
add_library(myrpc STATIC ${SRC} header.pb.cc)
target_link_libraries(myrpc muduo_net muduo_base pthread libzookeeper_mt.a)
                                                         
