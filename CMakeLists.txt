#支持的cmake最低版本号
cmake_minimum_required(VERSION 3.0)
project(main)
#配置编译选项
set(CMKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#配置最终的可执行文件的输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
# 加载子目录
add_subdirectory(src)