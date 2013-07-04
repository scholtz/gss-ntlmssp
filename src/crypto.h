/*
   Copyright (C) 2013 Simo Sorce <simo@samba.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SRC_CRYPTO_H_
#define _SRC_CRYPTO_H_

#include <stdbool.h>
#include "ntlm_common.h"

/**
 * @brief   Fills the provided preallocated buffer with random data
 *
 * @param random        A preallocated buffer, length determines the amount of
 *                      random bytes the function will return.
 *
 * @return 0 for success or error otherwise
 */
int RAND_BUFFER(struct ntlm_buffer *random);

/**
 * @brief HMAC-MD5 function
 *
 * @param key           The authentication key
 * @param payload       The payload to be authenticated
 * @param result        A preallocated 16 byte buffer
 *
 * @return 0 on success or ERR_CRYPTO
 */
int HMAC_MD5(struct ntlm_buffer *key,
             struct ntlm_buffer *payload,
             struct ntlm_buffer *result);

/**
 * @brief MD4 Hash Function
 *
 * @param payload   The payoad to hash
 * @param result    The resulting Hash (preallocated, length must be 16)
 *
 * @return 0 on success or an error
 */
int MD4_HASH(struct ntlm_buffer *payload,
             struct ntlm_buffer *result);

/**
 * @brief MD5 Hash Function
 *
 * @param payload   The payoad to hash
 * @param result    The resulting Hash (preallocated, length must be 16)
 *
 * @return 0 on success or an error
 */
int MD5_HASH(struct ntlm_buffer *payload,
             struct ntlm_buffer *result);

/**
 * @brief RC4 engine initialization
 *
 * @param rc4_key   The encryption/decryption key
 * @param mode      The cipher mode
 * @param state     Allocated ntlm_rc4_state structure
 *
 * @return 0 on success or error
 */
int RC4_INIT(struct ntlm_buffer *rc4_key,
             enum ntlm_cipher_mode mode,
             struct ntlm_rc4_handle **handle);


/**
 * @brief RC4 encrypt/decrypt function
 *
 * @param state     The state initialized by RC4_INIT
 * @param in        Input buffer (plaintext for enc or ciphertext for dec)
 * @param out       Resulting buffer. Must be preallocated.
 *
 * @return 0 on success or error
 */
int RC4_UPDATE(struct ntlm_rc4_handle *handle,
               struct ntlm_buffer *in, struct ntlm_buffer *out);

/**
 * @brief           Release an rc4 handle
 *
 * @param state     A pointer to the rc4 handle
 */
void RC4_FREE(struct ntlm_rc4_handle **handle);

/**
 * @brief RC4 encryption/decryption all in one
 *
 * @param key       The encryption/decryption key
 * @param mode      The cipher mode
 * @param payload   Input buffer (plaintext for enc or ciphertext for dec)
 * @param result    Resulting buffer. Must be preallocated.
 *
 * @return 0 on success or error
 */
int RC4K(struct ntlm_buffer *key,
         enum ntlm_cipher_mode mode,
         struct ntlm_buffer *payload,
struct ntlm_buffer *result);

#endif /* _SRC_CRYPTO_H_ */
