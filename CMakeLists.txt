# CMakeList.txt : CMake project for Createnet, include source and define
# project specific logic here.
#

file(GLOB SOURCES "*.c" "glad.c" "stb/*.c" "*.cpp" "Createnet/*.c" "Createnet/*.cpp" "Createnet/Networking/*.c" "Createnet/Networking/*.cpp")
file(GLOB HEADERS "*.h" "glad/*.h" "stb/*.h" "*.hpp" "Createnet/*.h" "Createnet/*.hpp" "Createnet/Networking/*.h" "Createnet/Networking/*.hpp")
add_executable (Createnet "${SOURCES}" "${HEADERS}" "version.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Createnet PROPERTY CXX_STANDARD 20)
endif()

if (WIN32)
	set(GLFW3_DIR "C:/Program Files (x86)/GLFW")
	set(GLFW3_LIB "glfw3.lib")
	set(ASSIMP_DIR "C:/Program Files/Assimp")
	set(ASSIMP_LIB "assimp-vc143-mtd.lib")
	set(GLM_DIR "C:/Program Files (x86)/glm")
	set(GLM_LIB "glm.lib")
	add_custom_command(
		TARGET Createnet
		PRE_BUILD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Createnet/
		COMMAND python3 ${CMAKE_SOURCE_DIR}/Createnet/prebuild.py
	)
	add_custom_command(
		TARGET Createnet
		POST_BUILD
		COMMAND C:/cygwin64/bin/cp.exe -r C:/Users/User/source/repos/Createnet/Createnet \\\\192.168.50.155\\sigmashare\\Createnet-src\\
		COMMAND C:/cygwin64/bin/cp.exe -r C:/Users/User/source/repos/Createnet/CMakeLists.txt \\\\192.168.50.155\\sigmashare\\Createnet-src\\
	)
endif()

if (APPLE OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message("Configuring for macOS build (Xcode probably)")
	set(GLFW3_DIR "/opt/homebrew")
	set(GLFW3_LIB "libglfw.dylib")
	set(ASSIMP_DIR "/opt/homebrew")
	set(ASSIMP_LIB "libassimp.dylib")
	set(GLM_DIR "/opt/homebrew")
	set(GLM_LIB "libglm.dylib")
endif()

find_package(OpenGL REQUIRED)
target_link_libraries(Createnet PRIVATE OpenGL::GL)

list(APPEND CMAKE_PREFIX_PATH GLFW3_DIR)
include_directories("${GLFW3_DIR}/include")
link_directories("${GLFW3_DIR}/lib")
add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES IMPORTED_LOCATION "${GLFW3_DIR}/lib/${GLFW3_LIB}")

list(APPEND CMAKE_PREFIX_PATH ASSIMP_DIR)
include_directories("${ASSIMP_DIR}/include")
link_directories("${ASSIMP_DIR}/lib")
add_library(assimp STATIC IMPORTED)
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION "${ASSIMP_DIR}/lib/${ASSIMP_LIB}")

list(APPEND CMAKE_PREFIX_PATH GLM_DIR)
include_directories("${GLM_DIR}/include")
link_directories("${GLM_DIR}/lib")
add_library(glm STATIC IMPORTED)
set_target_properties(glm PROPERTIES IMPORTED_LOCATION "${GLM_DIR}/lib/${GLM_LIB}")

set(BOOST_INCLUDE_LIBRARIES thread filesystem system asio date_time)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
	Boost
	GIT_REPOSITORY https://github.com/boostorg/boost.git
	GIT_PROGRESS TRUE
	GIT_TAG boost-1.87.0
)
FetchContent_MakeAvailable(Boost)

if (WIN32)
	add_library(zlibstatic STATIC IMPORTED)
	set_target_properties(zlibstatic PROPERTIES IMPORTED_LOCATION "${ASSIMP_DIR}/lib/zlibstaticd.lib")
endif()

find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(Createnet PRIVATE glfw3)
target_link_libraries(Createnet PRIVATE assimp)
target_link_libraries(Createnet PRIVATE glm)
target_link_libraries(Createnet PRIVATE zlibstatic)
target_link_libraries(Createnet PRIVATE Boost::asio Boost::filesystem Boost::thread)