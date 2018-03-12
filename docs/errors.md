# Errors

LibLedger functions typically return `false` on error and `true` on success.
Functions returning a pointer will return `NULL` on error.

Functions that have `struct ledger_device` as parameter, will provide specific
error codes. See [devices](./devices.md) on how to access them.
