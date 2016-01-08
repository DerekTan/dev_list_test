/*
 * ===========================================================================
 *
 *       Filename:  dev.h
 *
 *    Description:  head file for dev_list.c
 *
 *        Version:  1.0
 *        Created:  2015/11/18 13:54:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DerekTan, tandake@gmail.com
 *   Organization:  SKLSSL.ORG
 *
 * ===========================================================================
 */

#ifndef DEV_LIST_H
#define DEV_LIST_H

#include <stdint.h>
#include <string.h>

#define is_dev_same_short(a, b)     (a.shortAddr == b.shortAddr)
#define is_dev_same_addr(a, b)      \
    (is_dev_same_short(a, b) ? !memcmp(a.extAddr, b.extAddr, 8) : 0)
#define is_dev_same_ext(a, b)       (!memcmp(a.extAddr, b.extAddr, 8))
#define is_dev_same_exact(a, b)     \
    (is_dev_same_addr(a, b) ? ((a.grpCnt == b.grpCnt) && !memcmp(a.grpList, b.grpList, a.grpCnt*sizeof(uint16_t))) : 0)

typedef struct devData {
    uint16_t shortAddr;
    uint8_t  extAddr[8];
    uint8_t  grpCnt;
    uint16_t *grpList;
} devData_t;

typedef struct devNode {
    devData_t dev;
    struct devNode * next;
} devNode_t;

typedef struct {
    devNode_t *head;
    uint8_t num;
} devList_t;

typedef uint8_t (*devCmpFunc_t)(devData_t, devData_t);

devList_t *dev_list_create(void);
uint8_t list_dev_add(devList_t *lst, devData_t dev);
devList_t *list_dev_del(devList_t *lst, devData_t dev);
uint8_t *list_clear(devList_t *lst);
uint8_t list_dev_get_shortaddr_array(devList_t *lst, uint16_t *pArray);
devNode_t* list_find_dev_by_shortaddr (devList_t *lst, uint16_t shortaddr);
devNode_t *list_node_pop_by_shortaddr(devList_t *lst, uint16_t shortaddr);
devList_t *list_node_add(devList_t *lst, devNode_t *pNode);
devList_t *list_dev_del_by_dev_extaddr(devList_t *lst, devData_t dev);
devList_t *list_dev_del_by_dev_shortaddr(devList_t *lst, devData_t dev);
devList_t *list_dev_del_by_dev_addr(devList_t *lst, devData_t dev);
void free_node(devNode_t *pNode);
uint8_t list_dev_print(devList_t *lst);
uint8_t list_dev_get_all_groups(devList_t *lst, uint16_t *grplist, uint8_t max);
uint8_t add_group_info_to_node( devNode_t *pNode, uint16_t group );
uint8_t del_group_info_from_node( devNode_t *pNode, uint16_t group );
devNode_t *dev_node_create( uint16_t shortaddr, const uint8_t extaddr[], uint8_t grpcnt, uint16_t *grplist);
uint8_t dev_node_update ( devNode_t *pNode, uint8_t grpcnt, uint16_t *grplist );
#endif
