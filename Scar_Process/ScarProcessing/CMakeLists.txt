cmake_minimum_required(VERSION 2.8)

project(ScarProcessing)
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(ScarProcessing MACOSX_BUNDLE ScarProcessing)

target_link_libraries(ScarProcessing
    ${ITK_LIBRARIES} ${VTK_LIBRARIES})






