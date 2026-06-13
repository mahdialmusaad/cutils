cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(cutils
	LANGUAGES C
	VERSION 2
	HOMEPAGE_URL "https://github.com/mahdialmusaad/cutils"
	DESCRIPTION "A portable C utilities library"
)

# ---------------------------- User options ----------------------------

option(CU_SETTING_FUNCS "Enable main functions" ON)

if(CU_SETTING_FUNCS)
	option(CU_SETTING_STRING_FUNCS "Build the string functions" ON)
	option(CU_SETTING_FILE_FUNCS "Build the file management functions" ON)
	option(CU_SETTING_RESOURCES_FUNCS "Build the hardware functions" ON)
	option(CU_SETTING_TIME_FUNCS "Build the time functions" ON)
	option(CU_SETTING_NETWORK_FUNCS "Build the networking functions" ON)
	option(CU_SETTING_THREAD_FUNCS "Build the threading functions" ON)

	option(CU_BUILD_MISC "Build the test and example programs" OFF)
endif()

option(CU_SETTING_QUIET "Do not warn about unavailable features" OFF)
option(CU_SETTING_FORCE_DEBUG "Assume compiling in debug mode" OFF)

option(CU_DEFAULT_CFLAGS "Use predefined compiler options" ON)

option(CU_INSTALL "Create installation target" OFF)

# ----------------------------------------------------------------------

add_library(cutils STATIC cutils.c)

if(MSVC AND CU_DEFAULT_CFLAGS)
	add_compile_options(/W4)
elseif(CU_DEFAULT_CFLAGS)
	if(CMAKE_C_FLAGS STREQUAL "")
		target_compile_options(cutils PUBLIC -Wall -Wextra -pedantic -fno-asynchronous-unwind-tables -fno-ident -ffast-math -fno-stack-protector)
	else()
		target_compile_options(cutils PUBLIC ${CMAKE_C_FLAGS})
	endif()

	if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
		target_compile_options(cutils PUBLIC -Oz)
	endif()
endif()

target_compile_definitions(cutils PRIVATE
	${CU_SETTING_FUNCS}
	${CU_SETTING_STRING_FUNCS}
	${CU_SETTING_FILE_FUNCS}
	${CU_SETTING_RESOURCES_FUNCS}
	${CU_SETTING_TIME_FUNCS}
	${CU_SETTING_NETWORK_FUNCS}
	${CU_SETTING_THREAD_FUNCS}
	${CU_SETTING_QUIET}
	${CU_SETTING_FORCE_DEBUG}
)

if(CU_INSTALL)
	install(TARGETS cutils ARCHIVE DESTINATION lib)
	install(FILES cutils.h DESTINATION include)

	set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
	set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
	set(CPACK_PACKAGE_VENDOR "Mahdi Almusaad")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
	set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
	set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
	set(CPACK_COMPRESSION_LEVEL 9)
	set(CPACK_THREADS 0)
	if(WIN32)
		set(CPACK_GENERATOR "ZIP")
	else()
		set(CPACK_GENERATOR "TGZ")
	endif()

	include(CPack)
endif()

if(CU_BUILD_MISC)
	get_target_property(CU_COMPILE_OPTIONS cutils COMPILE_OPTIONS)
	if (CU_COMPILE_OPTIONS STREQUAL CU_COMPILE_OPTIONS-NOTFOUND)
		set(CU_COMPILE_OPTIONS "")	
	endif()

	add_executable(test tests/main.c)
	target_link_libraries(test PRIVATE cutils)
	target_compile_options(test PRIVATE ${CU_COMPILE_OPTIONS})

	file(GLOB CU_EXAMPLES examples/*.c)

	foreach(CU_EXAMPLE_SOURCE ${CU_EXAMPLES})
		get_filename_component(CU_EXAMPLE_TARGET ${CU_EXAMPLE_SOURCE} NAME_WE)
		add_executable(${CU_EXAMPLE_TARGET} ${CU_EXAMPLE_SOURCE})
		target_link_libraries(${CU_EXAMPLE_TARGET} PRIVATE cutils)
		target_compile_options(${CU_EXAMPLE_TARGET} PRIVATE ${CU_COMPILE_OPTIONS})
	endforeach()
endif()
