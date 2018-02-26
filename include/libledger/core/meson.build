version_conf = configuration_data()
libledger_version = meson.project_version()
libledger_version_arr = libledger_version.split('.')
version_conf.set('LIBLEDGER_VERSION', libledger_version)
version_conf.set('LIBLEDGER_VERSION_MAJOR', libledger_version_arr[0].to_int())
version_conf.set('LIBLEDGER_VERSION_MINOR', libledger_version_arr[1].to_int())
version_conf.set('LIBLEDGER_VERSION_PATCH', libledger_version_arr[2].to_int())

configure_file(
    input: 'version.h.in',
    output: 'version.h',
    configuration: version_conf
)

libledger_headers += files(
    'apdu.h',
    'device.h',
    'error.h',
    'transport.h'
)