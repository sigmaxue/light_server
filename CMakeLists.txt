cmake_minimum_required(VERSION 3.21)
project(light_server)

message(${PROJECT_BINARY_DIR})
message(${PROJECT_SOURCE_DIR})
message(${CMAKE_SOURCE_DIR})
message(${CMAKE_BINARY_DIR})
message("PROJECT:"${CMAKE_PROJECT_NAME})




include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# add_subdirectory(src)
# add_subdirectory(core)
# add_subdirectory(connection)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/core SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/global SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/connection SRCS)

add_executable(${CMAKE_PROJECT_NAME} ${SRCS})

