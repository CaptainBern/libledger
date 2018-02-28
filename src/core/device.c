#include <stdlib.h>
#include <pthread.h>

#include <hidapi/hidapi.h>

#include "libledger/core/error.h"
#include "libledger/core/device.h"

struct ledger_device {
    hid_device *usb;
	pthread_mutex_t lock;
};

int ledger_open_device(char *path, struct ledger_device **device) {
	hid_device *usb = hid_open_path(path);
	if (!usb) {
		return LEDGER_ERROR_INTERNAL;
	}

	struct ledger_device *_device = malloc(sizeof(struct ledger_device));
	if (!device) {
		return LEDGER_ERROR_NOMEM;
	}

	_device->usb = usb;
	pthread_mutex_init(&_device->lock, NULL);
	*device = _device;

	return LEDGER_SUCCESS;
}

void ledger_close_device(struct ledger_device *device) {
	if (device) {
		pthread_mutex_destroy(&device->lock);
		hid_close(device->usb);
		free(device);
	}
}

int ledger_write(struct ledger_device *device, const uint8_t *data, size_t data_len, size_t *written) {
	int n;
	int ret;

	pthread_mutex_lock(&device->lock);
	n = hid_write(device->usb, data, data_len);
	pthread_mutex_unlock(&device->lock);

	if (n < 0) {
		ret = LEDGER_ERROR_IO;
		n = 0;
	} else {
		ret = LEDGER_SUCCESS;
	}

	if (written) {
		*written = n;
	}

	return ret;
}

int ledger_read(struct ledger_device *device, uint8_t *data, size_t data_len, size_t *read, int timeout) {
	int n;
	int ret;

	pthread_mutex_lock(&device->lock);
	n = hid_read_timeout(device->usb, data, data_len, timeout);
	pthread_mutex_unlock(&device->lock);

	if (n < 0) {
		ret = LEDGER_ERROR_IO;
		n = 0;
	} else {
		ret = LEDGER_SUCCESS;
	}

	if (read) {
		*read = n;
	}

	return ret;
}
