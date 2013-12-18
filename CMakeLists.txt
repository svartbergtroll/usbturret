CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(usbturret)

INCLUDE(FindPkgConfig)

IF(NOT PKG_CONFIG_FOUND)
	MESSAGE(FATAL_ERROR "You need to have pkg-config installed")
ENDIF()

PKG_CHECK_MODULES(USB libusb-1.0)

IF(NOT USB_FOUND)
	MESSAGE(FATAL_ERROR "You need to install libusb-1.0")
ENDIF()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -DDEBUGMODE -g")
INCLUDE_DIRECTORIES("include" ${USB_INCLUDE_DIRS})
LINK_DIRECTORIES(${USB_LIBRARY_DIRS})

FILE(
	GLOB_RECURSE
	sources
	src/*.c
	src/*.cpp
	include/*.h
)

ADD_EXECUTABLE(
	usbturret
	${sources}
)

TARGET_LINK_LIBRARIES(
	usbturret
	${USB_LIBRARIES}
)
