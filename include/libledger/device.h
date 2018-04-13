#ifndef LIBLEDGER_DEVICE_H_
#define LIBLEDGER_DEVICE_H_

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Ledger USB Vendor ID */
#define LEDGER_VENDOR_ID         0x2c97

/* Ledger Blue USB Product ID */
#define LEDGER_BLUE_PRODUCT_ID   0x0000

/* Ledger Nano S USB Product ID */
#define LEDGER_NANO_S_PRODUCT_ID 0x0001

/**
 * struct ledger_device - A ledger device handle.
 *
 * An opaque struct that holds the underlying
 * USB device handle.
 */
struct ledger_device;

/**
 * ledger_open() - Open a Ledger device.
 * @path: The device path.
 *
 * Opens a Ledger device. A ledger_device allows you
 * to perform I/O operations on the device in question.
 * LibLedger will not verify that the device in question
 * is an actual Ledger device. As a result of this, passing
 * in a path to any type of HID device will make the
 * function return successful.
 *
 * Return: Pointer to a ledger_device on success or NULL on failure.
 */
extern struct ledger_device *ledger_open(char *path);

/**
 * ledger_close() - Close a Ledger device.
 * @device: A ledger_device pointer returned from ledger_open().
 */
extern void ledger_close(struct ledger_device *device);

/**
 * ledger_write() - Write raw data to the device.
 * @device: A ledger_device pointer returned from ledger_open().
 * @buffer: The data to send to the device.
 * @written: Pointer location to the amount of bytes actually
 *           written. This value may be NULL.
 *
 * You should generally avoid using this function and instead
 * use the dedicated ledger_transport_* and ledger_apdu_* functions
 * to communicate with the device. The reason this function is part
 * of the public API, is for the edge cases where you might need the
 * lowest level of I/O possible.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_write(struct ledger_device *device, const struct ledger_buffer *buffer, size_t *written);

/**
 * ledger_read() - Read raw data from the device.
 * @device: A ledger_device pointer returned from ledger_open().
 * @buffer: The data to read from the device.
 * @read: Pointer location to the amount of bytes actually read.
 *        This value may be NULL.
 * @timeout: Timeout in milliseconds or -1 for blocking wait.
 *
 * You should generally avoid using this function and instead
 * use the dedicated leger_transport_* and ledger_apdu_* functions
 * to communicate with the device. The reason this function is part
 * of the public API, is for the edge cases where you might need the
 * lowest level of I/O possible.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_read(struct ledger_device *device, struct ledger_buffer *buffer, size_t *read, int timeout);

/**
 * ledger_get_error() - Returns the last error which occurred.
 * @device: A ledger_device pointer returned from ledger_open().
 *
 * All LibLedger functions that can fail and require a ledger_device
 * pointer, will store error information in the ledger_device object.
 * LibLedger is thread safe so you can safely access the value from
 * multiple threads, though it should be noted that the error value
 * is always set to the one of the last function that failed.
 * Functions that executed successfully will not alter this value.
 *
 * Return: Error code of the last error which occurred.
 */
extern int ledger_get_error(struct ledger_device *device);

/**
 * ledger_get_error_debug_str() - Return a debug string.
 * @device: A ledger_device pointer returned from ledger_open().
 *
 * This function is similar to ledger_get_error(). The value that
 * is returned is always set by a function that failed. The value
 * in this case is a string containing debug information related
 * to the error. In most cases it is a string containing the file
 * and line where the error occurred exactly.
 *
 * Return: Debug string related to the last error which occurred.
 */
extern char *ledger_get_error_debug_str(struct ledger_device *device);

/**
 * ledger_clear_error() - Clear the error information.
 * @device: A ledger_device pointer returned from ledger_open().
 *
 * Clear the error and debug information stored in the given
 * ledger_device object. It is generally advised to call this function
 * after you are done checking for errors. You could also call it before
 * calling any functions (that use ledger_device). This is to prevent
 * 'error-leaks'. Functions that execute successfully will not alter
 * the error/debug values. Checking these values after a function
 * executed successfully might return errors from a previously failed
 * function.
 */
extern void ledger_clear_error(struct ledger_device *device);


#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_DEVICE_H_ */
