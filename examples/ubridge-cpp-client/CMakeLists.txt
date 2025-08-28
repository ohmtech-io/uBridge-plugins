cmake_minimum_required(VERSION 3.13)
project(ubridge-client-example)
set(CMAKE_CXX_STANDARD 17)

find_package(nng)

add_compile_options(-Wall -O3 -Wno-format-truncation -Wno-psabi)
add_compile_options(-DLOGURU_WITH_STREAMS)

link_directories("/usr/local/lib")
set(LIBS -lrt -lnng -lpthread -ldl -lstdc++fs -latomic)

add_executable(ubridge-client-example
    main.cpp
)

set(UBRIDGE_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../sources/uBridge/include")

target_include_directories(ubridge-client-example PUBLIC
    "${UBRIDGE_INCLUDE_DIR}"
)

if(TARGET nng::nng)
    target_link_libraries(ubridge-client-example nng::nng pthread dl)
else()
    target_link_libraries(ubridge-client-example ${LIBS})
endif()

message(STATUS "Using uBridge includes at: ${UBRIDGE_INCLUDE_DIR}")
