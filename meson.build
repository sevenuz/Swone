project('swone', 'cpp', default_options : ['cpp_std=c++17'])

GL = dependency('GL')

sfmlGraphics = dependency('sfml-graphics')
sfmlSystem = dependency('sfml-system')
sfmlWindow = dependency('sfml-window')
sfmlNetwork = dependency('sfml-network')

thread_dep = dependency('threads')

incdir = include_directories('include')

iSource = include_directories('./include')
iInclude = include_directories('./source')
iImgui = include_directories('./bib/imgui')
iTinydir = include_directories('./bib/tinydir')
iMd5 = include_directories('./bib/md5')

src = [
	'bib/imgui/imgui-SFML.cpp',
	'bib/md5/md5.cpp',
	'bib/imgui/imgui.cpp',
	'bib/imgui/imgui_draw.cpp',
	'bib/imgui/imgui_demo.cpp',
	'source/physics/Body.cpp',
	'source/physics/Collision.cpp',
	'source/physics/Manifold.cpp',
	'source/physics/Scene.cpp',
	'source/main.cpp',
	'source/client/Client.cpp',
	'source/client/Controller.cpp',
	'source/client/menu/MainMenu.cpp',
	'source/client/menu/LocalMenu.cpp',
	'source/client/menu/OnlineMenu.cpp',
	'source/client/menu/SettingsMenu.cpp',
	'source/client/game/CharacterSelection.cpp',
	'source/client/game/InfoPanel.cpp',
	'source/client/game/NetController.cpp',
	'source/client/game/ClientGameController.cpp',
	'source/client/game/GameWindow.cpp',
	'source/client/game/MapDrawing.cpp',
	'source/client/game/object/GameObjectDrawing.cpp',
	'source/client/game/object/extensions/InventoryDrawing.cpp',
	'source/client/Settings.cpp',
	'source/server/Server.cpp',
	'source/server/Lobby.cpp',
	'source/server/Player.cpp',
	'source/server/SrvSettings.cpp',
	'source/game/GameController.cpp',
	'source/game/GameReader.cpp',
	'source/game/Scenery.cpp',
	'source/game/Map.cpp',
	'source/game/Net.cpp',
	'source/game/object/GameObject.cpp',
	'source/game/object/extensions/MovementX.cpp',
	'source/game/object/extensions/MultiJump.cpp',
	'source/game/object/extensions/Inventory.cpp',
	'source/game/object/extensions/OrientCorrection.cpp',
	'source/util/reader/Reader.cpp',
	'source/util/reader/MapReader.cpp',
	'source/util/Log.cpp',
	'source/util/Helper.cpp',
	'source/util/embedded_data.cpp',
	'source/graphics/ParticleSystem.cpp',
	'source/graphics/Animation.cpp',
	'source/graphics/AnimatedSprite.cpp'
]

executable(
	'swone',
	src,
	cpp_args : ['-DNOMINMAX'],
	include_directories : [
		iSource,
		iInclude,
		iImgui,
		iTinydir,
		incdir,
		iMd5
	],
	dependencies : [
		GL,
		sfmlGraphics,
		sfmlSystem,
		sfmlWindow,
		sfmlNetwork,
		thread_dep
	]
)
