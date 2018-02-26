#include "libledger/internal/binary.h"
#include "libledger/core/apdu.h"

// TODO: find a use for the APDU sw shit

/*
struct apdu_status {
    const unsigned int sw1;
    const unsigned int sw2;
    const char *description;
};

// APDU status words, taken from https://web.archive.org/web/20090623030155/http://cheef.ru/docs/HowTo/SW1SW2.info
static const struct apdu_status apdu_status_table[] = {
    { 0x62, 0x00, "No information given" },
    { 0x62, 0x81, "Returned data may be corrupted" },
    { 0x62, 0x82, "The end of the file has been reached before the end of reading" },
    { 0x62, 0x83, "Invalid DF "},
    { 0x62, 0x84, "Selected file is not valid. File descriptor error" },
    
    { 0x63, 0x00, "Authentication failed. Invalid secret code or forbidden value" },
    { 0x63, 0x81, "File filled up by the last write" },

    { 0x65, 0x01, "Memory failure" },
    { 0x65, 0x81, "Memory failure" },

    { 0x67, 0x00, "Incorrect length or address range error" },

    { 0x68, 0x00, "The request function is not supported by the card" },
    { 0x68, 0x81, "Logical channel not supported" },
    { 0x68, 0x82, "Secure messaging not supported" },

    { 0x69, 0x00, "No successful transaction executed during session" },
    { 0x69, 0x81, "Cannot select indicated file, command not compatible with file organization" },
    { 0x69, 0x82, "Access conditions not fulfilled" },
    { 0x69, 0x83, "Secret code locked" },
    { 0x69, 0x84, "Referenced data invalidated" },
    { 0x69, 0x85, "No currently selected EF" },
    { 0x69, 0x86, "Command not allowed (no current EF)" },
    { 0x69, 0x87, "Expected SM data objects missing" },
    { 0x69, 0x88, "SM data objects incorrect" },

    { 0x6A, 0x00, "Bytes P1 and/or P2 are incorrect" },
    { 0x6A, 0x80, "The parameters in the data field are incorrect" },
    { 0x6A, 0x81, "Card is blocked or command is not supported" },
    { 0x6A, 0x82, "File not found" },
    { 0x6A, 0x83, "Record not found" },
    { 0x6A, 0x84, "There is insufficient memory space in record or file" },
    { 0x6A, 0x85, "Lc inconsistent with TLV structure" },
    { 0x6A, 0x86, "Incorrect parameters P1-P2" },
    { 0x6A, 0x87, "The P3 value is not consistent with the P1 and P2 values" },
    { 0x6A, 0x88, "Referenced data not found" },

    { 0x6B, 0x00, "Invalid P1 or P2 parameter" },
    { 0x6D, 0x00, "Invalid or unsupported instruction" },
    { 0x6E, 0x00, "Incorrect application" },
    { 0x6F, 0x00, "Checking error" },

    { 0x90, 0x00, "Command executed without error" }
};

static const char* apdu_sw_description(uint8_t sw1, uint8_t sw2) {
    const int table_size = sizeof(apdu_status_table) / sizeof(struct apdu_status);

    switch (sw1) {
        case 0x6C: {
            return "Incorrect P3 length"; // Special case, correct length is stored in sw2
        }
        default: {
            for (int i = 0; i < table_size; i++) {
                if (apdu_status_table[i].sw1 == sw1 && apdu_status_table[i].sw2 == sw2) {
                    return apdu_status_table[i].description;
                }
            }
        }
    }

    return "Unknown sw1-sw2";
}

*/

size_t ledger_apdu(void *out, size_t out_len, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2) {
    if (out_len < 4) {
        return 0;
    }

    size_t written = 0;
    written += binary_hton_uint8(out + written, cla);
    written += binary_hton_uint8(out + written, ins);
    written += binary_hton_uint8(out + written, p1);
    written += binary_hton_uint8(out + written, p2);
    
    return written;
}

size_t ledger_apdu_le(void *out, size_t out_len, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t le) {
    if (out_len < (4 + 1)) {
        return 0;
    }

    size_t written = 0;
    written += binary_hton_uint8(out + written, cla);
    written += binary_hton_uint8(out + written, ins);
    written += binary_hton_uint8(out + written, p1);
    written += binary_hton_uint8(out + written, p2);
    written += binary_hton_uint8(out + written, le);

    return written;
}

size_t ledger_apdu_data(void *out, size_t out_len, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *data, size_t data_len) {
    if (out_len < (4 + 1 + data_len)) {
        return 0;
    }

    size_t written = 0;
    written += binary_hton_uint8(out + written, cla);
    written += binary_hton_uint8(out + written, ins);
    written += binary_hton_uint8(out + written, p1);
    written += binary_hton_uint8(out + written, p2);
    written += binary_hton_uint8(out + written, data_len);
    written += binary_hton(out + written, data, data_len);

    return written;
}

size_t ledger_apdu_data_le(void *out, size_t out_len, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *data, size_t data_len, uint8_t le) {
    if (out_len < (4 + 1 + data_len + 1)) {
        return 0;
    }

    size_t written = 0;
    written += binary_hton_uint8(out + written, cla);
    written += binary_hton_uint8(out + written, ins);
    written += binary_hton_uint8(out + written, p1);
    written += binary_hton_uint8(out + written, p2);
    written += binary_hton_uint8(out + written, data_len);
    written += binary_hton(out + written, data, data_len);
    written += binary_hton_uint8(out + written, le);
    
    return written;
}