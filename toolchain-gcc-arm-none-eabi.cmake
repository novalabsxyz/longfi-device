set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

###########################################################################
# Toolchain executables                                                   #
###########################################################################
find_program(TOOLCHAIN_CC      "arm-none-eabi-gcc")
find_program(TOOLCHAIN_CXX     "arm-none-eabi-g++")
find_program(TOOLCHAIN_OBJCOPY "arm-none-eabi-objcopy")
find_program(TOOLCHAIN_OBJDUMP "arm-none-eabi-objdump")
find_program(TOOLCHAIN_SIZE    "arm-none-eabi-size")
find_program(TOOLCHAIN_LD      "arm-none-eabi-ld")

set(CMAKE_C_COMPILER   ${TOOLCHAIN_CC})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_CXX})
set(CMAKE_SYSTEM_NAME Generic)

# All
set(CMAKE_C_FLAGS "-ffunction-sections -fdata-sections -mthumb -march=armv6s-m")
set(CMAKE_CXX_FLAGS "-ffunction-sections -fdata-sections -mthumb -march=armv6s-m")

# Debug
set(CMAKE_C_FLAGS_DEBUG "-ggdb3 -Og")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -Og")

# Release
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -Os")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Os")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Os")
