cmake_minimum_required(VERSION 3.1)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Debug)

project(demo VERSION 1.0.0 LANGUAGES CXX)

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

include_directories("${CMAKE_SOURCE_DIR}/include")

find_library(CPPCONN mysqlcppconn
    PATHS /usr/lib64 /usr/lib /opt/mysqlcppconn
    )
find_path(CPPCONN_INCLUDE mysql_connection.h)
if(${CPPCONN_INCLUDE_NOTFOUND})
    message(ERROR "libmysqlcppconn has not been installed")
endif()
#message("CPPCONN Found At ${CPPCONN_INCLUDE}")

add_executable(demo demo.cpp)
target_link_libraries(demo mysqlcppconn)

install(TARGETS demo RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin)
