project('swone', 'cpp', default_options : ['cpp_std=c++17'])

GL = dependency('GL')

sfmlGraphics = dependency('sfml-graphics')
sfmlSystem = dependency('sfml-system')
sfmlWindow = dependency('sfml-window')
sfmlNetwork = dependency('sfml-network')

incdir = include_directories('include')

iSource = include_directories('./include')
iInclude = include_directories('./source')
iImgui = include_directories('./bib/imgui')
iTinydir = include_directories('./bib/tinydir')
iMd5 = include_directories('./bib/md5')

src = [	'bib/imgui/imgui-SFML.cpp',
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
	'source/client/GameMenu.cpp',
	'source/client/MainMenu.cpp',
	'source/client/Settings.cpp',
	'source/client/GameWindow.cpp',
	'source/server/Server.cpp',
	'source/server/Lobby.cpp',
	'source/server/Player.cpp',
	'source/server/SrvSettings.cpp',
	'source/game/GameController.cpp',
	'source/game/GameReader.cpp',
	'source/game/Scenery.cpp',
	'source/game/Map.cpp',
	'source/game/object/GameObject.cpp',
	'source/game/object/extensions/MovementX.cpp',
	'source/game/object/extensions/MultiJump.cpp',
	'source/game/object/extensions/Inventory.cpp',
	'source/game/object/extensions/OrientCorrection.cpp',
	'source/util/reader/Reader.cpp',
	'source/util/reader/MapReader.cpp',
	'source/util/Log.cpp',
	'source/util/Helper.cpp',
	'source/graphics/ParticleSystem.cpp',
	'source/graphics/Animation.cpp',
	'source/graphics/AnimatedSprite.cpp']

executable('swone', src, cpp_args : ['-DNOMINMAX'], include_directories : [iSource, iInclude, iImgui, iTinydir, incdir, iMd5], dependencies : [GL, sfmlGraphics, sfmlSystem, sfmlWindow, sfmlNetwork])
