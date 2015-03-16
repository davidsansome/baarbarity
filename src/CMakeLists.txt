# Source files
SET(BAARBARITY-SOURCES
	main.cpp
	gameengine.cpp
	settings.cpp
	worldobject.cpp
	sheep.cpp
	terrain.cpp
	modelloader.cpp
	milkshapemodel.cpp
	gamestate.cpp
	gluimanager.cpp
	welshman.cpp
	utilities.cpp
	player.cpp
	facebookrequest.cpp
	pools.cpp
	building.cpp
	
	actions/action.cpp
	actions/moveaction.cpp
	actions/eatgrassaction.cpp
	actions/playermoveaction.cpp
	actions/constructionaction.cpp
	
	scripting/scriptmanager.cpp
	scripting/ability.cpp
	scripting/requirement.cpp
	scripting/objecttype.cpp
	scripting/vec2prototype.cpp
	scripting/playertype.cpp
	scripting/prototypestore.cpp
	scripting/buildingtype.cpp
	scripting/colorprototype.cpp
	scripting/engineprototype.cpp
	scripting/billboard.cpp
	scripting/timer.cpp
	
	gui/menuhud.cpp
	gui/shadereditor.cpp
	gui/optionsdialog.cpp
	gui/gamemenudialog.cpp
	gui/gamehud.cpp
	gui/progressdialog.cpp
	gui/playeroptionsdialog.cpp
	gui/facebooklogindialog.cpp
)

# Header files that have Q_OBJECT in
SET(BAARBARITY-MOC-HEADERS
	gameengine.h
	worldobject.h
	gluimanager.h
	terrain.h
	gamestate.h
	settings.h
	facebookrequest.h
	pools.h
	player.h
	sheep.h
	welshman.h
	building.h
	
	actions/action.h
	actions/eatgrassaction.h
	actions/playermoveaction.h
	actions/constructionaction.h
	
	scripting/ability.h
	scripting/requirement.h
	scripting/objecttype.h
	scripting/vec2prototype.h
	scripting/playertype.h
	scripting/resourcerequirement.h
	scripting/buildingtype.h
	scripting/colorprototype.h
	scripting/engineprototype.h
	scripting/billboard.h
	scripting/timer.h
	
	gui/shadereditor.h
	gui/playeroptionsdialog.h
	gui/facebooklogindialog.h
	gui/menuhud.h
	gui/gamehud.h
	gui/gamemenudialog.h
	gui/optionsdialog.h
	gui/progressdialog.h
)

# UI files
SET(BAARBARITY-UI
	gui/menuhud.ui
	gui/gamemenudialog.ui
	gui/optionsdialog.ui
	gui/gamehud.ui
	gui/progressdialog.ui
	gui/playeroptionsdialog.ui
	gui/facebooklogindialog.ui
	gui/shadereditor.ui
)

# Resource files
SET(BAARBARITY-RESOURCES
	data/textures.qrc
	data/shaders.qrc
	data/models.qrc
	data/portraits.qrc
	data/state.qrc
	data/scripts.qrc
	data/icons.qrc
)

SET (EXECUTABLE_OUTPUT_PATH ..)

QT4_WRAP_CPP(BAARBARITY-SOURCES-MOC ${BAARBARITY-MOC-HEADERS})
QT4_WRAP_UI(BAARBARITY-SOURCES-UI ${BAARBARITY-UI})
QT4_ADD_RESOURCES(BAARBARITY-SOURCES-RESOURCE ${BAARBARITY-RESOURCES})

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${OPENGL_INCLUDE_DIR}
	"${CMAKE_CURRENT_SOURCE_DIR}/../libcommongl"
)

ADD_EXECUTABLE(baarbarity ${BAARBARITY-SOURCES} ${BAARBARITY-SOURCES-MOC} ${BAARBARITY-SOURCES-UI} ${BAARBARITY-SOURCES-RESOURCE})

TARGET_LINK_LIBRARIES(baarbarity
	commongl
	${QT_LIBRARIES}
	${OPENGL_LIBRARIES}
)

INSTALL(TARGETS baarbarity
	RUNTIME DESTINATION bin)
