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
set(APP_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
add_definitions(-DAPP_NAME=\"${PROJECT_NAME}\"
                -DAPP_VERSION=\"${APP_VERSION}\")

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
if (APPLE)
	SET (SRC ${SRC} macosx/${PROJECT_NAME}.icns)
endif (APPLE)

set(MOCS
    src/workbreak.h
)

set(ICONS
    src/icons/green.png
    src/icons/red.png
    src/icons/yellow.png
)

set(DOCS
    docs/index_en.html.in
)

    
if (QT_QTDBUS_FOUND)
    qt4_add_dbus_interface(SRC src/dbus/org.freedesktop.Notifications.xml notifications_interface)
endif (QT_QTDBUS_FOUND)

qt4_wrap_cpp(MOCS_SRC ${MOCS})


add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
    ${SRC} ${MOCS_SRC})

target_link_libraries(${PROJECT_NAME} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
if (QT_QTDBUS_FOUND)
    target_link_libraries(${PROJECT_NAME} ${QT_QTDBUS_LIBRARY})
endif (QT_QTDBUS_FOUND)


# generate documantation form *.in sources
foreach (i ${DOCS})
    get_filename_component(htmlfile ${i} NAME_WE)
    message(STATUS "  generating docs for: ${i} -> ${htmlfile}.html")
    configure_file(${i} ${CMAKE_CURRENT_BINARY_DIR}/${htmlfile}.html)
    set (DOCS_HTML ${DOCS_HTML} ${CMAKE_CURRENT_BINARY_DIR}/${htmlfile}.html)
endforeach()


# platform specific
if (APPLE)
    # mac's bundle install
    SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/macosx/Info.plist.in")
  	SET(MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME}.icns)
   	SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${QSHARE_VERSION}")
   	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "cz.yarpen.${PROJECT_NAME}")
   	SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${APP_VERSION}")
   	SET(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
   	SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${APP_VERSION}")
   	SET(MACOSX_BUNDLE_BUNDLE_VERSION "${APP_VERSION}")
   	SET(MACOSX_BUNDLE_COPYRIGHT "(c) Petr Vanek; petr@yarpen.cz")
   	SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/macosx/${PROJECT_NAME}.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    INSTALL(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})
    INSTALL(FILES ${ICONS} DESTINATION ${PROJECT_NAME}.app/Contents/Resources/icons)
    INSTALL(FILES ${DOCS_HTML} DESTINATION ${PROJECT_NAME}.app/Contents/Resources)
    set(FILE_PREFIX "/../Resources/")

    # create a "transportable" bundle - all libs into the bundle: "make bundle" after make install
    configure_file(macosx/bundle.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/bundle.cmake @ONLY)
    add_custom_target(bundle ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/bundle.cmake)
elseif (WIN32)
    message(FATAL_ERROR "Building for windows is not implemented yet")
elseif (UNIX)
    set(FILE_PREFIX ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    install(FILES ${ICONS} DESTINATION share/${PROJECT_NAME}/icons/)
    install(FILES ${PROJECT_NAME}.desktop DESTINATION share/applications/)
    install(FILES ${PROJECT_NAME}.png DESTINATION share/pixmaps/)
    install(FILES ${DOCS_HTML} DESTINATION share/${PROJECT_NAME}/)
else ()
    message(FATAL_ERROR "Unknown platform to build")
endif ()

message(STATUS "File prefix: ${FILE_PREFIX}")
add_definitions(-DFILE_PREFIX=\"${FILE_PREFIX}\")


########## Add make dist target ################
SET(CPACK_PACKAGE_VERSION "${APP_VERSION}")
SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
SET(CPACK_IGNORE_FILES "/CVS/;/\\\\.git\;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;refresh-copyright-and-license.pl")
SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})
INCLUDE(CPack)
# simulate autotools' "make dist"
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

