#include <stdlib.h>
#include <pthread.h>

#include <hidapi/hidapi.h>

#include "internal/macros.h"

#include "libledger/error.h"

#include "libledger/device.h"
#include "internal/device.h"

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
	if (!usb)
		return NULL;

	struct ledger_device *device = malloc(sizeof(struct ledger_device));
	if (!device)
		return NULL;

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

bool ledger_write(struct ledger_device *device, size_t *written, const uint8_t *buffer, size_t buffer_len)
{
	int n;

	pthread_mutex_lock(&device->lock);
	n = hid_write(device->usb, buffer, buffer_len);
	pthread_mutex_unlock(&device->lock);

	if (n < 0) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
	}

	if (written)
		*written = n;

	return true;
}

bool ledger_read(struct ledger_device *device, size_t *read, uint8_t *buffer, size_t buffer_len, int timeout)
{
	int n;

	pthread_mutex_lock(&device->lock);
	n = hid_read_timeout(device->usb, buffer, buffer_len, timeout);
	pthread_mutex_unlock(&device->lock);

	if (n < 0) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
	}

	*read = n;

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
