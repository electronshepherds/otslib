cmake_minimum_required(VERSION 3.16)

project(otslib)

# Address Sanitizer
set(OTSLIB_ADDRESS_SANITIZER OFF CACHE BOOL "Enable address sanitizer.")
if (OTSLIB_ADDRESS_SANITIZER)
	set(ADDRESS_SANIITZER -fsanitize=address)
endif()

# Examples
set(OTSLIB_EXAMPLES OFF CACHE BOOL "Enable build and install of examples.")
if (OTSLIB_EXAMPLES)
	add_subdirectory(examples)
endif()

# Documentation
set(OTSLIB_BUILD_DOC OFF CACHE BOOL "Include documentation build.")
if (OTSLIB_BUILD_DOC)
	add_subdirectory(doc)
endif()

add_subdirectory(src)