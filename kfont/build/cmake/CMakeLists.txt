PROJECT(kfont)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.6)

IF(COMMAND cmake_policy)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

SET(KFONT_MAJOR_VERSION 1)
SET(KFONT_MINOR_VERSION 0)
SET(KFONT_PATCH_VERSION 0)
SET(KFONT_VERSION ${KFONT_MAJOR_VERSION}.${KFONT_MINOR_VERSION}.${KFONT_PATCH_VERSION})

SET(KFONT_PROJECT_DIR "${CMAKE_HOME_DIRECTORY}/../..")
SET(KLAYGE_ROOT_DIR "${KFONT_PROJECT_DIR}/..")

INCLUDE(${KLAYGE_ROOT_DIR}/cmake/Common.cmake)
INCLUDE(${KLAYGE_ROOT_DIR}/cmake/Platform.cmake)
INCLUDE(${KLAYGE_ROOT_DIR}/cmake/Compiler.cmake)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

SET(KFONT_OUTPUT_DIR "${KFONT_PROJECT_DIR}/lib/${KLAYGE_PLATFORM_NAME}" )
SET(KLAYGE_BIN_DIR "${KFONT_PROJECT_DIR}/../KlayGE/bin/${KLAYGE_PLATFORM_NAME}" )


# The project settings

SET(LIB_NAME kfont)

SET(KFONT_SOURCE_FILES
	${KFONT_PROJECT_DIR}/src/kfont.cpp
)
SET(KFONT_HEADER_FILES
	${KFONT_PROJECT_DIR}/include/kfont/kfont.hpp
)
SOURCE_GROUP("Source Files" FILES ${KFONT_SOURCE_FILES})
SOURCE_GROUP("Header Files" FILES ${KFONT_HEADER_FILES})

ADD_DEFINITIONS(-DKFONT_SOURCE)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${KFONT_PROJECT_DIR}/../External/7z)
INCLUDE_DIRECTORIES(${KFONT_PROJECT_DIR}/../KFL/include)
INCLUDE_DIRECTORIES(${KFONT_PROJECT_DIR}/include)
LINK_DIRECTORIES(${Boost_LIBRARY_DIR})
LINK_DIRECTORIES(${KFONT_PROJECT_DIR}/../KFL/lib/${KLAYGE_PLATFORM_NAME})
ADD_LIBRARY(${LIB_NAME} ${KLAYGE_PREFERRED_LIB_TYPE}
	${KFONT_SOURCE_FILES} ${KFONT_HEADER_FILES}
)

SET(LIB_OUTPUT_NAME ${LIB_NAME}${KLAYGE_OUTPUT_SUFFIX})

IF(MSVC)
	SET(EXTRA_LINKED_LIBRARIES "")
ELSE()
	SET(EXTRA_LINKED_LIBRARIES 
		debug KFL${KLAYGE_OUTPUT_SUFFIX}_d optimized KFL${KLAYGE_OUTPUT_SUFFIX}
		${Boost_SYSTEM_LIBRARY})
ENDIF()

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${KFONT_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${KFONT_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${KFONT_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${KFONT_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${KFONT_OUTPUT_DIR}
	PROJECT_LABEL ${LIB_NAME}
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
	OUTPUT_NAME ${LIB_OUTPUT_NAME}
)
IF(KLAYGE_PLATFORM_WINDOWS_RUNTIME)
	SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES VS_WINRT_EXTENSIONS TRUE)
ENDIF()
IF(KLAYGE_PLATFORM_DARWIN OR KLAYGE_PLATFORM_IOS OR KLAYGE_PLATFORM_LINUX)
	SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${KFONT_OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY_DEBUG ${KFONT_OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY_RELEASE ${KFONT_OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${KFONT_OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${KFONT_OUTPUT_DIR})
ENDIF()

TARGET_LINK_LIBRARIES(${LIB_NAME}
	${EXTRA_LINKED_LIBRARIES})

IF(KLAYGE_PREFERRED_LIB_TYPE STREQUAL "SHARED")
	ADD_POST_BUILD(${LIB_NAME} "")

	INSTALL(TARGETS ${LIB_NAME}
		RUNTIME DESTINATION ${KLAYGE_BIN_DIR}
		LIBRARY DESTINATION ${KLAYGE_BIN_DIR}
		ARCHIVE DESTINATION ${KFONT_OUTPUT_DIR}
	)
ENDIF()
