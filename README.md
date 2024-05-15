# Dealership Simulation System

## Overview
This repository contains a C++ implementation for simulating a dealership system. The system includes classes for generating random numbers, managing car data, and handling a database of cars.

## Files
- `dealer.h` and `dealer.cpp`: These files contain the implementation of the `Random` and `CarDB` classes, which are essential for generating random numbers and managing car data.
- `mytest.cpp`: This file contains the main function for testing the functionality of the dealership simulation system.
- `Makefile`: Contains instructions for compiling the project.

## How to Use
1. Include `dealer.h` in your C++ project.
2. Create instances of the `Random` and `CarDB` classes to generate random numbers and manage car data, respectively.
3. Use the `Random` class to generate random numbers with various distributions, such as uniform integer, uniform real, and normal.
4. Utilize the `CarDB` class to manage the database of cars, including insertion, retrieval, and rehashing operations.
5. Compile the project using the provided Makefile instructions.

## Compilation
To compile the project, you can use the provided Makefile. Use the following commands:
- `make p`: Compiles `mytest.cpp` with `dealer.o` to create an executable named `proj4`.
- `make d`: Compiles `driver.cpp` with `dealer.o` to create an executable named `proj4`.
- `make b`: Runs `gdb` for debugging purposes.
- `make v`: Runs `valgrind` to check for memory leaks.
- `make r`: Runs the executable `proj4`.

## Cleaning Up
To clean up object files and executables, you can use:
- `make clean`: Removes object files and temporary files.
