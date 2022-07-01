cmake_minimum_required(VERSION 3.22)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_GENERATOR "Unix Makefiles")
project(ay38910a_synth C)

# Config settings
if(EXISTS ${PROJECT_SOURCE_DIR}/conf.json)
    file(READ conf.json PROJECT_CONF)
    string(JSON SERIAL_PORT GET ${PROJECT_CONF} port)
    if(SERIAL_PORT STREQUAL "")
        message(FATAL_ERROR "Empty serial port in conf.json")
    else()
        message("Flashing will happen on serial port: ${SERIAL_PORT}")
    endif()
else()
    file(TOUCH conf.json)
    file(WRITE conf.json "{\n\t\"port\": \"\"\n}")
    message(FATAL_ERROR "conf.json not found: an empty one was generated. "
            "Set up the serial port and re-initialize the Cmake project")
endif()

# C related stuff
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)
set(MCU "atmega2560")
set(GCC_FLAGS "-Wall -Wextra -Og -ggdb -DF_CPU=16000000 -mmcu=${MCU}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_FLAGS}")

# avrdude settings
set(AVRDUDE_PRG_STR stk500v2 -D -b 115200 -P ${SERIAL_PORT})

file(GLOB_RECURSE SOURCES "main.c" "1602a_lcd/*.*" "inc/*.*" "Ay38910a/*.*" "logger/*.*")
add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE Ay38910a)
target_include_directories(${PROJECT_NAME}.elf PRIVATE 1602a_lcd)
target_include_directories(${PROJECT_NAME}.elf PRIVATE logger)
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

