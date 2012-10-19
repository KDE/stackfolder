project(stackfolder)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

find_library(konq_lib NAMES konq DOC "konq library")
find_path(konq_inc "konq_operations.h" DOC "Icnlude dir for konq")

if(NOT konq_lib)
    message(FATAL_ERROR "konq library isn't found")
endif()

if(NOT konq_inc)
    message(FATAL_ERROR "development files for konq library aren't found")
endif()

message(STATUS "Found konq library: ${konq_lib}")
message(STATUS "Found konq library include dir: ${konq_inc}")

set(QT_USE_QTMAIN TRUE)
set(QT_USE_QTDECLARATIVE TRUE)
find_package(Qt4 4.7 COMPONENTS QtCore QtGui QtDeclarative REQUIRED)
include(${QT_USE_FILE})

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
    ${konq_inc}
   )

set(stackfolder_SRCS
    stackfolder.cpp
    dirlister.cpp
    proxymodel.cpp
    imageprovider.cpp
    previewgenerator.cpp
    directory.cpp
    file.cpp
    viewer.cpp
    )


kde4_add_plugin(plasma_applet_stackfolder ${stackfolder_SRCS})
target_link_libraries(plasma_applet_stackfolder ${KDE4_PLASMA_LIBS} ${konq_lib}
                      ${KDE4_KIO_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_SOLID_LIBS}
                      ${X11_LIBRARIES} ${QT_LIBRARIES})

install(TARGETS plasma_applet_stackfolder DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-stackfolder.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(DIRECTORY package/ DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/stackfolder)
