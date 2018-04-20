#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecp.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/sha256.h>

#include "internal/cursor.h"
#include "internal/macros.h"

#include "libledger/error.h"
#include "libledger/apdu.h"
#include "libledger/bolos/constants.h"

#include "libledger/bolos/secure_channel.h"

#define RNG_PERSONALISATION "LibLedger"

/**
 * Ledger Secure Channel Protocol:
 * 1) Generate Signer Keypair (SKP) + Signer Nonce (SN)
 * 2) Send SN to the device
 * 3) Device replies with Signer Serial (SS) and Device Nonce (DN)
 * 4) Send SKP publickey + a signature of the SKP publickey to the device
 * 5) Device replies with OK
 * 6) Generate Ephemeral Keypair (EKP)
 * 7) Create a Signer Certificate (SC) -> SKP_sign(SN + DN + EKP_public_key)
 * 8) Send EKP Public key + SC to the device
 * 9) Device replies with OK
 * 10) Request Device Certificate Chain (DCC)
 * 11) Device replies with DCC -> Same as SC but using the Device EKP
 * 12) Compute ECDH Shared (no idea how to do that yet)
 */
