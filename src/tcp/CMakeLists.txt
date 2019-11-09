# To compile this with cmake 
# 1. Go to path=<tcp>
# 2. Create a build directory outside the source directory
# 3. Following are the steps to compile
#	a. mkdir build
#	b. cd build
#	c. cmake ../tcp
#	d. make
# The final executable is in the build directory <tcp>
project(tcp)

add_executable(server server.cc)
