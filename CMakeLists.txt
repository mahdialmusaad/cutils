cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(cutils
	LANGUAGES C
	VERSION 2
	HOMEPAGE_URL "https://github.com/mahdialmusaad/cutils"
	DESCRIPTION "A portable C utilities library"
)

# ---------------------------- User options ----------------------------

option(CU_SETTING_FILE_FUNCS, "Build the FILE functions" ON)
option(CU_SETTING_RAND_FUNCS, "Build the RAND functions" ON)
option(CU_SETTING_RESOURCES_FUNCS, "Build the RESOURCES functions" ON)
option(CU_SETTING_TIME_FUNCS, "Build the TIME functions" ON)
option(CU_SETTING_NETWORK_FUNCS, "Build the NETWORK functions" ON)
option(CU_SETTING_THREAD_FUNCS, "Build the THREAD functions" ON)
option(CU_SETTING_ALLOC_FUNCS, "Build the ALLOC functions" ON)

option(CU_BUILD_MISC "Build the test and example programs" ON)
option(CU_INSTALL "Create installation target" OFF)

# ----------------------------------------------------------------------

add_library(cutils STATIC cutils.c)

if (MSVC)
	add_compile_options(/W4)
else()
	if (CMAKE_C_FLAGS STREQUAL "")
		target_compile_options(cutils PUBLIC -Wall -Wextra -pedantic -fno-asynchronous-unwind-tables -fno-ident -ffast-math -fno-stack-protector)
	else()
		target_compile_options(cutils PUBLIC ${CMAKE_C_FLAGS})
	endif()

	if (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
		target_compile_options(cutils PUBLIC -Oz)
	endif()
endif()

set_target_properties(cutils PROPERTIES C_STANDARD 90)
set_target_properties(cutils PROPERTIES C_EXTENSIONS OFF)
target_compile_definitions(cutils PRIVATE
	${CU_SETTING_FILE_FUNCS}
	${CU_SETTING_RAND_FUNCS}
	${CU_SETTING_RESOURCES_FUNCS}
	${CU_SETTING_TIME_FUNCS}
	${CU_SETTING_NETWORK_FUNCS}
	${CU_SETTING_THREAD_FUNCS}
	${CU_SETTING_ALLOC_FUNCS}
)

if (CU_INSTALL)
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

if (CU_BUILD_MISC)
	add_executable(cutils_test test.c)
	add_executable(cutils_network_test net_test.c)
	target_link_libraries(cutils_test PRIVATE cutils)
	target_link_libraries(cutils_network_test PRIVATE cutils)
endif()
