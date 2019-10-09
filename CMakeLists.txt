##########################################################################
# Project Setup                                                          #
##########################################################################
cmake_minimum_required(VERSION 3.11)

project(LongFiDevice
  VERSION   1.0
  LANGUAGES C
  )

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

add_compile_options(-fPIC)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
##########################################################################
# External                                                               #
##########################################################################
include(FetchContent)
FetchContent_Declare(
  lfc
  GIT_REPOSITORY ssh://git@github.com/helium/longfi-core.git
  GIT_TAG        19d432928b089d170977730f87c5ca5d4069913e
  )
FetchContent_GetProperties(lfc)
if(NOT lfc)
  FetchContent_Populate(lfc)
  add_subdirectory(${lfc_SOURCE_DIR} ${lfc_BINARY_DIR} ${lfc_INCLUDE_DIR})
endif()

##########################################################################
# Local                                                                  #
##########################################################################
add_subdirectory(radio)

add_library(longfi
  board.c
  longfi.c
)

target_link_libraries(longfi
  PUBLIC lfc golay cursor cursor_varint
  )
  
target_include_directories(longfi
  PUBLIC ${PROJECT_SOURCE_DIR}
  )

target_compile_options(longfi PRIVATE -fPIC)

install(TARGETS longfi lfc golay cursor cursor_varint
        DESTINATION lib
  )
