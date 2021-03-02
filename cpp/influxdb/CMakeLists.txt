cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)

project(influx-plugin)

find_package(nng)

# add_compile_options(-Wall -O3) #release
add_compile_options(-Wall -O3 -Wno-format-truncation -Wno-psabi) #no-psabi suppress a warning on the JSOn library for GCC 8.x compiler

link_directories("/usr/local/lib")
set(LIBS -lrt -lnng -lpthread -ldl -lstdc++fs -latomic) #stdc++fs and atomic is needed for GCC8.x 

add_executable(influx-plugin
	src/main.cpp
)

# target_compile_options(ubridge PUBLIC -Wall)
target_include_directories(influx-plugin PUBLIC 
	"../include"
	"../3dparty/nngpp/include"
	"../3dparty/nlohmann"
	"../3dparty/cxxopts/include"
	"../3dparty")
# target_link_libraries(ubridge -lrt -lpthread)
target_link_libraries(influx-plugin ${LIBS})

# add_executable(consumer
# 	src/consumer.cpp
# )

# target_compile_options(consumer PUBLIC -Wall)
# target_include_directories(consumer PUBLIC "include")
# target_link_libraries(consumer ${LIBS})



#  cd build && cmake -GNinja ..
#  ninja

