# To compile this with cmake 
# 1. Go to path=<tcp>
# 2. Create a build directory outside the source directory
# 3. Following are the steps to compile
#	a. mkdir build
#	b. cd build
#	c. cmake ../tcp
#	d. make
# The final executable is in the build directory <tcp>
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -v")

project(tcp)

add_executable(server server.cc)