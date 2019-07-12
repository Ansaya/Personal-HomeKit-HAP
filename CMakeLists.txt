project(HAP)
cmake_minimum_required(VERSION 3.7)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
include(FindDNSSD)
find_package(DNSSD REQUIRED)

set(HAP_LIB "HAP")

file(GLOB_RECURSE LIB_SRCS "srcs/*.cpp")
file(GLOB_RECURSE CHACHA20_SRCS "Chacha20/*.c")
file(GLOB_RECURSE CURVE25519_SRCS "curve25519/*.c")
file(GLOB_RECURSE CURVE25519_SRCS_DONNA_SRCS "ed25519-donna/*.c")
file(GLOB_RECURSE POLY1305_OPT_SRCS "poly1305-opt-master/*.c")
file(GLOB_RECURSE RFC6234_SRCS "rfc6234-master/*.c")
file(GLOB_RECURSE SRP_SRCS "srp/*.c")

add_library(${HAP_LIB} SHARED 
	"Helpers.cpp" 
	${LIB_SRCS} 
	${CHACHA20_SRCS} 
	${CURVE25519_SRCS} 
	${CURVE25519_SRCS_DONNA_SRCS} 
	${POLY1305_OPT_SRCS} 
	${RFC6234_SRCS} 
	${SRP_SRCS} )
add_library(${PROJECT_NAME}::${HAP_LIB} ALIAS ${HAP_LIB})

set_target_properties(${HAP_LIB} PROPERTIES CXX_STANDARD 11)

target_include_directories(${HAP_LIB}
	PUBLIC
	"include"
	
	PRIVATE
	"Chacha20" 
	"curve25519" 
	"ed25519-donna" 
	"poly1305-opt-master" 
	"rfc6234-master" 
	"srp"
	${DNSSD_INCLUDE_DIRS}
	)
	
target_link_libraries(${HAP_LIB} 
	PUBLIC
	OpenSSL::SSL 
	OpenSSL::Crypto 
	Threads::Threads 
	${DNSSD_LIBRARIES} )

### Installation
install(TARGETS ${HAP_LIB}
	EXPORT ${PROJECT_NAME}
	LIBRARY 
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
		COMPONENT SharedLibrary
	)

install(DIRECTORY "include"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/hap
)

### Showcase application
add_subdirectory(Example)