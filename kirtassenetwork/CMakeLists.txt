set(CMAKE_AUTOMOC ON)
qt_add_library( kirtasNETWORK STATIC
    dialognet.cpp
    dialognet.h
    dialognet.ui
    networkplugin.cpp
    networkplugin.h
)

target_include_directories( kirtasNETWORK PRIVATE ../qbooks )

target_link_libraries( kirtasNETWORK PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Widgets
    Qt6::Xml
)
#install( TARGETS kirtasNETWORK
    #LIBRARY	DESTINATION	${CMAKE_INSTALL_LIBDIR}
    #FRAMEWORK	DESTINATION	${CMAKE_INSTALL_LIBDIR}
    #RUNTIME	DESTINATION	${CMAKE_INSTALL_BINDIR}
#)
