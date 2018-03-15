# Device

## Opening devices

LibLedger allows you to open multiple devices at once. Every
open device is managed through a device handle (`ledger_device`).

To aquire a device handle, simply call the `ledger_open` function:

```C
#include <libledger/libledger.h>

...

struct ledger_device *handle = ledger_open("/path/to_device");
if (!handle) {
    // Oh noes! Failed to open the device!
    return;
}

...
```

Internally, LibLedger makes use of [HID API](http://www.signal11.us/oss/hidapi/)
which makes use of [LibUSB](http://libusb.info/) (at least in the version used by LibLedger).

It is up to you to find possible Ledger devices. You can use whatever way you like.
If, for example, you'd use HID API, you could do something like this:

```C
#include <hidapi/hidapi.h>
#include <libledger/libledger.h>

...

struct hid_device_info *device_info = hid_enumerate(LEDGER_VENDOR_ID, LEDGER_NANO_S_PRODUCT_ID);
if (!device_info) {
    // Failed to find any devices matching our criteria
    return;
}

struct ledger_device *handle = ledger_open(device_info->path);

hid_free_enumeration(device_info);

...
```

LibLedger defines 3 macros to help you with identifying (supported) Ledger devices:

- LEDGER_VENDOR_ID
- LEDGER_BLUE_PRODUCT_ID
- LEDGER_NANO_S_PRODUCT_ID

## Closing devices

After you are done using it, the device should be closed to cleanup and prevent memory leaks.
Closing the device is done through the `ledger_close` function:

```C
#include <libledger/libledger.h>

...

struct ledger_device *handle = ...;

...

ledger_close(handle);

...
```

## Error handling

See [errors](./errors.md).

The *last* error that occured is stored in the device handle. You can access the error code by using
the `ledger_get_error` function.

```C
#include <libledger/libledger.h>

...

struct ledger_device *handle = ...;

...

// Some function call failed. Let's check the error
int error = ledger_get_error(handle);

...
```

In case you wish to provide the user with an error message, or log the error in human-readable format,
you can use the `ledger_error_str` function:

```C
#include <stdio.h>

#include <libledger/libledger.h>

...

struct ledger_device *handle = ...;

...

// Some function call failed. Let's check the error
int error = ledger_get_error(handle);
printf("Error: %s\n", ledger_error_str(error));

...
```

Sometimes you want to be able to check what _type_ an error is. Is it I/O related? Or
was it an internal error?

There are currently 5 error types:

- LEDGER_ERROR_TYPE_SUCCESS
- LEDGER_ERROR_TYPE_INTERNAL
- LEDGER_ERROR_TYPE_IO
- LEDGER_ERROR_TYPE_PROTOCOL
- LEDGER_ERROR_TYPE_OTHER

You can check what type an error is by using the `ledger_get_error_type` function:

```C
#include <stdio.h>

#include <libledger/libledger.h>

...

struct ledger_device *handle = ...;

...

// Some function call failed. Let's check the error
int error = ledger_get_error(handle);
switch (ledger_get_error_type(error)) {
case LEDGER_ERROR_TYPE_INTERNAL:
    printf("Internal error!\n");
    break;
case LEDGER_ERROR_TYPE_IO:
    printf("I/O error!\n");
    break;
default:
    printf("Some other error!\n");
    break;
}

...
```

After you are done handling the error, it is important to call the `ledger_clear_error` function to
ensure the error won't 'leak' after a function was successful (successful functions don't change the
error value):

```C
#include <libledger/libledger.h>

...

struct ledger_device *handle = ...;

...

// After handling the error (or after the function finishes)
ledger_clear_error(handle);

...
```

You could optionally call `ledger_clear_error` before every function call (to a function that takes
`struct ledger_device` as a parameter). This way you are sure there is no 'error-leakage'.

(Note that 'error-leakage' is only a problem when you ignore the result of the function. If you,
for example, check the error value after the function executes successfully, you might
be checking against an old error.)

Additionally you can access debugging information via the `ledger_get_error_debug_str` function.
This function will return a string that specifies where the error occurred.

```C
#include <stdio.h>

#include <libledger/libledger.h>

...

struct ledger_device *handle = ...;

...

// Some function call failed. Let's print the debug string
printf("%s\n", ledger_get_error_debug_str(handle));

...
```

These functions are thread-safe. Keep in mind though that only the *last* error is stored.
