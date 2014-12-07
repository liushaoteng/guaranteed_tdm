#project name
cmake_minimum_required(VERSION 2.8)

PROJECT(ppb_cpp_hard)
set(CMAKE_BUILD_TYPE Release)

#head file path
INCLUDE_DIRECTORIES(/afs/it.kth.se/home/l/i/liu2/systemc/systemc-2.3.0/include)
INCLUDE_DIRECTORIES(/afs/it.kth.se/home/l/i/liu2/systemc/TLM-2009-07-15/include/tlm)
#source directory
AUX_SOURCE_DIRECTORY(../src/core_of_single_router CORE_SRCS)
AUX_SOURCE_DIRECTORY(../src/main_and_map MAIN_SRCS)
AUX_SOURCE_DIRECTORY(../src/test_functions TEST_SRCS)


 MESSAGE(STATUS "ALL THE SOURCE FILES IS " ${CORE_SRCS} ${MAIN_SRCS} ${TEST_SRCS})
#set environment variable
SET(TEST_MATH ${CORE_SRCS} ${MAIN_SRCS} ${TEST_SRCS})

#add executable file
ADD_EXECUTABLE(ppb_cpp ${TEST_MATH})
#set extern librariesls

SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -DNDEBUG -DCONCISE -pthread")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall  -g -ggdb -rdynamic -pthread")

SET(EXECUTABLE_OUTPUT_PATH ../test_bench/)
#ADD_DEFINITIONS(-DDEBUG)

#add link library
set (LIB_SYSTEMC /afs/it.kth.se/home/l/i/liu2/systemc/systemc-2.3.0/lib-linux64/libsystemc.a)

#FIND_LIBRARY(LIB_SYSTEMC systemc /afs/it.kth.se/home/l/i/liu2/systemc/systemc-2.3.0/lib-linux64)
# IF (LIB_SYSTEMC)
	MESSAGE(STATUS "systemc lib IS " ${LIB_SYSTEMC})
	TARGET_LINK_LIBRARIES(ppb_cpp ${LIB_SYSTEMC})
# ENDIF(LIB_SYSTEMC)







