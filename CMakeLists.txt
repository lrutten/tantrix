cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_AUTOMOC ON)

project(tantrix)

#include_directories(include)

file(GLOB SOURCES "src/*.cpp")

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
add_executable(tantrix ${SOURCES})
target_link_libraries(tantrix Qt5::Gui Qt5::Widgets)

#add_custom_target(tests)
#add_executable(pandoc-table-filter-test ${SOURCES})
#add_dependencies(tests pandoc-table-filter-test)
#target_compile_definitions(pandoc-table-filter-test DEBUG)

#find_package(Qt6 COMPONENTS Widgets DBus REQUIRED)
#add_executable(publisher publisher.cpp)
#target_link_libraries(publisher Qt6::Widgets Qt6::DBus)

#find_package(Qt4 REQUIRED)
#add_executable(subscriber2 subscriber2.cpp)
#target_link_libraries(subscriber2 Qt4::QtGui Qt4::QtDBus)

