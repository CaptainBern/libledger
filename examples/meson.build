libshared = static_library(
    'libshared',
    files('common/common.c'),
    dependencies: libledger,
    include_directories: include_directories('common')
)

executable('ping', 'ping.c', dependencies: libledger, link_with: libshared)

executable('reset', 'reset.c', dependencies: libledger, link_with: libshared)
