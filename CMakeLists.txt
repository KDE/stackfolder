project(stackfolder)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)


set(QT_USE_QTMAIN TRUE)
set(QT_USE_QTDECLARATIVE TRUE)
find_package(Qt4 4.7 COMPONENTS QtCore QtGui QtDeclarative REQUIRED)
include(${QT_USE_FILE})

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

set(stackfolder_SRCS
    stackfolder.cpp
    dialog.cpp
    dirlister.cpp
    proxymodel.cpp
    imageprovider.cpp
    previewgenerator.cpp
    directory.cpp
    file.cpp
    iconwidget.cpp
    viewer.cpp
    )


kde4_add_plugin(plasma_applet_stackfolder ${stackfolder_SRCS})
target_link_libraries(plasma_applet_stackfolder ${KDE4_PLASMA_LIBS}
                      ${KDE4_KIO_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_SOLID_LIBS}
                      ${X11_LIBRARIES} ${QT_LIBRARIES})

install(TARGETS plasma_applet_stackfolder DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-stackfolder.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(DIRECTORY package/ DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/stackfolder)
