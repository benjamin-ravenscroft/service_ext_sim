cmake_minimum_required(VERSION 3.0.0)
project(simulation VERSION 0.1.0 LANGUAGES C CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(CTest)
enable_testing()

include_directories(include)

set(SOURCES
    src/simulation.cpp
    src/DischargeList.cpp
    src/Patient.cpp
    src/Server.cpp
    src/Waitlist.cpp
)

find_package(Arrow REQUIRED)
find_package(Parquet REQUIRED)

add_executable(simulation ${SOURCES})
target_link_libraries(simulation PRIVATE Arrow::arrow_shared ${PARQUET_SHARED_LIB})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
