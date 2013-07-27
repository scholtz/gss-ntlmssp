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

#ifndef _GSS_NTLMSSP_H_
#define _GSS_NTLMSSP_H_

#include "ntlm.h"
#include "crypto.h"

#define MAX_CHALRESP_LIFETIME 36 * 60 * 60 /* 36 hours in seconds */

#define SEC_LEVEL_MIN 0
#define SEC_LEVEL_MAX 5

#define SEC_LM_OK 0x01
#define SEC_NTLM_OK 0x02
#define SEC_EXT_SEC_OK 0x04
#define SEC_V2_ONLY 0x08
#define SEC_DC_LM_OK 0x10
#define SEC_DC_NTLM_OK 0x20
#define SEC_DC_V2_OK 0x40

#define NTLMSSP_DEFAULT_CLIENT_FLAGS ( \
                NTLMSSP_NEGOTIATE_ALWAYS_SIGN | \
                NTLMSSP_NEGOTIATE_128 | \
                NTLMSSP_NEGOTIATE_56 | \
                NTLMSSP_NEGOTIATE_NTLM | \
                NTLMSSP_REQUEST_TARGET | \
                NTLMSSP_NEGOTIATE_UNICODE)

#define NTLMSSP_DEFAULT_ALLOWED_SERVER_FLAGS ( \
                NTLMSSP_NEGOTIATE_ALWAYS_SIGN | \
                NTLMSSP_NEGOTIATE_56 | \
                NTLMSSP_NEGOTIATE_KEY_EXCH | \
                NTLMSSP_NEGOTIATE_128 | \
                NTLMSSP_NEGOTIATE_VERSION | \
                NTLMSSP_TARGET_TYPE_SERVER | \
                NTLMSSP_TARGET_TYPE_DOMAIN | \
                NTLMSSP_NEGOTIATE_ALWAYS_SIGN | \
                NTLMSSP_NEGOTIATE_OEM_WORKSTATION_SUPPLIED | \
                NTLMSSP_NEGOTIATE_OEM_DOMAIN_SUPPLIED | \
                NTLMSSP_NEGOTIATE_NTLM | \
                NTLMSSP_NEGOTIATE_SEAL | \
                NTLMSSP_NEGOTIATE_SIGN | \
                NTLMSSP_REQUEST_TARGET | \
                NTLMSSP_NEGOTIATE_OEM | \
                NTLMSSP_NEGOTIATE_UNICODE)

struct gssntlm_name {
    enum ntlm_name_type {
        GSSNTLM_NAME_ANON,
        GSSNTLM_NAME_USER,
        GSSNTLM_NAME_SERVER
    } type;

    union {
        struct {
            char *domain;
            char *name;
        } user;
        struct {
            char *name;
        } server;
    } data;
};

struct gssntlm_cred {
    enum ntlm_cred_type {
        GSSNTLM_CRED_NONE,
        GSSNTLM_CRED_ANON,
        GSSNTLM_CRED_USER,
        GSSNTLM_CRED_SERVER
    } type;

    union {
        struct {
            int dummy;
        } anon;
        struct {
            struct gssntlm_name user;
            struct ntlm_key nt_hash;
            struct ntlm_key lm_hash;
        } user;
        struct {
            struct gssntlm_name name;
        } server;
    } cred;
};

struct gssntlm_signseal {
    struct ntlm_key sign_key;
    struct ntlm_key seal_key;
    struct ntlm_rc4_handle *seal_handle;
    uint32_t seq_num;
};

struct gssntlm_ctx {
    enum gssntlm_role {
        GSSNTLM_CLIENT,
        GSSNTLM_SERVER,
        GSSNTLM_DOMAIN_SERVER,
        GSSNTLM_DOMAIN_CONTROLLER
    } role;

    enum {
        NTLMSSP_STAGE_INIT = 0,
        NTLMSSP_STAGE_NEGOTIATE,
        NTLMSSP_STAGE_CHALLENGE,
        NTLMSSP_STAGE_AUTHENTICATE,
        NTLMSSP_STAGE_DONE
    } stage;

    struct gssntlm_cred cred;
    char *workstation;

    struct ntlm_ctx *ntlm;
    struct ntlm_buffer nego_msg;
    struct ntlm_buffer chal_msg;
    struct ntlm_buffer auth_msg;

    uint8_t server_chal[8];

    /* requested gss fags */
    uint32_t gss_flags;

    /* negotiated flags */
    uint32_t neg_flags;

    /* TODO: Add whitelist of servers we are allowed to communicate with */

    struct ntlm_key exported_session_key;
    struct gssntlm_signseal send;
    struct gssntlm_signseal recv;

    bool established;
    time_t expiration_time;
};

uint8_t gssntlm_required_security(int security_level,
                                  enum gssntlm_role role);

uint32_t gssntlm_context_is_valid(struct gssntlm_ctx *ctx,
                                  time_t *time_now);

int gssntlm_get_lm_compatibility_level(void);

void gssntlm_int_release_name(struct gssntlm_name *name);
void gssntlm_int_release_cred(struct gssntlm_cred *cred);

int gssntlm_copy_name(struct gssntlm_name *src, struct gssntlm_name *dst);
int gssntlm_copy_creds(struct gssntlm_cred *in, struct gssntlm_cred *out);

extern const gss_OID_desc gssntlm_oid;

extern const gss_OID_desc gssntlm_oid;

uint32_t gssntlm_acquire_cred(uint32_t *minor_status,
                              gss_name_t desired_name,
                              uint32_t time_req,
                              gss_OID_set desired_mechs,
                              gss_cred_usage_t cred_usage,
                              gss_cred_id_t *output_cred_handle,
                              gss_OID_set *actual_mechs,
                              uint32_t *time_rec);

uint32_t gssntlm_acquire_cred_from(uint32_t *minor_status,
                                   gss_name_t desired_name,
                                   uint32_t time_req,
                                   gss_OID_set desired_mechs,
                                   gss_cred_usage_t cred_usage,
                                   gss_const_key_value_set_t cred_store,
                                   gss_cred_id_t *output_cred_handle,
                                   gss_OID_set *actual_mechs,
                                   uint32_t *time_rec);

uint32_t gssntlm_release_cred(uint32_t *minor_status,
                              gss_cred_id_t *cred_handle);

uint32_t gssntlm_import_name(uint32_t *minor_status,
                             gss_buffer_t input_name_buffer,
                             gss_OID input_name_type,
                             gss_name_t *output_name);

uint32_t gssntlm_import_name_by_mech(uint32_t *minor_status,
                                     gss_const_OID mech_type,
                                     gss_buffer_t input_name_buffer,
                                     gss_OID input_name_type,
                                     gss_name_t *output_name);

uint32_t gssntlm_duplicate_name(uint32_t *minor_status,
                                const gss_name_t input_name,
                                gss_name_t *dest_name);

uint32_t gssntlm_release_name(uint32_t *minor_status,
                              gss_name_t *input_name);

uint32_t gssntlm_init_sec_context(uint32_t *minor_status,
                                  gss_cred_id_t claimant_cred_handle,
                                  gss_ctx_id_t *context_handle,
                                  gss_name_t target_name,
                                  gss_OID mech_type,
                                  uint32_t req_flags,
                                  uint32_t time_req,
                                  gss_channel_bindings_t input_chan_bindings,
                                  gss_buffer_t input_token,
                                  gss_OID *actual_mech_type,
                                  gss_buffer_t output_token,
                                  uint32_t *ret_flags,
                                  uint32_t *time_rec);

uint32_t gssntlm_delete_sec_context(uint32_t *minor_status,
                                    gss_ctx_id_t *context_handle,
                                    gss_buffer_t output_token);


uint32_t gssntlm_context_time(uint32_t *minor_status,
                              gss_ctx_id_t context_handle,
                              uint32_t *time_rec);

uint32_t gssntlm_accept_sec_context(uint32_t *minor_status,
                                    gss_ctx_id_t *context_handle,
                                    gss_cred_id_t acceptor_cred_handle,
                                    gss_buffer_t input_token_buffer,
                                    gss_channel_bindings_t input_chan_bindings,
                                    gss_name_t *src_name,
                                    gss_OID *mech_type,
                                    gss_buffer_t output_token,
                                    uint32_t *ret_flags,
                                    uint32_t *time_rec,
                                    gss_cred_id_t *delegated_cred_handle);

#endif /* _GSS_NTLMSSP_H_ */
