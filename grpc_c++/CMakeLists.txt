cmake_minimum_required(VERSION 3.16.3)
project(grpcpp_test)

#用已经写好的cmake模块作为辅助
include(./common.cmake)
#stdc++=11
set(CMAKE_CXX_STANDARD 11)
#将这两个文件夹中的内容加入编译范围
AUX_SOURCE_DIRECTORY( source/ SOURCEFILE)
AUX_SOURCE_DIRECTORY( header/ HEADER)
#先编译出链接文件
add_library(DEPEND ${SOURCEFILE} ${HEADER})
target_link_libraries(DEPEND
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})
#对每个需要作为执行目标的文件进行编译同时加入编译选项
foreach(_target
        client server)
    add_executable(${_target} target/${_target}.cpp)
    target_link_libraries(${_target}
            DEPEND
            ${_GRPC_GRPCPP}
            ${_PROTOBUF_LIBPROTOBUF})
endforeach()

