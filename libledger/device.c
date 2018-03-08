#include <stdlib.h>
#include <pthread.h>

#include <hidapi/hidapi.h>

#include "internal/macros.h"
#include "libledger/error.h"

#include "libledger/device.h"
#include "libledger/device_internal.h"

struct ledger_device {
    hid_device *usb;

	struct {
		int error;
		char *error_debug_str;
	} error_state;

	pthread_mutex_t lock;
};

struct ledger_device *ledger_open(char *path)
{
	hid_device *usb = hid_open_path(path);
	if (!usb) {
		return NULL;
	}

	struct ledger_device *device = malloc(sizeof(struct ledger_device));
	if (!device) {
		return NULL;
	}

	device->usb = usb;
	device->error_state.error = LEDGER_SUCCESS;
	device->error_state.error_debug_str = "";
	pthread_mutex_init(&device->lock, NULL);

	return device;
}

void ledger_close(struct ledger_device *device)
{
	if (device) {
		pthread_mutex_destroy(&device->lock);
		hid_close(device->usb);
		free(device);
	}
}

bool ledger_write(struct ledger_device *device, const struct ledger_buffer *buffer, size_t *written)
{
	int n;

	pthread_mutex_lock(&device->lock);
	n = hid_write(device->usb, buffer->data, buffer->len);
	pthread_mutex_unlock(&device->lock);

	if (n < 0) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
	}

	if (written) {
		*written = n;
	}

	return true;
}

bool ledger_read(struct ledger_device *device, struct ledger_buffer *buffer, size_t *read, int timeout)
{
	int n;

	pthread_mutex_lock(&device->lock);
	n = hid_read_timeout(device->usb, buffer->data, buffer->len, timeout);
	pthread_mutex_unlock(&device->lock);

	if (n < 0) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
	}

	if (read) {
		*read = n;
	}

	return true;
}

void ledger_set_error(struct ledger_device *device, int error, char *error_debug_str)
{
	pthread_mutex_lock(&device->lock);
	device->error_state.error = error;
	device->error_state.error_debug_str = error_debug_str;
	pthread_mutex_unlock(&device->lock);
}

int ledger_get_error(struct ledger_device *device)
{
	int errno = 0;
	pthread_mutex_lock(&device->lock);
	errno = device->error_state.error;
	pthread_mutex_unlock(&device->lock);
	return errno;
}

char *ledger_get_error_debug_str(struct ledger_device *device)
{
	char *error_debug_str = NULL;
	pthread_mutex_lock(&device->lock);
	error_debug_str = device->error_state.error_debug_str;
	pthread_mutex_unlock(&device->lock);
	return error_debug_str;
}

void ledger_clear_error(struct ledger_device *device)
{
	ledger_set_error(device, LEDGER_SUCCESS, "");
}
