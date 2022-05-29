cmake_minimum_required(VERSION 3.22)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project(ay38910a_synth C)

# C related stuff
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER  avr-gcc)
set(GCC_FLAGS "-Wall -O1 -DF_CPU=16000000 -mmcu=atmega2560")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_FLAGS}")

# avrdude settings
set(AVRDUDE_PRG_STR stk500v2 -D -P /dev/ttyACM0 -b 115200)
set(MCU "atmega2560")

file(GLOB_RECURSE SOURCES "main.c" "1602a_lcd/*.*" "inc/*.*" "Ay38910a/*.*")
add_executable(ay38910a_synth.elf ${SOURCES})

target_include_directories(ay38910a_synth.elf PRIVATE Ay38910a)
target_include_directories(ay38910a_synth.elf PRIVATE 1602a_lcd)
target_include_directories(ay38910a_synth.elf PRIVATE inc)


add_custom_command(TARGET ${PROJECT_NAME}.elf
        POST_BUILD
        COMMAND rm -f ${PROJECT_NAME}.hex &&
            avr-objcopy -j .text -j .data -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
        COMMAND rm -f ${PROJECT_NAME}.bin &&
            avr-objcopy -j .text -j .data -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
        )

add_custom_target(flash
        COMMAND avrdude -c ${AVRDUDE_PRG_STR} -p ${MCU} -U flash:w:ay38910a_synth.hex:i
        COMMENT "flashes the hex file onto the MCU"
        )
