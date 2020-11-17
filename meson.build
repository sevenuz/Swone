project('swone', 'cpp', default_options : ['cpp_std=c++17'])

GL = dependency('GL')

sfmlGraphics = dependency('sfml-graphics')
sfmlSystem = dependency('sfml-system')
sfmlWindow = dependency('sfml-window')

incdir = include_directories('include')

iSource = include_directories('./include')
iInclude = include_directories('./source')
iImgui = include_directories('./bib/imgui')
iTinydir = include_directories('./bib/tinydir')

src = [	'bib/imgui/imgui-SFML.cpp',
	'bib/imgui/imgui.cpp',
	'bib/imgui/imgui_draw.cpp',
	'bib/imgui/imgui_demo.cpp',
	'source/main.cpp',
	'source/Controller.cpp',
	'source/GameMenu.cpp',
	'source/MainMenu.cpp',
	'source/Settings.cpp',
	'source/game/GameController.cpp',
	'source/game/GameObject.cpp',
	'source/game/GameWindow.cpp',
	'source/game/Map.cpp',
	'source/game/Player.cpp',
	'source/util/reader/Reader.cpp',
	'source/util/reader/MapReader.cpp',
	'source/util/Log.cpp',
	'source/util/Helper.cpp',
	'source/graphics/ParticleSystem.cpp',
	'source/graphics/Animation.cpp',
	'source/graphics/AnimatedSprite.cpp']

executable('swone', src, cpp_args : ['-DNOMINMAX'], include_directories : [iSource, iInclude, iImgui, iTinydir, incdir], dependencies : [GL, sfmlGraphics, sfmlSystem, sfmlWindow])
