cmake_minimum_required( VERSION 3.24)
project( elkirtasse VERSION 3.6.8) # LANGUAGES CXX)

find_package(
  Qt6 REQUIRED COMPONENTS Core Widgets Gui Network PrintSupport Sql Xml )

add_subdirectory( qbooks)
add_subdirectory( kirtassenetwork)
add_subdirectory( kirtasserowat)
add_subdirectory( kirtassecdrom)

set( CMAKE_INCLUDE_CURRENT_DIR ON)
include( GNUInstallDirs)
