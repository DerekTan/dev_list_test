/*
 * ===========================================================================
 *
 *       Filename:  scene_list.h
 *
 *    Description:  scene list head file
 *
 *        Version:  1.0
 *        Created:  2015/11/30 9:02:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DerekTan (TDK), tandake@gmail.com
 *   Organization:  SKLSSL.ORG
 *
 * ===========================================================================
 */

#ifndef SCENE_LIST_H
#define SCENE_LIST_H

#include <stdint.h>
#include <string.h>

typedef struct sceneNode {
    uint8_t id;     //scene id
    uint8_t grpCnt;
    uint16_t *grpList;
    struct sceneNode *next;
} sceneNode_t;

typedef struct {
    sceneNode_t *head;
    uint8_t num;
} sceneList_t;


sceneList_t *scene_list_create ( void );
sceneNode_t *scene_list_find_scene( sceneList_t *lst, uint8_t sceneid );
uint8_t add_group_to_scene_node(sceneNode_t *pNode, uint16_t grpid);
uint8_t del_group_from_scene_node(sceneNode_t *pNode, uint16_t grpid);
uint8_t is_scene_contains_group(sceneNode_t *pNode, uint16_t grpid);
sceneNode_t *create_scene_node ( uint8_t sceneid, uint16_t grpid );
sceneList_t *scene_list_add_node(sceneList_t *lst, sceneNode_t *pNode);
sceneList_t *scene_list_del_scene( sceneList_t *lst, uint8_t sceneid );
uint8_t scene_list_del_group_from_all_scene ( sceneList_t *lst, uint16_t grpid );
void scene_list_strip ( sceneList_t *lst );
uint8_t scene_list_print ( sceneList_t *lst );
uint8_t scene_list_get_all_scene_id ( sceneList_t *lst, uint8_t *idlst, uint8_t max );
uint8_t scene_list_update_scenes_for_group ( sceneList_t *lst, uint16_t grpid, uint8_t *scnlst, uint8_t num );
#endif
