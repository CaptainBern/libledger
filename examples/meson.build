libshared = static_library(
    'libshared',
    files('common/common.c'),
    dependencies: libledger,
    include_directories: include_directories('common')
)

# TLP examples
executable('ping', 'ping.c', dependencies: libledger, link_with: libshared)

# BOLOS examples
executable('reset', 'reset.c', dependencies: libledger, link_with: libshared)
executable('validate', 'validate.c', dependencies: libledger, link_with: libshared)
executable('version', 'version.c', dependencies: libledger, link_with: libshared)
executable('secure', 'secure.c', dependencies: libledger, link_with: libshared)
