set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

###########################################################################
# Toolchain executables                                                   #
###########################################################################
find_program(TOOLCHAIN_CC      "xtensa-esp32-elf-gcc")
find_program(TOOLCHAIN_CXX     "xtensa-esp32-elf-g++")
find_program(TOOLCHAIN_OBJCOPY "xtensa-esp32-elf-objcopy")
find_program(TOOLCHAIN_OBJDUMP "xtensa-esp32-elf-objdump")
find_program(TOOLCHAIN_SIZE    "xtensa-esp32-elf-size")
find_program(TOOLCHAIN_LD      "xtensa-esp32-elf-ld")

set(CMAKE_C_COMPILER   ${TOOLCHAIN_CC})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_CXX})
set(CMAKE_SYSTEM_NAME Generic)

# All
set(CMAKE_C_FLAGS "-ffunction-sections -fdata-sections -mlongcalls")
set(CMAKE_CXX_FLAGS "-ffunction-sections -fdata-sections -mlongcalls")

# Debug
set(CMAKE_C_FLAGS_DEBUG "-ggdb3 -Og")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -Og")

# Release
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -Os -flto")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Os -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Os -flto -Wno-lto-type-mismatch")
