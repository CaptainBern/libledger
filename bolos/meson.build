openssl = dependency('openssl')

libledger_bolos = static_library(
    'libledger_bolos',
    files(
        'reset.c',
        'validate.c',
        'version.c',
        'secret.c'
    ),
    include_directories: libledger_inc,
    dependencies: [
        openssl
    ]
)
