cmake_minimum_required(VERSION 3.16)

set(INSTALL_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/otslib/action.h
	${CMAKE_CURRENT_LIST_DIR}/otslib/directory.h
	${CMAKE_CURRENT_LIST_DIR}/otslib/list.h
	${CMAKE_CURRENT_LIST_DIR}/otslib/metadata.h
	${CMAKE_CURRENT_LIST_DIR}/otslib/otslib.h
)

set_target_properties(
	${TARGET_NAME} PROPERTIES
	PUBLIC_HEADER "${INSTALL_HEADERS}"
)
