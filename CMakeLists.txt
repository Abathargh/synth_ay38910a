cmake_minimum_required(VERSION 3.22)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_GENERATOR "Unix Makefiles")
project(ay38910a_synth C)

# Default to Debug build
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# Set the MCU
if (NOT MCU)
  set(MCU "atmega644")
endif (NOT MCU)

# avrdude settings
if (NOT AVRDUDE_PRG_STR)
  set(AVRDUDE_PRG_STR atmelice)
endif (NOT AVRDUDE_PRG_STR)

# C related stuff
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)
set(GCC_FLAGS "-Wall -Wextra -DF_CPU=16000000 -mmcu=${MCU}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_FLAGS}")
add_compile_options(
	$<$<CONFIG:DEBUG>:-Og> 
	$<$<CONFIG:DEBUG>:-ggdb> 
	$<$<CONFIG:RELEASE>:-O2> 
)


file(GLOB_RECURSE SOURCES
	"main.c" 
	"delay/*.*" 
	"pinout/*.*" 
	"logger/*.*" 
	"Ay38910a/*.*" 
	"1602a_lcd/*.*" 
	"keyboard_ifc/*.*"
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE delay)
target_include_directories(${PROJECT_NAME}.elf PRIVATE pinout)
target_include_directories(${PROJECT_NAME}.elf PRIVATE logger)
target_include_directories(${PROJECT_NAME}.elf PRIVATE Ay38910a)
target_include_directories(${PROJECT_NAME}.elf PRIVATE 1602a_lcd)
target_include_directories(${PROJECT_NAME}.elf PRIVATE keyboard_ifc)

set_property(TARGET ${PROJECT_NAME}.elf
        APPEND
        PROPERTY ADDITIONAL_CLEAN_FILES ${PROJECT_NAME}.hex ${PROJECT_NAME}.bin
        )

add_custom_command(TARGET ${PROJECT_NAME}.elf
        POST_BUILD
        COMMAND avr-objcopy -j .text -j .data -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
        COMMAND avr-objcopy -j .text -j .data -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
        )

add_custom_target(flash
        COMMAND avrdude -c ${AVRDUDE_PRG_STR} -p ${MCU} -U flash:w:${PROJECT_NAME}.hex:i
        COMMENT "flashes the hex file onto the MCU"
        )

add_custom_target(flash-debug
        COMMAND avrdude -c ${AVRDUDE_PRG_STR} -p ${MCU} -U flash:w:${PROJECT_NAME}.elf:e
        COMMENT "flashes the elf file onto the MCU"
)
