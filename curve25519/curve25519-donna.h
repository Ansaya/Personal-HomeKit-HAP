//
//  curve25519-donna.h
//
//  Created by Wolfgang on 1/1/14.
//  Copyright (c) 2014 Wolfgang. All rights reserved.
//

#ifndef curve25519_donna_h
#define curve25519_donna_h

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef uint8_t u8;
extern int curve25519_donna(u8 *, const u8 *, const u8 *);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
