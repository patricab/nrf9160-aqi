/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#if defined(CONFIG_LWM2M_DTLS_SUPPORT)
static char client_psk[] = "234aec5efc67aecf576c5aef765fc76e"; //pre shared key
#define SERVER_TLS_TAG 35724861
#define BOOTSTRAP_TLS_TAG 35724862
#endif /* defined(CONFIG_LWM2M_DTLS_SUPPORT) */
