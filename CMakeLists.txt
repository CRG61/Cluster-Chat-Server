cmake_minimum_required(VERSION 3.0)
project(CHATSERVER)

# 配置编译选项  这个变量名是预定义的
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 配置最终的可执行文件输出路径 
# EXECUTABLE_OUTPUT_PATH PROJECT_SOURCE_DIR是预定义变量名
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/redis)
include_directories(${PROJECT_SOURCE_DIR}/3rd\ part)


# 加载子目录 （下一级CMAKE的套娃读取）
add_subdirectory(src)