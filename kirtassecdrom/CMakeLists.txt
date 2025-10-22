set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
qt_add_library( kirtasCDROM STATIC
  dialogcdrom.cpp
  dialogcdrom.ui
  cdromshamila.cpp
)

target_include_directories( kirtasCDROM PRIVATE ../qbooks)

target_link_libraries( kirtasCDROM PUBLIC
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    Qt6::Xml
)
#install( TARGETS kirtasCDROM
    #LIBRARY	DESTINATION	${CMAKE_INSTALL_LIBDIR}
    #FRAMEWORK	DESTINATION	${CMAKE_INSTALL_LIBDIR}
    #RUNTIME	DESTINATION	${CMAKE_INSTALL_BINDIR}
#)
