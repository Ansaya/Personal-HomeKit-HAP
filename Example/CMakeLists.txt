project(libHAPExample)
cmake_minimum_required(VERSION 3.7)

add_executable(hap_showcase "main.cpp")
set_target_properties(hap_showcase PROPERTIES CXX_STANDARD 11)
target_include_directories(hap_showcase
	PRIVATE
	${DNSSD_INCLUDE_DIRS} )
target_link_libraries(hap_showcase
	PUBLIC
	HAP::HAP )

install(TARGETS hap_showcase DESTINATION ${PROJECT_SOURCE_DIR}/..)
