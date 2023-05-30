project('WanganArcadeLoader', 'c', 'cpp', version: '1.0.0')
warning_level = 3
debug = true
optimization = 3
b_lto = true
b_pgo = 'use'

cpp = meson.get_compiler('cpp')

add_project_arguments(
	cpp.get_supported_arguments(
		'-D_WIN32_WINNT=_WIN32_WINNT_WIN10',
	),
	language: 'cpp',
)

add_project_link_arguments(
	cpp.get_supported_arguments(
		'-static',
		#'-s',
		'-lws2_32',
		'-lssp',
	),
	language: 'cpp',
)

minhook = subproject('minhook')
tomlc99 = subproject('tomlc99')
sdl2 = subproject('sdl2', default_options: ['default_library=static', 'test=false', 'use_render=disabled'])

library(
	'bngrw',
	include_directories: [
		'src',
	],
	dependencies : [
		minhook.get_variable('minhook_dep'),
		tomlc99.get_variable('tomlc99_dep'),
		sdl2.get_variable('sdl2_dep'),
	],
	sources : [
		'src/dllmain.cpp',
		'src/helpers.cpp',
		'src/poll.cpp',
	],
	name_prefix: ''
)
