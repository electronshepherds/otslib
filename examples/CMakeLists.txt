cmake_minimum_required(VERSION 3.16)

function(declare_example NAME)
	set(TARGET_NAME ${NAME})
	add_executable(
		${TARGET_NAME}
		common.c
		"${ARGN}"
	)
	target_link_libraries(
		${TARGET_NAME}
		otslib
	)
	target_include_directories(
		${TARGET_NAME} PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/../include/
	)
	target_link_options(
		${TARGET_NAME} PRIVATE
		${ADDRESS_SANIITZER}
	)
	target_compile_options(
		${TARGET_NAME} PRIVATE
		${ADDRESS_SANIITZER}
		-Wall
		-Werror
	)
	install(
		TARGETS ${TARGET_NAME}
		RUNTIME
		DESTINATION share/otslib
	)
endfunction()

declare_example(
	features
	features.c
)

declare_example(
	create
	create.c
)

declare_example(
	metadata
	metadata.c
)

declare_example(
	read
	read.c
	hexdump.c
)

declare_example(
	write
	write.c
	hexdump.c
)

declare_example(
	directory
	directory.c
	hexdump.c
)
