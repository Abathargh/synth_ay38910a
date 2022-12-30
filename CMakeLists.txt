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
if (${MCU} STREQUAL "atmega644")
	set(AVRDUDE_PRG_STR atmelice)
elseif(${MCU} STREQUAL "atmega2560")
  set(AVRDUDE_PRG_STR stk500v2 -D -b 115200 -P /dev/ttyACM0)
else()
	message(ERROR "unsupported chip")
endif()

# C related stuff
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)
set(GCC_FLAGS "-Wall -Wextra -Werror -DF_CPU=16000000 -mmcu=${MCU}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_FLAGS}")
add_compile_options(
	$<$<CONFIG:DEBUG>:-Og> 
	$<$<CONFIG:DEBUG>:-ggdb> 
	$<$<CONFIG:RELEASE>:-O2> 
)

file(GLOB_RECURSE SOURCES
	"main.c"
	"src/*.*"
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE inc)

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

add_custom_target(docs
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND doxygen
	COMMAND mkdir -p docs && mv html/* docs/ && rm -r html
	COMMENT "generates documentation from the comments using doxygen"
)

add_custom_target(clean-docs
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND rm -rf docs
	COMMENT "cleans up the generated documentation"
)
