mbedcrypto = cc.find_library('libmbedcrypto')
libledger_bolos = static_library(
    'libledger_bolos',
    files(
        'reset.c',
        'validate.c',
        'version.c',
        'secure.c'
    ),
    include_directories: libledger_inc,
    dependencies: [
        mbedcrypto
    ]
)
