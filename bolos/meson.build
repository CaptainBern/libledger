openssl = dependency('openssl')

libledger_bolos = static_library(
    'libledger_bolos',
    files(
        'reset.c',
    ),
    include_directories: libledger_inc,
    dependencies: [
        openssl
    ]
)
