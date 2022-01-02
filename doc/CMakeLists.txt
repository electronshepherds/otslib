find_package(Doxygen
	OPTIONAL_COMPONENTS dot mscgen dia
)

set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_GENERATE_HTML NO)
doxygen_add_docs(
	doxygen
	../include
)

find_program(SPHINX_EXECUTABLE NAMES sphinx-build
	HINTS
	$ENV{SPHINX_DIR}
	PATH_SUFFIXES bin
	DOC "Sphinx documentation generator"
)
mark_as_advanced(SPHINX_EXECUTABLE)

# This is the directory where github looks for the pages output
set(SPHINX_HTML_DIR "${PROJECT_SOURCE_DIR}/docs")

set(DOXYGEN_XML_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/xml")
set(SPHINX_HTML_STATIC_PATH "${CMAKE_CURRENT_LIST_DIR}")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in"
	"${CMAKE_CURRENT_BINARY_DIR}/conf.py"
	@ONLY)

add_custom_target(
	doc
	${SPHINX_EXECUTABLE}
	-q -b html
	-c ${CMAKE_CURRENT_BINARY_DIR}
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${SPHINX_HTML_DIR}"
	COMMENT "Building HTML documentation with Sphinx"
)
add_dependencies(doc doxygen)
