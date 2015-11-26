/*
 * ===========================================================================
 *
 *       Filename:  dev_list.c
 *
 *    Description:  maintain device list
 *
 *        Version:  1.0
 *        Created:  2015/11/18 15:14:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DerekTan (TDK), tandake@gmail.com
 *   Organization:  SKLSSL.ORG
 *
 * ===========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "dev_list.h"

static uint8_t dev_cmp_by_addr(devData_t a, devData_t b);
static uint8_t dev_cmp_by_shortaddr(devData_t a, devData_t b);
static uint8_t dev_cmp_by_extaddr(devData_t a, devData_t b);
static devList_t *list_dev_del_by_dev_func(devList_t *lst, devData_t dev, devCmpFunc_t func);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  dev_list_create
 *  Description:  create the device list,
 *                must be called before any device list operation
 *       Return:  the created list
 * =====================================================================================
 */
devList_t *dev_list_create(void)
{
    devList_t *lst;
    lst = (devList_t *)malloc(sizeof(devList_t));
    if (lst) {
        lst->head = NULL;
        lst->num = 0;
    }
    return lst;
}

/*
 * ===  FUNCTION  ============================================================
 *         Name:  list_dev_get_shortaddr_array
 *  Description:  get short addresses array from a list
 *       Return:  the len of the array
 *    Attention:  the caller should provide enough space to be filled
 * ===========================================================================
 */
uint8_t list_dev_get_shortaddr_array(devList_t *lst, uint16_t *pArray)
{
    devNode_t *p;
    if (lst->num) {
        p = lst->head;
        while ( p ) {
            *pArray++ = p->dev.shortAddr;
            p = p->next;
        }
    }
    return lst->num;
}

uint8_t list_dev_add(devList_t *lst, devData_t dev)
{
    devNode_t *pNode = (devNode_t *)malloc(sizeof(devNode_t));
    if (pNode != NULL) {
        pNode->dev = dev;
        list_node_add(lst, pNode);
        return 0;
    }
    return 1;
}

devList_t *list_dev_del_by_dev_addr(devList_t *lst, devData_t dev)
{
    return list_dev_del_by_dev_func(lst, dev, dev_cmp_by_addr);
}

devList_t *list_dev_del_by_dev_shortaddr(devList_t *lst, devData_t dev)
{
    return list_dev_del_by_dev_func(lst, dev, dev_cmp_by_shortaddr);
}

devList_t *list_dev_del_by_dev_extaddr(devList_t *lst, devData_t dev)
{
    return list_dev_del_by_dev_func(lst, dev, dev_cmp_by_extaddr);
}

/*
 * ===  FUNCTION  ============================================================
 *         Name:  list_dev_del_by_dev_func
 *  Description:  delete all the nodes that match from list.
 * ===========================================================================
 */
static devList_t *list_dev_del_by_dev_func(devList_t *lst, devData_t dev, devCmpFunc_t cmp_func)
{
    devNode_t *pPre, *pNext, *pDel;
    pDel = NULL;
    while (NULL != lst->head && cmp_func(lst->head->dev, dev)) {
        pDel = lst->head;
        lst->head = pDel->next;

        free_node(pDel);
        lst->num--;
    }
    if ( lst->head ) {
        pPre = lst->head;
        pNext = pPre->next;
        while (pNext) {
            if ( cmp_func(pNext->dev, dev) ) {
                pDel = pNext;
                free_node(pDel);
                lst->num--;

                pNext = pNext->next;
                pPre->next = pNext;
                continue;
            }
            pPre = pNext;
            pNext = pPre->next;
        }
    }
    return lst;
}

devNode_t* list_find_dev_by_shortaddr (devList_t *lst, uint16_t shortaddr)
{
    devNode_t *p;

    p = lst->head;
    while ( p ) {
        if ( p->dev.shortAddr ==  shortaddr ) {
            break;
        }
        p = p->next;
    }
    return p;
}

uint8_t list_dev_shift(devList_t *srcLst, devList_t *dstLst, devData_t dev)
{
    devNode_t *pNode;
    pNode = list_node_pop_by_shortaddr(srcLst, dev.shortAddr);
    if (pNode == NULL) { //dev not found in srcLst
        return 1;
    }
    list_node_add(dstLst, pNode);
    return 0;
}

uint8_t *list_clear(devList_t *lst)
{
    devNode_t *p;
    p = lst->head;
    while ( p ) {
        lst->head = p->next;
        free_node(p);
        p = lst->head;
    }
    lst->num = 0;
    return 0;
}

devList_t *list_node_add(devList_t *lst, devNode_t *pNode)
{
    if (pNode) {
        pNode->next = lst->head;
        lst->head = pNode;
        lst->num++;
    }
    return lst;
}

/*
 * ===  FUNCTION  ============================================================
 *         Name:  list_node_pop_by_shortaddr
 *  Description:  pop the first node that matches the shortaddr from list.
 * ===========================================================================
 */
devNode_t *list_node_pop_by_shortaddr(devList_t *lst, uint16_t shortaddr)
{
    devNode_t *pPre, *pNext, *pRet;
    pRet = NULL;
    pPre = pNext = lst->head;
    if ( lst->head ) {
        if ( pNext->dev.shortAddr == shortaddr ) {
            pRet = lst->head;
            lst->head = lst->head->next;
        }
        else {
            pNext = pNext->next;
            while (pNext) {
                if ( pNext->dev.shortAddr == shortaddr ) {
                    pRet = pNext;
                    pNext = pNext->next;
                    pPre->next = pNext;
                    break;
                }
                pPre = pNext;
                pNext = pNext->next;
            }
        }
        if ( pRet ) {
            lst->num--;
        }
    }
    return pRet;
}

void free_node(devNode_t *pNode)
{
    if (pNode->dev.grpCnt) {
        free(pNode->dev.grpList);
        //pNode->dev.grpList = NULL;
    }
    free(pNode);
}

uint8_t list_dev_print(devList_t *lst)
{
    int i, j;
    devNode_t *p;
    printf("==========list has %d nodes==========\n", lst->num);

    if (lst->num) {
        i = 0;
        p = lst->head;
        while ( p ) {
            printf("node %d:\n", i++);
            printf("\tshortAddr:%d\n",p->dev.shortAddr);
            printf("\textAddr:0x");
            for (j=0; j<8; j++)
                printf("%02x",p->dev.extAddr[j]);
            printf("\n");
            printf("\tgrpCnt:%d\n",p->dev.grpCnt);
            if (p->dev.grpCnt) {
                printf("\tgrpList:");
                for (j=0; j<p->dev.grpCnt-1; j++) {
                    printf("%3d,",p->dev.grpList[j]);
                }
                printf("%3d",p->dev.grpList[j]);
                printf ("\n");
            }
            p = p->next;
        }
    }
    printf("==========end print list==========\n");
    return 0;
}

static uint8_t dev_cmp_by_addr(devData_t a, devData_t b)
{
    return is_dev_same_addr(a, b);
}

static uint8_t dev_cmp_by_shortaddr(devData_t a, devData_t b)
{
    return is_dev_same_short(a, b);
}

static uint8_t dev_cmp_by_extaddr(devData_t a, devData_t b)
{
    return is_dev_same_ext(a, b);
}
