# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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

# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/fengxu/thread/mythreadPoll

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fengxu/thread/mythreadPoll/build

# Include any dependencies generated for this target.
include CMakeFiles/thread_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/thread_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/thread_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/thread_test.dir/flags.make

CMakeFiles/thread_test.dir/test/threadtest.cpp.o: CMakeFiles/thread_test.dir/flags.make
CMakeFiles/thread_test.dir/test/threadtest.cpp.o: ../test/threadtest.cpp
CMakeFiles/thread_test.dir/test/threadtest.cpp.o: CMakeFiles/thread_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fengxu/thread/mythreadPoll/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/thread_test.dir/test/threadtest.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/thread_test.dir/test/threadtest.cpp.o -MF CMakeFiles/thread_test.dir/test/threadtest.cpp.o.d -o CMakeFiles/thread_test.dir/test/threadtest.cpp.o -c /home/fengxu/thread/mythreadPoll/test/threadtest.cpp

CMakeFiles/thread_test.dir/test/threadtest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/thread_test.dir/test/threadtest.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fengxu/thread/mythreadPoll/test/threadtest.cpp > CMakeFiles/thread_test.dir/test/threadtest.cpp.i

CMakeFiles/thread_test.dir/test/threadtest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/thread_test.dir/test/threadtest.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fengxu/thread/mythreadPoll/test/threadtest.cpp -o CMakeFiles/thread_test.dir/test/threadtest.cpp.s

# Object files for target thread_test
thread_test_OBJECTS = \
"CMakeFiles/thread_test.dir/test/threadtest.cpp.o"

# External object files for target thread_test
thread_test_EXTERNAL_OBJECTS =

../bin/thread_test: CMakeFiles/thread_test.dir/test/threadtest.cpp.o
../bin/thread_test: CMakeFiles/thread_test.dir/build.make
../bin/thread_test: ../lib/libini.so
../bin/thread_test: ../lib/libthreadpool.so
../bin/thread_test: CMakeFiles/thread_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fengxu/thread/mythreadPoll/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/thread_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/thread_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/thread_test.dir/build: ../bin/thread_test
.PHONY : CMakeFiles/thread_test.dir/build

CMakeFiles/thread_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/thread_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/thread_test.dir/clean

CMakeFiles/thread_test.dir/depend:
	cd /home/fengxu/thread/mythreadPoll/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fengxu/thread/mythreadPoll /home/fengxu/thread/mythreadPoll /home/fengxu/thread/mythreadPoll/build /home/fengxu/thread/mythreadPoll/build /home/fengxu/thread/mythreadPoll/build/CMakeFiles/thread_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/thread_test.dir/depend
