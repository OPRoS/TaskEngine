cmake_minimum_required(VERSION 2.6)
set(LIB_NAME TPLutility)
PROJECT(${LIB_NAME})

FILE(GLOB_RECURSE LIBSRCS src/*.cpp src/*.c)

INCLUDE_DIRECTORIES(
        ${MY_INCLUDE_DIR}
	${ANTLR_DIR}/include
)

ADD_LIBRARY(${LIB_NAME} STATIC ${LIBSRCS})