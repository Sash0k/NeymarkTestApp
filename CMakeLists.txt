cmake_minimum_required(VERSION 3.10)
project (simple-service VERSION 1.0.0 LANGUAGES CXX)
include(GNUInstallDirs)

set(APP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(APP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB APP_HEADERS ${APP_INCLUDE_DIR}/*.h)
file(GLOB APP_SOURCE ${APP_SOURCE_DIR}/*.cpp)

set(APP_HEADERS_LIST ${APP_INCLUDE_DIR})

include_directories(${APP_HEADERS_LIST})

# Выделяем протокол в общую библиотеку, для упрощения разработки новых сервисов
add_library(transport SHARED lib/Transport.cpp)
set_target_properties(transport PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER lib/Transport.h)
	
add_executable(${PROJECT_NAME} ${APP_SOURCE})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")

target_link_libraries(${PROJECT_NAME} transport)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION} DESTINATION bin)
