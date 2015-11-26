/*
 * ===========================================================================
 *
 *       Filename:  dev_list_app.c
 *
 *    Description:  provide dev_list api to app layer
 *
 *        Version:  1.0
 *        Created:  2015/11/23 15:39:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DerekTan (TDK), tandake@gmail.com
 *   Organization:  SKLSSL.ORG
 *
 * ===========================================================================
 */

#include <stdlib.h>
#include <string.h>
#include "dev_list.h"
#include "dev_list_app.h"

const uint8_t INVALID_EXTADDR[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

devList_t *g_onlineList = NULL;
devList_t *g_offlineList = NULL;

uint8_t create_online_list(void)
{
    if (NULL == g_onlineList)
        g_onlineList = dev_list_create();
    if (g_onlineList)
        return 0;
    return 1;
}

uint8_t create_offline_list(void)
{
    if (NULL == g_offlineList)
        g_offlineList = dev_list_create();
    if (g_offlineList)
        return 0;
    return 1;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  add_online_dev
 *  Description:  add device to online list
 *       Return:  0 - success
 *                1 - fail
 * =====================================================================================
 */
uint8_t add_online_dev( uint16_t shortaddr, uint8_t extaddr[] )
{
    devNode_t *pnode;
    devData_t dev;

    /* generate a new node */
    dev.shortAddr = shortaddr;
    memcpy(dev.extAddr, extaddr, 8);
    dev.grpCnt = 0;
    dev.grpList = NULL;

    if ( is_dev_invalid_ext(extaddr) ) { //ignore extaddr
        if ( list_find_dev_by_shortaddr(g_onlineList, shortaddr) ) {
            //already in online device list, do nothing
            return 0;
        }
        if ( NULL != (pnode = list_node_pop_by_shortaddr(g_offlineList, shortaddr)) ) {
            //in offline device list
            list_node_add(g_onlineList, pnode);
            return 0;
        }
    }

    /* check extAddr */
    if ( NULL != (pnode = list_find_dev_by_shortaddr(g_onlineList, shortaddr)) ) {
        //short address found in online device list
        if ( !memcmp(pnode->dev.extAddr, extaddr, 8) ) {
            //exactly the same device, do nothing
            return 0;
        }
    }
    if ( NULL != (pnode = list_node_pop_by_shortaddr(g_offlineList, shortaddr)) ) {
        //short address found in offline device list
        if ( !memcmp(pnode->dev.extAddr, extaddr, 8) ) {
            //exactly the same, move to online device list
            list_node_add(g_onlineList, pnode);
            return 0;
        }
        // the node popped but not used, free it
        free_node(pnode);
    }

    // short address not found either in online or offline device list,
    // or found but the coordinate extend address is not match.

    // delete the node with the same shortaddr or extaddr
    list_dev_del_by_dev_shortaddr(g_onlineList, dev);
    list_dev_del_by_dev_extaddr(g_onlineList, dev);
    list_dev_del_by_dev_shortaddr(g_offlineList, dev);
    list_dev_del_by_dev_extaddr(g_offlineList, dev);
    // create new node then add to online device list
    return list_dev_add(g_onlineList, dev);
}		/* -----  end of function add_online_dev  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  add_offline_dev
 *  Description:  add device to offline list
 *       Return:  0 - success
 *                1 - fail
 * =====================================================================================
 */
uint8_t add_offline_dev ( uint16_t shortaddr )
{
    devNode_t *pnode;
    devData_t dev;

    /* generate a new node */
    dev.shortAddr = shortaddr;
    memcmp(dev.extAddr, INVALID_EXTADDR, 8);
    dev.grpCnt = 0;
    dev.grpList = NULL;

    /* pop the node from online device list */
    if ( NULL != (pnode = list_node_pop_by_shortaddr(g_onlineList, shortaddr)) ) {
        //in offline device list
        list_node_add(g_offlineList, pnode);
        return 0;
    }
    return 0;
}		/* -----  end of function add_offline_dev  ----- */

static uint8_t add_group_info_to_node( devNode_t *pNode, uint16_t group )
{
    uint16_t *pNewGrpList;
    uint8_t i;
    for ( i=0; i<pNode->dev.grpCnt; i++) {
        if (pNode->dev.grpList[i] == group) { //already in group list
            return 0;
        }
    }
    /* group not in group list, add it */
    pNode->dev.grpCnt++;
    pNewGrpList = realloc( pNode->dev.grpList, pNode->dev.grpCnt * sizeof(uint16_t) );
    if ( pNewGrpList ) { //not NULL
        pNode->dev.grpList = pNewGrpList;
        pNode->dev.grpList[pNode->dev.grpCnt - 1] = group;
        return 0;
    }
    else { // fail to realloc
        pNode->dev.grpCnt--;
        return 1;
    }
}
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  add_dev_group_info
 *  Description:  add a single group to the device
 *       Return:  0 - success
 *                1 - fail
 *    Attention:  caller should provide the memory space for grplist
 *    =====================================================================================
 */
uint8_t add_dev_group_info ( uint16_t shortaddr, uint16_t group )
{
    devNode_t *pNode;
    uint16_t *pNewGrpList;
    uint8_t i;
    if ( NULL != (pNode = list_find_dev_by_shortaddr(g_onlineList, shortaddr)) ) {
        return add_group_info_to_node( pNode, group );
    }

    /* check offline -- is this a must? */
    if ( NULL != (pNode = list_find_dev_by_shortaddr(g_offlineList, shortaddr)) ) {
        return add_group_info_to_node( pNode, group );
    }
    /* short address not found, return fail */
    return 1;
}		/* -----  end of function add_dev_group_info  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  update_dev_group_info
 *  Description:  update group info to the device (update if already exist)
 *       Return:  0 - success
 *                1 - fail
 *    Attention:  caller should provide the memory space for grplist
 *    =====================================================================================
 */
uint8_t update_dev_group_info ( uint16_t shortaddr, uint8_t grpcnt, uint16_t *grplist )
{
    devNode_t *pNode;
    uint16_t *newGpLst;
    newGpLst = (uint16_t *)malloc( grpcnt * sizeof(uint16_t) );
    if ( NULL == newGpLst )
        return 1;
    memcpy( newGpLst, grplist, grpcnt * sizeof(uint16_t) );

    if ( NULL != (pNode = list_find_dev_by_shortaddr(g_onlineList, shortaddr)) ) {
        if (pNode->dev.grpCnt != 0) {
            free(pNode->dev.grpList);
            //pNode->dev.grpList = NULL;
        }
        pNode->dev.grpCnt = grpcnt;
        pNode->dev.grpList = newGpLst;
        return 0;
    }

    /* check offline -- is this a must? */
    if ( NULL != (pNode = list_find_dev_by_shortaddr(g_offlineList, shortaddr)) ) {
        if (pNode->dev.grpCnt != 0) {
            free(pNode->dev.grpList);
            //pNode->dev.grpList = NULL;
        }
        pNode->dev.grpCnt = grpcnt;
        pNode->dev.grpList = newGpLst;
        return 0;
    }
    return 1;
}		/* -----  end of function update_dev_group_info  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_group_member
 *  Description:  return the short address list which blongs the provided group
 *       Return:  the number of short addresses
 * =====================================================================================
 */
uint8_t get_group_member ( uint16_t grpid, uint16_t *paddrlist )
{
    uint8_t num = 0;
    devNode_t *p;
    uint8_t i;
    devList_t *lst = g_onlineList;

    p = lst->head;
    while ( p ) {
        if ( p->dev.grpCnt ) { //if the node blongs to any group
            for ( i=0; i<p->dev.grpCnt; i++ ) {
                if (p->dev.grpList[i] == grpid) {
                    paddrlist[num++] = p->dev.shortAddr;
                }
            }
        }
        p = p->next;
    }
    return num;
}		/* -----  end of function get_group_member  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_online_devices
 *  Description:  get short addresses of online devices
 *       Return:  the len of the array
 *    Attention:  the caller should provide enough space to be filled
 * =====================================================================================
 */
uint8_t get_online_devices(uint16_t *pArray)
{
    return list_dev_get_shortaddr_array(g_onlineList, pArray);
}		/* -----  end of function get_online_devices  ----- */

uint8_t online_dev_print(void)
{
    return list_dev_print(g_onlineList);
}

uint8_t offline_dev_print(void)
{
    return list_dev_print(g_offlineList);
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  del_dev_group_info
 *  Description:  get all groups
 * =====================================================================================
 */
uint8_t del_dev_group_info( uint16_t shortaddr, uint16_t grpid )
{
    return 0;
}		/* -----  end of function del_dev_group_info  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_dev_extaddr
 *  Description:
 * =====================================================================================
 */
uint8_t get_dev_extaddr( uint16_t shortaddr, uint8_t extaddr[] )
{
    return 0;
}		/* -----  end of function get_dev_extaddr  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_all_groups
 *  Description:
 * =====================================================================================
 */
uint8_t get_all_groups(uint16_t *grplist, uint8_t max)
{
    return 0;
}		/* -----  end of function get_all_groups  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  del_all_groups
 *  Description:
 * =====================================================================================
 */
uint8_t del_all_groups(uint8_t addrMode, uint16_t addr)
{
    return 0;
}		/* -----  end of function del_all_groups  ----- */
