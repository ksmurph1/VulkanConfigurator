# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kerry/vulkan/src/folly

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kerry/vulkan/src/folly/build

# Include any dependencies generated for this target.
include CMakeFiles/folly_base.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/folly_base.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/folly_base.dir/flags.make

folly_base: CMakeFiles/folly_base.dir/build.make

.PHONY : folly_base

# Rule to build all files generated by this target.
CMakeFiles/folly_base.dir/build: folly_base

.PHONY : CMakeFiles/folly_base.dir/build

CMakeFiles/folly_base.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/folly_base.dir/cmake_clean.cmake
.PHONY : CMakeFiles/folly_base.dir/clean

CMakeFiles/folly_base.dir/depend:
	cd /home/kerry/vulkan/src/folly/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kerry/vulkan/src/folly /home/kerry/vulkan/src/folly /home/kerry/vulkan/src/folly/build /home/kerry/vulkan/src/folly/build /home/kerry/vulkan/src/folly/build/CMakeFiles/folly_base.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/folly_base.dir/depend
