project(cmake_vtk)

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(VTK COMPONENTS
  CommonColor
  CommonCore
  IOImage
  InteractionImage
  FiltersSources
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "Unable to find the VTK build folder.")
endif()

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")

include_directories($(CMAKE_CURRENT_SOURCE_DIR)/source)

add_executable(main source/main.cpp)
add_executable(readerfile source/ReaderTextFile.cpp)
add_executable(drawregion source/DrawRegion.cpp)
add_executable(drawimage source/DrawImage.cpp)

target_link_libraries(main PRIVATE ${VTK_LIBRARIES} )
target_link_libraries(readerfile PRIVATE ${VTK_LIBRARIES} )
target_link_libraries(drawregion PRIVATE ${VTK_LIBRARIES} )
target_link_libraries(drawimage PRIVATE ${VTK_LIBRARIES} )

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS main readerfile drawregion drawimage
  MODULES ${VTK_LIBRARIES}
)

