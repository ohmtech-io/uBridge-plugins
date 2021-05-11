cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)

project(mqtt-plugin)

find_package(nng)

# add_compile_options(-Wall -O3) #release
add_compile_options(-Wall -O3 -Wno-format-truncation -Wno-psabi ) #no-psabi suppress a warning on the JSOn library for GCC 8.x compiler
add_compile_options(-DLOGURU_WITH_STREAMS)

link_directories("/usr/local/lib")
set(LIBS -lrt -lnng -lpthread -ldl -lstdc++fs -latomic -lpaho-mqtt3as -lpaho-mqttpp3 ) #stdc++fs and atomic is needed for GCC8.x 

add_executable(mqtt-plugin
	../3dparty/loguru/loguru.cpp
	src/main.cpp
	src/mqttClient.cpp
)

target_include_directories(mqtt-plugin PUBLIC 
	"../common"
	"../3dparty/nngpp/include"
	"../3dparty/nlohmann"
	"../3dparty/cxxopts/include"
	"../3dparty/loguru")

target_link_libraries(mqtt-plugin ${LIBS})

install(TARGETS mqtt-plugin DESTINATION bin COMPONENT binaries)
install(FILES mqtt-plugin-config.json DESTINATION /etc/ubridge COMPONENT config)
install(FILES ubridge-mqtt-plugin.service DESTINATION /etc/systemd/system/ COMPONENT service)
