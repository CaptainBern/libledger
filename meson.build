project(
    'libledger',
    'c',
    version: '1.0.0',
    default_options: [
        'c_std=c11',
        'werror=true',
    ],
)

# see: https://autotools.io/libtool/version.html
so_version = ['0', '0', '0']

cc = meson.get_compiler('c')

libledger_inc = include_directories('include')

# Dependencies
threads = dependency('threads')
hidapi = cc.find_library('libhidapi-libusb')

libledger_headers = []
libledger_deps = []
libledger_parts = []

subdir('include')
subdir('src')

install_headers(libledger_headers, subdir: 'libledger')

libledger_deps += [
    threads,
    hidapi
]

libledger_parts += [
    libledger_internal,
    libledger_core
]

lib_libledger = library(
    meson.project_name(),
    version: '.'.join(so_version),
    link_whole: libledger_parts,
    dependencies: libledger_deps,
    include_directories: libledger_inc,
    install: true
)

libledger = declare_dependency(
    link_with: lib_libledger,
    dependencies: libledger_deps,
    include_directories: libledger_inc
)

pkgconfig = import('pkgconfig')
pkgconfig.generate(
    libraries: lib_libledger,
    version: meson.project_version(),
    filebase: meson.project_name(),
    name: meson.project_name(),
    description: 'Ledger Nano S/Ledger Blue library'
)

# example test shit fo laterz
ledger_ping = executable('ledger_ping', 'ledger_ping.c', dependencies: libledger)
test('ledger ping', ledger_ping)
