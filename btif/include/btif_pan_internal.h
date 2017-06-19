/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *  Filename:      btif_pan_internal.h
 *
 *  Description:   Bluetooth pan internal
 *
 ******************************************************************************/

#ifndef BTIF_PAN_INTERNAL_H
#define BTIF_PAN_INTERNAL_H

#include "bt_types.h"
#include "btif_pan.h"

/*******************************************************************************
 *  Constants & Macros
 ******************************************************************************/

#define PAN_NAP_SERVICE_NAME "Android Network Access Point"
#define PANU_SERVICE_NAME "Android Network User"
#define TAP_IF_NAME "bt-pan"
#define TAP_MAX_PKT_WRITE_LEN 2000
#ifndef PAN_SECURITY
#define PAN_SECURITY                                                         \
  (BTM_SEC_IN_AUTHENTICATE | BTM_SEC_OUT_AUTHENTICATE | BTM_SEC_IN_ENCRYPT | \
   BTM_SEC_OUT_ENCRYPT)
#endif

#define PAN_STATE_UNKNOWN 0
#define PAN_STATE_OPEN 1
#define PAN_STATE_CLOSE 2
#ifndef PAN_ROLE_INACTIVE
#define PAN_ROLE_INACTIVE 0
#endif

/*******************************************************************************
 *  Type definitions and return values
 ******************************************************************************/

typedef struct eth_hdr {
  bt_bdaddr_t h_dest;
  bt_bdaddr_t h_src;
  short h_proto;
} tETH_HDR;

typedef struct {
  int handle;
  int state;
  uint16_t protocol;
  bt_bdaddr_t peer;
  int local_role;
  int remote_role;
  bt_bdaddr_t eth_addr;
} btpan_conn_t;

typedef struct {
  int btl_if_handle;
  int btl_if_handle_panu;
  int tap_fd;
  int enabled;
  int open_count;
  int flow;  // 1: outbound data flow on; 0: outbound data flow off
  btpan_conn_t conns[MAX_PAN_CONNS];
  int congest_packet_size;
  unsigned char congest_packet[1600];  // max ethernet packet size
} btpan_cb_t;

/*******************************************************************************
 *  Functions
 ******************************************************************************/

extern btpan_cb_t btpan_cb;
btpan_conn_t* btpan_new_conn(int handle, const bt_bdaddr_t& addr,
                             int local_role, int peer_role);
btpan_conn_t* btpan_find_conn_addr(const bt_bdaddr_t& addr);
btpan_conn_t* btpan_find_conn_handle(uint16_t handle);
void btpan_set_flow_control(bool enable);
int btpan_get_connected_count(void);
int btpan_tap_open(void);
void create_tap_read_thread(int tap_fd);
void destroy_tap_read_thread(void);
int btpan_tap_close(int tap_fd);
int btpan_tap_send(int tap_fd, const bt_bdaddr_t& src, const bt_bdaddr_t& dst,
                   uint16_t protocol, const char* buff, uint16_t size, bool ext,
                   bool forward);

static inline int is_empty_eth_addr(const bt_bdaddr_t& addr) {
  return addr == bd_addr_empty;
}

static inline int is_valid_bt_eth_addr(const bt_bdaddr_t& addr) {
  if (is_empty_eth_addr(addr)) return 0;
  return addr.address[0] & 1 ? 0 : 1; /* Cannot be multicasting address */
}

#endif
