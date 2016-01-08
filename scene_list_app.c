/*
 * ===========================================================================
 *
 *       Filename:  scene_list_app.c
 *
 *    Description:  provide scene api
 *
 *        Version:  1.0
 *        Created:  2015/11/30 9:43:00
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
#include "scene_list_app.h"

sceneList_t *g_sceneList = NULL;

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  create_scene_list
 *  Description:
 * =====================================================================================
 */
uint8_t create_scene_list ( void )
{
    if (g_sceneList == NULL)
        g_sceneList = scene_list_create();
    if (g_sceneList) return 0;
    return 1;
}		/* -----  end of function create_scene_list  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  add_group_to_scene
 *  Description:  add a group id to a scene
 * =====================================================================================
 */
uint8_t add_group_to_scene(uint8_t sceneid, uint16_t grpid)
{
    sceneNode_t *pnode;
    pnode = scene_list_find_scene(g_sceneList, sceneid);
    if (pnode) {
        return add_group_to_scene_node(pnode, grpid);
    }
    else {
        pnode = create_scene_node(sceneid, grpid);
        if (pnode) {
            scene_list_add_node(g_sceneList, pnode);
        }
    }
    return 1;
}		/* -----  end of function add_group_to_scene  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  del_group_from_scene
 *  Description:
 * =====================================================================================
 */
uint8_t del_group_from_scene(uint8_t sceneid, uint16_t grpid)
{
    sceneNode_t *p = NULL;
    p = scene_list_find_scene(g_sceneList, sceneid);
    if (p) {
        return del_group_from_scene_node(p, grpid);
    }
    return 1; //not found
}		/* -----  end of function del_group_from_scene  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_scene_membership
 *  Description:  get the group IDs blong to a scene.
 *       Return:  the actual number of returned group IDs
 * =====================================================================================
 */
uint8_t get_scene_membership(uint8_t sceneid, uint16_t *grplist, uint8_t max)
{
    uint8_t num = 0;
    sceneNode_t *p = NULL;
    p = scene_list_find_scene(g_sceneList, sceneid);
    if (p) {
        num = p->grpCnt > max ? max : p->grpCnt;
        memcpy(grplist, p->grpList, num * sizeof(uint16_t));
        return num;
    }
    return 0;
}		/* -----  end of function get_scene_membership  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  del_group_from_all_scene
 *  Description:
 * =====================================================================================
 */
uint8_t del_group_from_all_scene ( uint16_t grpid )
{
    return scene_list_del_group_from_all_scene ( g_sceneList, grpid );
}		/* -----  end of function del_group_from_all_scene  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  print_all_scene
 *  Description:
 * =====================================================================================
 */
void print_all_scene ( void )
{
    scene_list_print ( g_sceneList );
}		/* -----  end of function print_all_scene  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  update_scenes_for_group
 *  Description:  update scene info for group
 *       Return:  0 - sucess
 *                1 - error occurs
 * =====================================================================================
 */
uint8_t update_scenes_for_group ( uint16_t grpid, uint8_t *scnlst, uint8_t num )
{
    return scene_list_update_scenes_for_group(g_sceneList, grpid, scnlst, num);
}		/* -----  end of function update_scenes_for_group  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  get_all_scenes
 *  Description:  get the scene list
 *       Return:  the actual number of scenes
 * =====================================================================================
 */
uint8_t get_all_scenes ( uint8_t *idlst, uint8_t max )
{
    return scene_list_get_all_scene_id ( g_sceneList, idlst, max);
}		/* -----  end of function get_all_scenes  ----- */
