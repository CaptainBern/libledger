#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hidapi/hidapi.h>
#include <libledger/libledger.h>
#include <libledger/internal/binary.h>

/**
 * Browser Support needs to be turned off inside the Ethereum App for this to work!
 *
 * Contract Data can be either turned off or on; it appears to glitch out sometimes tho
 */
void ethereum_app_get_version(struct ledger_device *device) {
    uint8_t apdu[5];
    apdu[0] = 0xe0;
    apdu[1] = 0x06;
    apdu[2] = 0x00;
    apdu[3] = 0x00;
    apdu[4] = 0x00;

    int ret = 0;
    ret = ledger_transport_write_apdu(device, LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID, &apdu[0], sizeof apdu);
    if (ret != LEDGER_SUCCESS) {
        printf("Failed to write APDU: %s\n", ledger_error_string(ret));
        return;
    }

    struct ledger_transport_apdu_reply *reply;
    ret = ledger_transport_read_apdu(device, LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID, &reply);
    if (ret != LEDGER_SUCCESS) {
        printf("Failed to read APDU: %s\n", ledger_error_string(ret));
        return;
    }

    printf("APDU length: %d\n", reply->apdu_len);

    size_t offset = 0;

    uint8_t flags;
    uint8_t major;
    uint8_t minor;
    uint8_t patch;

    offset += binary_ntoh_uint8(&reply->apdu[offset], &flags);
    offset += binary_ntoh_uint8(&reply->apdu[offset], &major);
    offset += binary_ntoh_uint8(&reply->apdu[offset], &minor);
    offset += binary_ntoh_uint8(&reply->apdu[offset], &patch);

    printf("Flags: %d\n", flags);
    printf("Version: %d.%d.%d\n", major, minor, patch);

    uint16_t sw = 0;
    offset += binary_ntoh_uint16(&reply->apdu[offset], &sw);
    printf("SW: 0x%x\n", sw);

    ledger_transport_free_apdu_reply(reply);
}

void bolos_get_version(struct ledger_device *device) {
    int ret = 0;

    printf("Writing GET_VERSION APDU:\n");
    uint8_t apdu[LEDGER_APDU_HEADER_SIZE];
    apdu[0] = 0xe0;
    apdu[1] = 0x01;
    apdu[2] = 0x00;
    apdu[3] = 0x00;
    apdu[4] = 0x00;

    ret = ledger_transport_write_apdu(device, LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID, &apdu[0], sizeof apdu);
    if (ret != LEDGER_SUCCESS) {
        printf("Failed to write APDU: %s\n", ledger_error_string(ret));
        return;
    }

    printf("\t-> Success\n");
    printf("Received response:\n");

    struct ledger_transport_apdu_reply *reply;
    ret = ledger_transport_read_apdu(device, LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID, &reply);
    if (ret != LEDGER_SUCCESS) {
        printf("Failed to read APDU: %s\n", ledger_error_string(ret));
        return;
    }

    printf("APDU length: %d\n", reply->apdu_len);

    size_t offset = 0;

    uint32_t target_id;
    offset += binary_ntoh_uint32(&reply->apdu[offset], &target_id);
    printf("\ttarget ID: %x\n", target_id);

    uint8_t version_str_len;
    offset += binary_ntoh_uint8(&reply->apdu[offset], &version_str_len);

    char version_str[version_str_len];
    offset += binary_ntoh(&reply->apdu[offset], &version_str[0], sizeof version_str);
    printf("\tversion string: %s\n", version_str);

    uint8_t flags_length;
    offset += binary_ntoh_uint8(&reply->apdu[offset], &flags_length);

    uint8_t flags[flags_length];
    offset += binary_ntoh(&reply->apdu[offset], &flags[0], flags_length);

    uint8_t mcu_str_length;
    offset += binary_ntoh_uint8(&reply->apdu[offset], &mcu_str_length);

    char mcu[mcu_str_length];
    offset += binary_ntoh(&reply->apdu[offset], &mcu[0], mcu_str_length);
    printf("\tMCU: %s\n", mcu);

    uint16_t sw;
    offset += binary_ntoh_uint16(&reply->apdu[offset], &sw);
    printf("\tSW: 0x%x\n", sw);

    ledger_transport_free_apdu_reply(reply);
}

int main() {
    printf("Libledger Version: %s\n", libledger_version());
    printf("Devices:\n");

    struct ledger_device *device = NULL;

    struct hid_device_info *devices;
    struct hid_device_info *current_device;

    devices = hid_enumerate(LEDGER_VENDOR_ID, LEDGER_NANO_S_PRODUCT_ID);
    current_device = devices;
    while (current_device != NULL) {
        printf("Found Ledger Nano S: %s\n", current_device->path);
        int ret = ledger_open_device(current_device->path, &device);
        if (ret != LEDGER_SUCCESS) {
            printf("Error: %d\n", ret);
            return -1;
        }
        break;
    }

    hid_free_enumeration(devices);

    if (!device) {
        printf("Failed to find a Ledger device!\n");
        return -1;
    }

    //bolos_get_version(device);
    ethereum_app_get_version(device);
    int ret = ledger_transport_ping(device);
    printf("Ping status: %s\n", ledger_error_string(ret));

    ledger_close_device(device);

    hid_exit();

    return 0;
}
