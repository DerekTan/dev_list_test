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

uint8_t list_dev_get_dev_num(devList_t *lst)
{
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

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  dev_node_create
 *  Description:
 * =====================================================================================
 */
devNode_t *dev_node_create( uint16_t shortaddr, const uint8_t extaddr[], uint8_t grpcnt, uint16_t *grplist)
{
    devNode_t *pNode = NULL;
    uint16_t *newGpLst = NULL;
    pNode = malloc(sizeof(devNode_t));
    if (pNode) {
        if (grpcnt) {
            newGpLst = (uint16_t *)malloc( grpcnt * sizeof(uint16_t) );
            if ( NULL == newGpLst ) {
                free(pNode);
                return NULL;
            }
        }
        else {
            newGpLst = NULL;
        }
        pNode->dev.shortAddr = shortaddr;
        memcpy(pNode->dev.extAddr, extaddr, 8);
        pNode->dev.grpCnt = grpcnt;
        pNode->dev.grpList = newGpLst;
    }
    return pNode;
}		/* -----  end of function dev_node_create  ----- */

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
 * ===  FUNCTION  ======================================================================
 *         Name:  dev_node_update
 *  Description:
 * =====================================================================================
 */
uint8_t dev_node_update ( devNode_t *pNode, uint8_t grpcnt, uint16_t *grplist )
{
    uint16_t *newGpLst;
    if (pNode) {
        if ( grpcnt ) {
            newGpLst = (uint16_t *)malloc( grpcnt * sizeof(uint16_t) );
            if ( NULL == newGpLst )
                return 1;
            memcpy( newGpLst, grplist, grpcnt * sizeof(uint16_t) );
        }
        else {
            newGpLst = NULL;
        }
        if (pNode->dev.grpCnt != 0) {
            free(pNode->dev.grpList);
        }
        pNode->dev.grpCnt = grpcnt;
        pNode->dev.grpList = newGpLst;
        return 0;
    }
    return 1;
}		/* -----  end of function dev_node_update  ----- */
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

/*
 * ===  FUNCTION  ============================================================
 *         Name:  bsearch
 *  Description:
 *       Return:  1 - found
 *                0 - not found
 * ===========================================================================
 */
int8_t u16_bsearch(uint16_t *array, uint8_t len, uint16_t item)
{
    int16_t left, right, mid;
    left = 0;
    right = len - 1;
    while (left <= right) {
        mid = (left + right) >> 1;
        if (array[mid] > item) {
            right = mid - 1;
        }
        else if (array[mid] < item) {
            left = mid + 1;
        }
        else {
            return 1; //found
        }
    }
    return 0; //not found
}

void u16_insert( uint16_t *array, uint8_t len, uint16_t item )
{
    int16_t i;
    for ( i=len-1; i>=0; i--) {
        if (array[i] > item) {
            array[i+1] = array[i];
        }
        else {
            break;
        }
    }
    array[i+1] = item;
}

/*
 * ===  FUNCTION  ============================================================
 *         Name:  merge_group_list
 *  Description:  merge two list into one
 *       Return:  the len of merged list
 * ===========================================================================
 */
static uint8_t merge_group_list(uint16_t *target, uint8_t t_num, uint16_t *src, uint8_t s_num, uint8_t max)
{
    uint8_t i = 0;
    for ( i=0; i<s_num; i++ ) {
        if ( u16_bsearch(target, t_num, src[i] ) ) {
            continue;
        }
        else {
            u16_insert(target, t_num, src[i]);
            if ( ++t_num == max )
                break;
        }
    }
    return t_num;
}

uint8_t list_dev_get_all_groups(devList_t *lst, uint16_t *grplist, uint8_t max)
{
    uint8_t i=0;
    devNode_t *p;

    if (lst->num) {
        i = 0;
        p = lst->head;
        while ( p ) {
            if ( p->dev.grpCnt ) {
                i = merge_group_list(grplist, i, p->dev.grpList, p->dev.grpCnt, max);
            }
            p = p->next;
        }
    }
    return i;
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


/*
 * ===  FUNCTION  ============================================================
 *         Name:  add_group_info_to_node
 *  Description:  add a group to the node's group list
 * ===========================================================================
 */
uint8_t add_group_info_to_node( devNode_t *pNode, uint16_t group )
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
 * ===  FUNCTION  ============================================================
 *         Name:  del_group_info_from_node
 *  Description:  delete a group from the node's group list
 * ===========================================================================
 */
uint8_t del_group_info_from_node( devNode_t *pNode, uint16_t group )
{
    uint16_t *pNewGrpList = NULL;
    uint8_t i;
    for ( i=0; i<pNode->dev.grpCnt; i++) {
        if (pNode->dev.grpList[i] == group) { //in group list
            /* group in group list, delete it */
            pNode->dev.grpCnt--;
            if (pNode->dev.grpCnt != 0) {
                pNewGrpList = malloc( pNode->dev.grpCnt * sizeof(uint16_t) );
                if ( NULL == pNewGrpList ) { // fail
                    pNode->dev.grpCnt++;
                    return 1;
                }
                else {
                    memcpy( pNewGrpList, pNode->dev.grpList, i*sizeof(uint16_t) );
                    memcpy( pNewGrpList+i, pNode->dev.grpList+i+1, (pNode->dev.grpCnt-i)*sizeof(uint16_t) );
                    free(pNode->dev.grpList);
                    pNode->dev.grpList = pNewGrpList;
                    return 0;
                }
            }
            else {
                    free(pNode->dev.grpList);
                    pNode->dev.grpList = NULL;
                    return 0;
            }
        }
    }
    return 1; // group not found
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  mov_dev_list
 *  Description:  move src list to target list:
 *                  append src list to target list
 *                  clear src list
 * =====================================================================================
 */
void mov_dev_list(devList_t *t_lst, devList_t *s_lst)
{
    devNode_t *ptemp;
    if ( s_lst->num != 0 ) { /* s_lst not empty */
        ptemp = t_lst->head;
        if ( ptemp ) { /* t_lst not empty */
            while ( ptemp->next ) {
                ptemp = ptemp->next;
            }
            ptemp->next = s_lst->head;
        }
        else { /* t_lst empty */
            t_lst->head = s_lst->head;
        }
        t_lst->num += s_lst->num;

        s_lst->head = NULL;
        s_lst->num = 0;
    }
}		/* -----  end of function mov_dev_list  ----- */
