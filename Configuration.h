#pragma once
//
//  Configuration.h
//  Workbench
//
//  Created by Wai Man Chan on 10/27/14.
//
//

#define HAP_DEBUG
//#define HAP_NET_DEBUG

// Allow more than one device to be paired to the accessory set
#define HAP_MULTIPLE_PAIRING

#define HAP_THREAD_SAFE
#define HAP_NET_THREAD_SAFE

//Device Setting
#define HAP_DEVICE_MAC "11:10:34:23:51:12"						//ID
#define HAP_DEVICE_UUID "62F47751-8F26-46BF-9552-8F4238E67D60"  //UUID, for pair verify

#define HAP_NET_HAP_SERVICE_KEEP_ALIVE_PERIOD 60

#include <openssl/sha.h>
#include <stdint.h>
#include <unistd.h>

typedef SHA512_CTX SHACTX;
#define SHAInit SHA512_Init
#define SHAUpdate SHA512_Update
#define SHAFinal SHA512_Final
#define SHA_DIGESTSIZE 64
#define SHA_BlockSize 128
