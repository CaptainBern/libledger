version_conf = configuration_data()
libledger_version = meson.project_version()
libledger_version_arr = libledger_version.split('.')
version_conf.set('LIBLEDGER_VERSION_MAJOR', libledger_version_arr[0].to_int())
version_conf.set('LIBLEDGER_VERSION_MINOR', libledger_version_arr[1].to_int())
version_conf.set('LIBLEDGER_VERSION_PATCH', libledger_version_arr[2].to_int())

configure_file(
    output: 'config.h',
    configuration: conf_data
)

configure_file(
    input: 'version.h.in',
    output: 'version.h',
    configuration: version_conf
)

libledger_headers += files(
    'apdu.h',
    'buffer.h',
    'device.h',
    'error.h',
    'libledger.h',
    'transport.h'
)

if get_option('bolos_support')
    subdir('bolos')
endif

if get_option('u2f_support')
    subdir('u2f')
endif

if get_option('ethereum_support')
    subdir('ethereum')
endif

if get_option('bitcoin_support')
    subdir('bitcoin')
endif
