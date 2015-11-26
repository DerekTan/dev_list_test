/*
 * ===========================================================================
 *
 *       Filename:  dev_list_app.h
 *
 *    Description:  provide api for device list operation
 *
 *        Version:  1.0
 *        Created:  2015/11/24 11:15:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DerekTan (TDK), tandake@gmail.com
 *   Organization:  SKLSSL.ORG
 *
 * ===========================================================================
 */
#ifndef DEV_LIST_APP_H
#define DEV_LIST_APP_H

#include <stdint.h>
#include <stddef.h>

extern const uint8_t INVALID_EXTADDR[];
#define is_dev_invalid_ext(a)       (!memcmp(INVALID_EXTADDR, a, 8))

uint8_t create_online_list(void);
uint8_t create_offline_list(void);
uint8_t add_online_dev( uint16_t shortaddr, uint8_t extaddr[] );
uint8_t add_offline_dev( uint16_t shortaddr );
uint8_t get_dev_extaddr( uint16_t shortaddr, uint8_t extaddr[] );
uint8_t add_dev_group_info( uint16_t shortaddr, uint8_t grpcnt, uint16_t *grplist );
uint8_t del_dev_group_info( uint16_t shortaddr, uint16_t grpid );
uint8_t get_group_member( uint16_t grpid, uint16_t *paddrlist );
uint8_t get_online_devices(uint16_t *pArray);

uint8_t get_all_groups(uint16_t *grplist, uint8_t max);
uint8_t del_all_groups(uint8_t addrMode, uint16_t addr);

uint8_t offline_dev_print(void);
uint8_t online_dev_print(void);
#endif
