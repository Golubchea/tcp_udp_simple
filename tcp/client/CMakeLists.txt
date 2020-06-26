cmake_minimum_required(VERSION 2.8)
project(client)


file(GLOB_RECURSE SRC src/*.cpp src/*.c src/*.h)
include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${SRC})
