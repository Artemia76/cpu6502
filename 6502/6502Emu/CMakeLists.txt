cmake_minimum_required(VERSION 3.13)

project( M6502Emu )

if(MSVC)
    add_compile_options(/MP)				#Use multiple processors when building
    add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
    add_compile_options(-W -Wall -Werror) #All Warnings, all warnings are errors
endif()

set  (M6502_SOURCES
    "src/main.cpp"
    "src/loop.cpp"
    "src/mainapp.cpp")
        
source_group("src" FILES ${M6502_SOURCES})
        
add_executable( M6502Emu ${M6502_SOURCES} )
add_dependencies( M6502Emu M6502Lib )
target_link_libraries(M6502Emu M6502Lib)
target_include_directories ( M6502Emu PUBLIC "${PROJECT_SOURCE_DIR}/include")
set_property(TARGET M6502Emu PROPERTY CXX_STANDARD 17)
set_property(TARGET M6502Emu PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET M6502Emu PROPERTY CXX_EXTENSIONS Off)