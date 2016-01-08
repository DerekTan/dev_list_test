/*
 * ===========================================================================
 *
 *       Filename:  scene_list_app.h
 *
 *    Description:  provide scene api
 *
 *        Version:  1.0
 *        Created:  2015/11/30 9:44:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DerekTan (TDK), tandake@gmail.com
 *   Organization:  SKLSSL.ORG
 *
 * ===========================================================================
 */

#ifndef SCENE_LIST_APP_H
#define SCENE_LIST_APP_H

#include <stdint.h>
#include <string.h>
#include "scene_list.h"

uint8_t create_scene_list ( void );
uint8_t add_group_to_scene(uint8_t sceneid, uint16_t grpid);
uint8_t del_group_from_scene(uint8_t sceneid, uint16_t grpid);
uint8_t get_scene_membership(uint8_t sceneid, uint16_t *grplist, uint8_t max);
uint8_t del_group_from_all_scene( uint16_t grpid );
void print_all_scene ( void );
uint8_t scene_list_del_group_from_all_scene ( sceneList_t *lst, uint16_t grpid );
uint8_t update_scenes_for_group ( uint16_t grpid, uint8_t *scnlst, uint8_t num );
uint8_t get_all_scenes ( uint8_t *idlst, uint8_t max );

#endif
