#include "libledger/version.h"

const char *libledger_version(void) {
	return LIBLEDGER_VERSION;
}

int libledger_version_major(void) {
	return LIBLEDGER_VERSION_MAJOR;
}

int libledger_version_minor(void) {
	return LIBLEDGER_VERSION_MINOR;
}

int libledger_version_patch(void) {
	return LIBLEDGER_VERSION_PATCH;
}
