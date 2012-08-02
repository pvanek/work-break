cmake_minimum_required( VERSION 2.6 )

project(work-break)


if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

if (NOT CMAKE_INSTALL_PREFIX)
    message(FATAL_ERROR "CMAKE_INSTALL_PREFIX is not set but it has to be. Something is wrong.")
endif ()

set(MAJOR_VERSION 1)
set(MINOR_VERSION 0)
set(PATCH_VERSION 0)
set(RAZOR_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
add_definitions(-DAPP_NAME=\"${PROJECT_NAME}\"
                -DAPP_VERSION=\"${RAZOR_VERSION}\")

add_definitions ( -Wall )

find_package ( Qt4 4.6.0 REQUIRED)
include ( ${QT_USE_FILE} )

if (QT_QTDBUS_FOUND)
    add_definitions(-DHAVE_QTDBUS)
endif (QT_QTDBUS_FOUND)

include_directories (
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)


set(SRC
   src/main.cpp
   src/workbreak.cpp
)

set(MOCS
   src/workbreak.h
)

set(ICONS
   src/icons/green.png
   src/icons/red.png
   src/icons/yellow.png
)

if (QT_QTDBUS_FOUND)
    qt4_add_dbus_interface(SRC src/dbus/org.freedesktop.Notifications.xml notifications_interface)
endif (QT_QTDBUS_FOUND)

qt4_wrap_cpp(MOCS_SRC ${MOCS})


add_executable(${PROJECT_NAME}
    ${SRC} ${MOCS_SRC})

target_link_libraries(${PROJECT_NAME} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
if (QT_QTDBUS_FOUND)
    target_link_libraries(${PROJECT_NAME} ${QT_QTDBUS_LIBRARY})
endif (QT_QTDBUS_FOUND)


# platform specific
if (APPLE)
    message(FATAL_ERROR "Building for apple is not implemented yet")
elseif (WIN32)
    message(FATAL_ERROR "Building for windows is not implemented yet")
elseif (UNIX)
    set(ICON_PREFIX ${CMAKE_INSTALL_PREFIX}/share/work-break/icons/)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    install(FILES ${ICONS} DESTINATION share/work-break/icons/)
else ()
    message(FATAL_ERROR "Unknown platform to build")
endif ()

message(STATUS "Icon prefix: ${ICON_PREFIX}")
add_definitions(-DICON_PREFIX=\"${ICON_PREFIX}\")


########## Add make dist target ################
SET(CPACK_PACKAGE_VERSION "${APP_VERSION}")
SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
SET(CPACK_IGNORE_FILES "/CVS/;/\\\\.git\;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;refresh-copyright-and-license.pl")
SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})
INCLUDE(CPack)
# simulate autotools' "make dist"
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
