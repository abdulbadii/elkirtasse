set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

qt_add_executable( qbooks WIN32 MACOSX_BUNDLE
    src/about.cpp		src/about.h
    src/animation.cpp		src/animation.h
    src/classepub.cpp		src/classepub.h
    src/classwizard.cpp      	src/classwizard.h
    src/databook.cpp         	src/databook.h
    src/dialog.cpp           	src/dialog.h
    src/dialogactions.cpp    	src/dialogactions.h
    src/dialogconfigbooks.cpp	src/dialogconfigbooks.h
    src/dialogconvert.cpp    	src/dialogconvert.h
    src/dialogfavo.cpp       	src/dialogfavo.h
    src/dialogfind.cpp       	src/dialogfind.h
    src/dialoggoto.cpp       	src/dialoggoto.h
    src/dialogimportdvd.cpp  	src/dialogimportdvd.h
    src/dialogoption.cpp     	src/dialogoption.h
    src/dialogupdat.cpp      	src/dialogupdat.h
    src/findbook.cpp         	src/findbook.h
    src/main.cpp             	src/booksInterface.h
    src/mainwindow.cpp       	src/mainwindow.h
    src/messages.cpp         	src/messages.h
    src/print.cpp            	src/print.h  
    src/toolrubon.cpp        	src/toolrubon.h
    ui/about.ui ui/dialog.ui ui/dialogactions.ui
    ui/dialogconfigbooks.ui ui/dialogconvert.ui ui/dialogfavo.ui ui/dialogfind.ui
    ui/dialoggoto.ui ui/dialogimportdvd.ui ui/dialogmdb.ui ui/dialogoption.ui
    ui/dialogupdat.ui ui/mainwindow.ui ui/mdbexport.ui ui/print.ui )

set_target_properties( qbooks	PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui"
    OUTPUT_NAME ${PROJECT_NAME} )

target_include_directories( qbooks PRIVATE build)

target_link_libraries( qbooks PRIVATE kirtasNETWORK kirtasROWAT kirtasCDROM
	PUBLIC
	Qt6::Core
	Qt6::Gui
	Qt6::PrintSupport
	Qt6::Widgets
	Qt6::Xml
)
set( image_files
    FIN_book.png		edit-clear.png		groupb.png
    Green.png			edit-delete.png		groupbak.png
    address-book-new.png	edit-find-user.png	groupbook.png
    arrow-left.png      	edit-rename.png		help-about.png
    bkg.png             	edit-select.png       	help-contents.png
    bookEdit.png        	edit_fahrass.png      	import-Shamila.png
    booki.png           	edit_fahrass_child.png	import-ShamilaCdrom.png
    bookmark-new.png    	exportBook.png        	import-cdrom.png
    bot.png             	fahrasa-exact.png     	importTxt.png
    bot_left.png        	findbook.png		insert-link.png
    bot_right.png       	go-back.png 		left.png
    btnfolder.png       	go-first.png		left3d.png
    colapse.png         	go-last.png 		list-add.png
    config-background.png	go-next.png 		listanim.png
    configure-toolbars.png	go-page.png 		mail-receive.png
    document-save.png   	gohome.png  		mask.png
    draw-text.png       	group2.png  		newBlanBook.png
    preferences-other.png	tab-new.png		top_right.png		view-fullscreen.png
    printer.png       		tar-gz.png  		undo-back.png		 whatsthis.png
    printerPreview.png		techkil.png		undo-next.png		 zoom-in.png
    quit.png          		telled.png  		view-books.png		zoom-out.png
    recent.png        		threeD.png		view-curan.png
    reflet.png        		titlekirtasse.png	view-fahrasse.png
    right.png         		top.png          	view-favo.png
    right3d.png       		top_left.png     	view-find.png )

list(	TRANSFORM image_files	PREPEND "ui/image/")

qt_add_resources( qbooks "images"
	BASE	"ui"
	PREFIX	"/images"
	FILES	${image_files} )

if( WIN32)
  target_sources( qbooks PUBLIC
    src/dialogmdb.cpp
    src/dialogmdb.h
    ui/dialogmdb.ui )
  target_link_libraries( qbooks PRIVATE Qt::Sql )
  set( CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}" CACHE PATH "" FORCE)
  install( TARGETS qbooks
      RUNTIME DESTINATION "."
      LIBRARY DESTINATION "." )
elseif( APPLE)
  message(STATUS "MacOS build is experimental, no file known yet")
  install( TARGETS qbooks BUNDLE DESTINATION .)
elseif( UNIX)
  target_sources( qbooks PUBLIC
    src/mdbexport.cpp
    src/mdbexport.h
    ui/mdbexport.ui )
  set( CMAKE_INSTALL_PREFIX "/usr" CACHE STRING "Install path prefix" FORCE)
  install( TARGETS	qbooks
	RUNTIME		DESTINATION ${CMAKE_INSTALL_BINDIR}
	COMPONENT	Runtime )
  install( DIRECTORY	../usr/share/			# trailing / in source name is crucial
	DESTINATION	${CMAKE_INSTALL_DATAROOTDIR}
	COMPONENT	Runtime )
  install( CODE "execute_process(COMMAND gtk-update-icon-cache ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor)")
endif()
