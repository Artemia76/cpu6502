cmake_minimum_required(VERSION 3.13)

project( M6502Lib )

if(MSVC)
	add_compile_options(/MP)				#Use multiple processors when building
	add_compile_options(/W4 /wd4201 /WX)	#Warning level 4, all warnings are errors
else()
	add_compile_options(-W -Wall -Werror) #All Warnings, all warnings are errors
endif()

set  (M6502_SOURCES
    "src/m6502/System/Mem.cpp"
	"src/m6502/System/Cpu.cpp")
		
source_group("src" FILES ${M6502_SOURCES})
		
add_library( M6502Lib ${M6502_SOURCES} )

target_include_directories ( M6502Lib PUBLIC "${PROJECT_SOURCE_DIR}/include")

#set_target_properties(M6502Lib PROPERTIES FOLDER "M6502Lib")