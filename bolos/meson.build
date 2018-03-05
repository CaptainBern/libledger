openssl = dependency('openssl')

libledger_bolos = static_library(
    'libledger_bolos',
    files(
        'get_version.c',
        'reset.c',
        'validate_target_id.c'
    ),
    include_directories: libledger_inc,
    dependencies: [
        openssl
    ]
)
