cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)

project(influx-plugin)

find_package(nng)

# add_compile_options(-Wall -O3) #release
add_compile_options(-Wall -O3 -Wno-format-truncation -Wno-psabi ) #no-psabi suppress a warning on the JSOn library for GCC 8.x compiler
add_compile_options(-DLOGURU_WITH_STREAMS)

link_directories("/usr/local/lib")
set(LIBS -lrt -lnng -lpthread -ldl -lcurl -lstdc++fs -latomic) #stdc++fs and atomic is needed for GCC8.x 

add_executable(influx-plugin
	../3dparty/loguru/loguru.cpp
	src/main.cpp
	src/influx.cpp
	influx-plugin-config.json
)

# target_compile_options(ubridge PUBLIC -Wall)
target_include_directories(influx-plugin PUBLIC 
	"../common"
	"../3dparty/nngpp/include"
	"../3dparty/nlohmann"
	"../3dparty/cxxopts/include"
	"../3dparty/loguru")
# target_link_libraries(ubridge -lrt -lpthread)
target_link_libraries(influx-plugin ${LIBS})

install(TARGETS influx-plugin DESTINATION bin COMPONENT binaries)
install(FILES influx-plugin-config.json DESTINATION /etc/ubridge COMPONENT config)

# add_executable(consumer
# 	src/consumer.cpp
# )

# target_compile_options(consumer PUBLIC -Wall)
# target_include_directories(consumer PUBLIC "include")
# target_link_libraries(consumer ${LIBS})



#  cd build && cmake -GNinja ..
#  ninja

