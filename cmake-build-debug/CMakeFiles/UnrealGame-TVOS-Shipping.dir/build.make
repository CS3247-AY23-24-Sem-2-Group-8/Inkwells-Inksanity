# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/aarch64/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/aarch64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/lucas/Desktop/NUS Com Science/Y3S2/CS3247/ScrabbleGame"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/lucas/Desktop/NUS Com Science/Y3S2/CS3247/ScrabbleGame/cmake-build-debug"

# Utility rule file for UnrealGame-TVOS-Shipping.

# Include any custom commands dependencies for this target.
include CMakeFiles/UnrealGame-TVOS-Shipping.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/UnrealGame-TVOS-Shipping.dir/progress.make

CMakeFiles/UnrealGame-TVOS-Shipping:
	cd "/Users/Shared/Epic Games/UE_5.3" && bash "/Users/Shared/Epic Games/UE_5.3/Engine/Build/BatchFiles/Mac/Build.sh" UnrealGame TVOS Shipping "-project=/Users/lucas/Desktop/NUS Com Science/Y3S2/CS3247/ScrabbleGame/ScrabbleGame.uproject" -game -progress

UnrealGame-TVOS-Shipping: CMakeFiles/UnrealGame-TVOS-Shipping
UnrealGame-TVOS-Shipping: CMakeFiles/UnrealGame-TVOS-Shipping.dir/build.make
.PHONY : UnrealGame-TVOS-Shipping

# Rule to build all files generated by this target.
CMakeFiles/UnrealGame-TVOS-Shipping.dir/build: UnrealGame-TVOS-Shipping
.PHONY : CMakeFiles/UnrealGame-TVOS-Shipping.dir/build

CMakeFiles/UnrealGame-TVOS-Shipping.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/UnrealGame-TVOS-Shipping.dir/cmake_clean.cmake
.PHONY : CMakeFiles/UnrealGame-TVOS-Shipping.dir/clean

CMakeFiles/UnrealGame-TVOS-Shipping.dir/depend:
	cd "/Users/lucas/Desktop/NUS Com Science/Y3S2/CS3247/ScrabbleGame/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/lucas/Desktop/NUS Com Science/Y3S2/CS3247/ScrabbleGame" "/Users/lucas/Desktop/NUS Com Science/Y3S2/CS3247/ScrabbleGame" "/Users/lucas/Desktop/NUS Com Science/Y3S2/CS3247/ScrabbleGame/cmake-build-debug" "/Users/lucas/Desktop/NUS Com Science/Y3S2/CS3247/ScrabbleGame/cmake-build-debug" "/Users/lucas/Desktop/NUS Com Science/Y3S2/CS3247/ScrabbleGame/cmake-build-debug/CMakeFiles/UnrealGame-TVOS-Shipping.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/UnrealGame-TVOS-Shipping.dir/depend

