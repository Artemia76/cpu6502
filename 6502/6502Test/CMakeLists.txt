cmake_minimum_required(VERSION 3.13)

project( M6502Test )

set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DM6502_DEBUG" )	#so we can add the DEBUG preprocessor define and other flags to stay in debug mode - see https://cmake.org/Wiki/CMake_Useful_Variables#Compilers_and_Tools
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DM6502_DEBUG" )

if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
    add_compile_options(-W -Wall -Werror) #All Warnings, all warnings are errors
endif()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# source for the test executable
set  (M6502_SOURCES
        "src/main_6502.cpp"
        "src/6502LoadRegisterTests.cpp"
        "src/6502StoreRegisterTests.cpp"
        "src/6502JumpsAndCallsTests.cpp"
        "src/6502StackOperationsTests.cpp"
        "src/6502AND_EOR_ORA_Tests.cpp"
        "src/6502LoadPrgTests.cpp"
        "src/6502TransferRegisterTests.cpp"
        "src/6502IncrementDecrementTests.cpp"
        "src/6502BranchTests.cpp"
        "src/6502StatusFlagChangeTests.cpp"
        "src/6502AddWithCarryTests.cpp"
        "src/6502CompareRegisterTests.cpp"
        "src/6502ShiftsTests.cpp"
        "src/6502SystemFunctionsTests.cpp"
)
        
source_group("src" FILES ${M6502_SOURCES})
        
add_executable( M6502Test ${M6502_SOURCES} 	)
add_dependencies( M6502Test M6502Lib )
target_link_libraries(M6502Test gtest)
target_link_libraries(M6502Test M6502Lib)

set_property(TARGET M6502Test PROPERTY CXX_STANDARD 17)
set_property(TARGET M6502Test PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET M6502Test PROPERTY CXX_EXTENSIONS Off)