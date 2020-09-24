cmake_minimum_required (VERSION 3.8)
project (JSON)

# setup vars
set(CXX_STANDARD_REQUIRED true)
set(CXX_STANDARD 11)

set(EXEC_NAME json-text)
set(LIB_NAME json)
set(LIB_SOURCE
	json/json.cpp
	json/dumps.cpp
	json/loads.cpp
)

add_library(${LIB_NAME} SHARED ${LIB_SOURCE})
add_executable(${EXEC_NAME} ${LIB_SOURCE} test.cpp)