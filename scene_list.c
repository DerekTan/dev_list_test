/*
 * ===========================================================================
 *
 *       Filename:  scene_list.c
 *
 *    Description:  scene management
 *
 *        Version:  1.0
 *        Created:  2015/11/30 9:08:20
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
#include "scene_list.h"

static void scene_list_free_node ( sceneNode_t *pNode );
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  scene_list_create
 *  Description:  create the scene list
 * =====================================================================================
 */
sceneList_t *scene_list_create ( void )
{
    sceneList_t *lst;
    lst = (sceneList_t *)malloc(sizeof(sceneList_t));
    if (lst) {
        lst->head = NULL;
        lst->num = 0;
    }
    return lst;
}		/* -----  end of function scene_list_create  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  scene_list_find_scene
 *  Description:  find the position of given scene id
 * =====================================================================================
 */
sceneNode_t *scene_list_find_scene( sceneList_t *lst, uint8_t sceneid )
{
    sceneNode_t *p = NULL;
    p = lst->head;
    while ( p ) {
        if ( p->id == sceneid ) {
            break;
        }
        p = p->next;
    }
    return p;
}		/* -----  end of function scene_list_find_scene  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  add_group_to_scene_node
 *  Description:
 * =====================================================================================
 */
uint8_t add_group_to_scene_node(sceneNode_t *pNode, uint16_t grpid)
{
    uint16_t *pNewGrpList;
    uint8_t i;
    for ( i=0; i<pNode->grpCnt; i++) {
        if (pNode->grpList[i] == grpid) { //already in group list
            return 0;
        }
    }
    /* group not in group list, add it */
    pNode->grpCnt++;
    pNewGrpList = realloc( pNode->grpList, pNode->grpCnt * sizeof(uint16_t) );
    if ( pNewGrpList ) { //not NULL
        pNode->grpList = pNewGrpList;
        pNode->grpList[pNode->grpCnt - 1] = grpid;
        return 0;
    }
    else { // fail to realloc
        pNode->grpCnt--;
        return 1;
    }
}		/* -----  end of function add_group_to_scene_node  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  del_group_from_scene_node
 *  Description:
 * =====================================================================================
 */
uint8_t del_group_from_scene_node(sceneNode_t *pNode, uint16_t grpid)
{
    uint16_t *pNewGrpList = NULL;
    uint8_t i;
    for ( i=0; i<pNode->grpCnt; i++) {
        if (pNode->grpList[i] == grpid) { //in group list
            /* group in group list, delete it */
            pNode->grpCnt--;
            if (pNode->grpCnt != 0) {
                pNewGrpList = malloc( pNode->grpCnt * sizeof(uint16_t) );
                if ( NULL == pNewGrpList ) { // fail
                    pNode->grpCnt++;
                    return 1;
                }
                else {
                    memcpy( pNewGrpList, pNode->grpList, i*sizeof(uint16_t) );
                    memcpy( pNewGrpList+i, pNode->grpList+i+1, (pNode->grpCnt-i)*sizeof(uint16_t) );
                    free(pNode->grpList);
                    pNode->grpList = pNewGrpList;
                    return 0;
                }
            }
            else { //grpCnt is 0 now, delete the node?
                free(pNode->grpList);
                pNode->grpList = NULL;
                return 0;
            }
        }
    }
    return 1; // group not found
}		/* -----  end of function del_group_from_scene_node  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  is_scene_contains_group
 *  Description:
 * =====================================================================================
 */
uint8_t is_scene_contains_group(sceneNode_t *pNode, uint16_t grpid)
{
    uint8_t i;
    for ( i=0; i<pNode->grpCnt; i++) {
        if (pNode->grpList[i] == grpid) { //in group list
            return 1;
        }
    }
    return 0;
}		/* -----  end of function is_scene_contains_group  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  create_scene_node
 *  Description:
 * =====================================================================================
 */
sceneNode_t *create_scene_node ( uint8_t sceneid, uint16_t grpid )
{
    sceneNode_t *pNewNode = NULL;
    uint16_t *grpList = NULL;
    pNewNode = (sceneNode_t *)malloc(sizeof(sceneNode_t));
    if (pNewNode) {
        grpList = malloc(sizeof(uint16_t));
        if (grpList == NULL) {
            free(pNewNode);
            pNewNode = NULL;
        }
        else {
            grpList[0] = grpid;
            pNewNode->id = sceneid;
            pNewNode->grpCnt = 1;
            pNewNode->grpList = grpList;
        }
    }
    return pNewNode;
}		/* -----  end of function create_scene_node  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  scene_list_add_node
 *  Description:
 * =====================================================================================
 */
sceneList_t *scene_list_add_node(sceneList_t *lst, sceneNode_t *pNode)
{
    if (pNode) {
        pNode->next = lst->head;
        lst->head = pNode;
        lst->num++;
    }
    return lst;
}		/* -----  end of function scene_list_add_node  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  scene_list_del_scene
 *  Description:
 * =====================================================================================
 */
sceneList_t *scene_list_del_scene( sceneList_t *lst, uint8_t sceneid )
{
    sceneNode_t *pPre, *pNext, *pDel = NULL;
    if ( lst->head ) {
        if (lst->head->id == sceneid) {
            pDel = lst->head;
            lst->head = pDel->next;
        }
        else {
            pPre = lst->head;
            pNext = pPre->next;
            while (pNext) {
                if ( pNext->id == sceneid ) {
                    pDel = pNext;
                    pNext = pNext->next;
                    pPre->next = pNext;
                    break;
                }
                pPre = pNext;
                pNext = pPre->next;
            }
        }
        if (pDel) {
            scene_list_free_node(pDel);
            lst->num--;
        }
    }
    return lst;
}		/* -----  end of function scene_list_del_node  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  scene_list_free_node
 *  Description:
 * =====================================================================================
 */
static void scene_list_free_node ( sceneNode_t *pNode )
{
    if (pNode->grpCnt) {
        free(pNode->grpList);
    }
    free(pNode);
}		/* -----  end of function scene_list_free_node  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  scene_list_del_group_from_all_scene
 *  Description:
 * =====================================================================================
 */
uint8_t scene_list_del_group_from_all_scene ( sceneList_t *lst, uint16_t grpid )
{
    sceneNode_t *p = NULL;
    uint8_t ret = 1;
    p = lst->head;
    while ( p ) {
        if ( is_scene_contains_group(p, grpid) ) {
            ret += del_group_from_scene_node(p, grpid);
        }
        p = p->next;
    }
    return ret;
}		/* -----  end of function scene_list_del_group_from_all_scene  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  scene_list_strip
 *  Description:  remove empty scene
 * =====================================================================================
 */
void scene_list_strip ( sceneList_t *lst )
{
    sceneNode_t *pPre, *pNext, *pDel;
    pDel = NULL;
    while (NULL != lst->head && lst->head->grpCnt == 0) {
        pDel = lst->head;
        lst->head = pDel->next;

        scene_list_free_node(pDel);
        lst->num--;
    }
    if ( lst->head ) {
        pPre = lst->head;
        pNext = pPre->next;
        while (pNext) {
            if ( pNext->grpCnt == 0 ) {
                pDel = pNext;
                scene_list_free_node(pDel);
                lst->num--;

                pNext = pNext->next;
                pPre->next = pNext;
                continue;
            }
            pPre = pNext;
            pNext = pPre->next;
        }
    }
}		/* -----  end of function scene_list_strip  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  scene_list_print
 *  Description:
 * =====================================================================================
 */
uint8_t scene_list_print ( sceneList_t *lst )
{
    int i, j;
    sceneNode_t *p;
    printf("========== total %d scene(s) ==========\n", lst->num);

    if (lst->num) {
        i = 0;
        p = lst->head;
        while ( p ) {
            printf("scene %d:\n", i++);
            printf("\tscene id:%d\n",p->id);
            printf("\tgrpCnt:%d\n",p->grpCnt);
            if (p->grpCnt) {
                printf("\tgrpList:");
                for (j=0; j<p->grpCnt-1; j++) {
                    printf("%3d,",p->grpList[j]);
                }
                printf("%3d",p->grpList[j]);
                printf ("\n");
            }
            p = p->next;
        }
    }
    printf("========== end print scene(s) ==========\n");
    return 0;
}		/* -----  end of function scene_list_print  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  scene_list_get_all_scene_id
 *  Description:
 * =====================================================================================
 */
uint8_t scene_list_get_all_scene_id ( sceneList_t *lst, uint8_t *idlst, uint8_t max )
{
    uint8_t num;
    uint8_t i;
    sceneNode_t *p;
    num = lst->num > max ? max : lst->num;
    p = lst->head;
    for ( i=0; i<num; i++ ) {
        if (p == NULL) { // should not runs here
            return 0;
        }
        idlst[i] = p->id;
        p = p->next;
    }
    return num;
}		/* -----  end of function scene_list_get_all_scene_id  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  scene_list_update_scenes_for_group
 *  Description:
 *       Return:  0 - sucess
 *                1 - error occurs
 * =====================================================================================
 */
uint8_t scene_list_update_scenes_for_group ( sceneList_t *lst, uint16_t grpid, uint8_t *scnlst, uint8_t num )
{
    uint8_t ret = 0;
    sceneNode_t *pNode;
    uint8_t *pid;
    uint8_t *tmplst;
    tmplst = malloc(num);
    if (tmplst == NULL) {
        return 1; /* fail */
    }
    memcpy(tmplst, scnlst, num);
    pNode = lst->head;
    while (pNode) {
        if ( NULL != ( pid=memchr(tmplst, pNode->id, num) ) ) {
            add_group_to_scene_node(pNode, grpid);
            /* remove this scene id from the scenes table */
            if ( pid != tmplst + num - 1 ) {
                /* copy the last one to current position,
                 * then throw the tail away */
                *pid = *(tmplst + num - 1);
            }
            num--;
        }
        else {
            del_group_from_scene_node(pNode, grpid);
        }
        pNode = pNode->next;
    }
    while (num != 0) {
        pNode = create_scene_node( tmplst[num-1], grpid);
        if ( NULL != pNode ) {
            scene_list_add_node( lst, pNode);
        }
        else {
            ret = 1;
        }
        num--;
    }
    free(tmplst);
    return ret;
}		/* -----  end of function scene_list_update_scenes_for_group  ----- */
