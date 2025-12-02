# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(MyCProject C)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/Agreements
                    ${PROJECT_SOURCE_DIR}/HalBsp
                    ${PROJECT_SOURCE_DIR})
                    

# 定义源文件
set(SOURCES
    main.c
)

file (GLOB_RECURSE SOURCES_FATFS 
    ${PROJECT_SOURCE_DIR}/Agreements/*.c
    ${PROJECT_SOURCE_DIR}/HalBsp/*.c
    )

# 创建可执行文件
add_executable(MyExecutable ${SOURCES} ${SOURCES_FATFS})
