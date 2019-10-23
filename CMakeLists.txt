##########################################################################
# Project Setup                                                          #
##########################################################################
cmake_minimum_required(VERSION 3.11)

project(longfi
  VERSION   1.0
  LANGUAGES C
  )

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

##########################################################################
# External                                                               #
##########################################################################
include(FetchContent)
FetchContent_Declare(
  lfc
  GIT_REPOSITORY ssh://git@github.com/helium/longfi-core.git
  GIT_TAG        ea66583679f6d4e6e03e9b6385f1c4424cc89a2a
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
  PUBLIC lfc golay cursor cursor_varint sx12xx
  )
  
target_include_directories(longfi
  PUBLIC ${PROJECT_SOURCE_DIR} ${lfc_SOURCE_DIR}/include
  )

install(TARGETS longfi
        DESTINATION lib
  )
