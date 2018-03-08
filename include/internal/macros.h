#ifndef LIBLEDGER_INTERNAL_MACRO_H_
#define LIBLEDGER_INTERNAL_MACRO_H_

#include "internal/device.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define LEDGER_AT __FILE__ ":" TOSTRING(__LINE__)
#define LEDGER_DEBUG_LINE "Error encountered at " LEDGER_AT

#define LEDGER_SET_ERROR(device, error) ledger_set_error((device), (error), LEDGER_DEBUG_LINE)

#endif /* LIBLEDGER_INTERNAL_MACRO_H_ */
