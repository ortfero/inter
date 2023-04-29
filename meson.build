project('inter', 'cpp',
    license: 'MIT',
    default_options: [
        'warning_level=3'])

cpp = meson.get_compiler('cpp')
if cpp.get_id() == 'msvc'
    add_global_arguments('/std:c++23', language: 'cpp')
else
    add_global_arguments('-std=c++23', language: 'cpp')
endif

headers = [
    'include/inter/sockets.hpp'
]

incdirs = include_directories('./include')

inter= declare_dependency(
    include_directories: incdirs,
    sources: headers
)

subdir('test')
