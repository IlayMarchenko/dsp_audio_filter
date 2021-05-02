cmake_minimum_required(VERSION 3.17)
project(audio C)

set(CMAKE_C_STANDARD 11)

add_executable(audio main.c plot/pbPlots.c plot/pbPlots.h plot/supportLib.c plot/supportLib.h filter.c noise_check.c audio.h graphics.c)