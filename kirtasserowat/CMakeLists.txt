set(CMAKE_AUTOMOC ON)
qt_add_library( kirtasROWAT STATIC
    rowat.cpp
    rowat.h
)

target_include_directories( kirtasROWAT PRIVATE  ../qbooks )

target_link_libraries( kirtasROWAT PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Xml
)
#install( TARGETS kirtasROWAT
    #LIBRARY	DESTINATION	${CMAKE_INSTALL_LIBDIR}
    #FRAMEWORK	DESTINATION	${CMAKE_INSTALL_LIBDIR}
    #RUNTIME	DESTINATION	${CMAKE_INSTALL_BINDIR}
#)
