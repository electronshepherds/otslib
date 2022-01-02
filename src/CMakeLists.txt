cmake_minimum_required(VERSION 3.16)

project(otslib)

set(TARGET_NAME otslib)

include(FindPkgConfig)
pkg_check_modules(GATTLIB gattlib)
pkg_check_modules(BLUETOOTH bluez)

# Logging
set(OTSLIB_LOG_BACKEND syslog CACHE STRING "Logging backend: syslog, stdout (default: syslog)")
if (${OTSLIB_LOG_BACKEND} STREQUAL "syslog")
	set(LOG_BACKEND logging/syslog.c)
elseif(${OTSLIB_LOG_BACKEND} STREQUAL "stdout")
	set(LOG_BACKEND logging/stdout.c)
else()
	message(FATAL_ERROR "Uknown LOG_BACKEND: ${LOG_BACKEND}")
endif()
set(OTSLIB_LOG_LEVEL LOG_ERR CACHE STRING "Minimum logging level for otslib (LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR, LOG_WARNING, LOG_NOTICE, LOG_INFO)")

# Action Control Point
set(OTSLIB_OACP_NOTIFICATION_TIMEOUT_S 1 CACHE STRING "Object Action Control Point Notification Timeout (s)")

# List Control Point
set(OTSLIB_OLCP_NOTIFICATION_TIMEOUT_S 1 CACHE STRING "Object List Control Point Notification Timeout (s)")

add_library(
	${TARGET_NAME} SHARED
	${LOG_BACKEND}
	action.c
	internal.c
	list.c
	metadata.c
	otslib.c
)

target_compile_definitions(
	${TARGET_NAME} PRIVATE
	LOG_LEVEL=${OTSLIB_LOG_LEVEL}
	OACP_NOTIFICATION_TIMEOUT_S=${OTSLIB_OACP_NOTIFICATION_TIMEOUT_S}
	OLCP_NOTIFICATION_TIMEOUT_S=${OTSLIB_OLCP_NOTIFICATION_TIMEOUT_S}
)

target_include_directories(
	${TARGET_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/../include
	${CMAKE_CURRENT_LIST_DIR}
	${GATTLIB_INCLUDE_DIRS}
	${BLUETOOTH_INCLUDE_DIRS}
)

target_link_libraries(
	${TARGET_NAME}
	${GATTLIB_LIBRARIES}
	${BLUETOOTH_LIBRARIES}
)

target_link_directories(
	${TARGET_NAME} PRIVATE
	${GATTLIB_LIBRARY_DIRS}
	${BLUETOOTH_LIBRARY_DIRS}
)

target_compile_options(
	${TARGET_NAME} PRIVATE
	${ADDRESS_SANIITZER}
	-Wall
	-Werror
)

target_link_options(
	${TARGET_NAME} PRIVATE
	${ADDRESS_SANIITZER}
)

include(GNUInstallDirs)

install(
	TARGETS ${TARGET_NAME}
	LIBRARY
	DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
	DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
