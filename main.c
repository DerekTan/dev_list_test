/*
 * ===========================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  main test program
 *
 *        Version:  1.0
 *        Created:  2015/11/19 9:01:33
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
#include <string.h>
#include "dev_list_app.h"
#include "dev_list.h"
#include "test.h"
#include "scene_list_app.h"

void print_n_u16(uint8_t num, uint16_t *p);
void print_n_u8(uint8_t num, uint8_t *p);
uint8_t ctoi(uint8_t c);
uint8_t get_num_byte(uint8_t *s);
uint8_t get_extaddr(uint8_t *pext, uint8_t *pstr);
uint16_t u8_to_u16(uint8_t msb, uint8_t lsb);
uint16_t get_num_from_dec_str(uint8_t *p);
uint8_t do_cmd_str(char *instr, uint8_t len);
void process_scene_command(char *s);
void process_group_command(char *s);
void print_help_msg(void);

int main(void) {
    char instr[128];

    create_online_list();
    create_offline_list();
    create_scene_list();
#if 0
    do_cmd_str("a11,1122334455667788", 128);
    do_cmd_str("a2,1234567812345678", 128);
    do_cmd_str("a333,aabbccddeeff9900", 128);
    do_cmd_str("a1234,efaadefada001122", 128);
    do_cmd_str("a5555,1234123412341234", 128);

    do_cmd_str("ga333,1", 128);
    do_cmd_str("ga333,2", 128);
//    do_cmd_str("ga333,3", 128);
//    do_cmd_str("ga333,4", 128);
//    do_cmd_str("ga333,5", 128);
//    do_cmd_str("ga333,6", 128);
    do_cmd_str("ga11,1", 128);
//    do_cmd_str("ga11,2", 128);
    do_cmd_str("ga11,3", 128);
//    do_cmd_str("ga2,4", 128);
//    do_cmd_str("ga2,5", 128);
//    do_cmd_str("ga2,6", 128);

    do_cmd_str("sa11,12", 128);
    do_cmd_str("sa11,13", 128);
    do_cmd_str("sa11,14", 128);
    do_cmd_str("sa11,15", 128);
    do_cmd_str("sa11,16", 128);
    do_cmd_str("sa11,17", 128);
    do_cmd_str("sa12,12", 128);
    do_cmd_str("sa12,13", 128);
    do_cmd_str("sa12,14", 128);
    do_cmd_str("sa12,15", 128);
    do_cmd_str("sa12,16", 128);
    do_cmd_str("sa12,17", 128);
    do_cmd_str("sa13,12", 128);
    do_cmd_str("sa13,13", 128);
    do_cmd_str("sa13,14", 128);
    do_cmd_str("sa14,15", 128);
    do_cmd_str("sa14,16", 128);
    do_cmd_str("sa15,17", 128);
#endif
    for ( ;; ) {

        if ( fgets(instr, 128, stdin) ) {
            if (do_cmd_str(instr, 128))
                break;

        }
    }
    return 0;
}

uint8_t do_cmd_str(char *s, uint8_t len)
{
    uint16_t shortaddr;
    uint8_t extaddr[8];
    uint8_t i;
    char *p = NULL;
    char instr[128];
    memcpy(instr, s, len);
    if (NULL != (p = strchr(instr, '\n')))
        *p = '\0';
    switch (instr[0]) {
        case 'a':   //add_online_dev
            if (NULL != (p = strchr(instr, ',')))
                *p = '\0';
            shortaddr = get_num_from_dec_str(instr+1);
            if (p && *(p+1))
                get_extaddr(extaddr, p+1);
            else
                memcpy(extaddr, INVALID_EXTADDR, 8);
            add_online_dev_addr(shortaddr, extaddr);
            break;
        case 'd':   //add_offline_dev
            if (NULL != (p = strchr(instr, ',')))
                *p = '\0';
            shortaddr = get_num_from_dec_str(instr+1);
            add_offline_dev_addr(shortaddr);
            break;
        case 'p':   //print online dev
            online_dev_print();
            offline_dev_print();
            break;
        case 'g':   //get group member
            process_group_command(instr+1);
            break;
        case 'e': //get i[e]ee address
            p = strtok(instr+1, ",");
            shortaddr = get_num_from_dec_str(p);
            get_dev_extaddr( shortaddr, extaddr );
            printf("extAddr:0x");
            for (i=0; i<8; i++)
                printf("%02x",extaddr[i]);
            printf("\n");
            break;
        case 's':
            process_scene_command(instr+1);
            break;
        case 'q':
            return 1;
            break;
        default:
            print_help_msg();
            break;
    }
    return 0;
}

void print_help_msg(void)
{
    printf("[a]dd device\n"
            "[d]elete device\n"
            "[p]rint online devices\n"
            "get i[e]ee address\n"
            "--[g]roup \n"
            "[g]roup [a]dd a device with a group info\n"
            "[g]roup [d]elete a device's one group info\n"
            "[g]roup [l]ist group members\n"
            "[g]roup [u]pdate group info\n"
            "[g]roup [r]emove all groups\n"
            "[g]roup [p]rint all groups\n"
            "--[s]cene \n"
            "[s]cene [a]dd\n"
            "[s]cene [d]elete\n"
            "[s]cene [r]emove all from a group\n"
            "[s]cene [l]ist scene members\n"
            "[s]cene [p]rint\n"
            "[q]uit\n");
}

void process_group_command(char *s)
{
    uint16_t shortaddr;
    uint8_t grpcnt;
    uint16_t grplist[128];
    uint16_t ret[128];
    uint8_t num;
    char *p = NULL;
    uint8_t mode;
    uint8_t i;
    switch (s[0]) {
        case 'l':  //list group members
            p = strtok(s+1, ",");
            shortaddr = get_num_from_dec_str(p);
            num = get_group_member(shortaddr, ret);
            print_n_u16(num, ret);
            break;
        case 'a':   //add group info
            p = strtok(s+1, ",");
            shortaddr = get_num_from_dec_str(p);
            p = strtok(NULL, ",");
            grplist[0] = get_num_from_dec_str(p);
            add_dev_group_info(shortaddr, grplist[0]);
            break;
        case 'u':   //update group info
            p = strtok(s+1, ",");
            shortaddr = get_num_from_dec_str(p);
            p = strtok(NULL, ",");
            grpcnt = get_num_from_dec_str(p);
            for (i = 0; i < grpcnt; i++)
            {
                p = strtok(NULL, ",");
                grplist[i] = get_num_from_dec_str(p);
            }
            update_dev_group_info(shortaddr, grpcnt, grplist);
            break;
        case 'd':
            p = strtok(s+1, ",");
            shortaddr = get_num_from_dec_str(p);
            p = strtok(NULL, ",");
            grplist[0] = get_num_from_dec_str(p);
            del_dev_group_info(shortaddr, grplist[0]);
            break;
        case 'p': //print all groups
            num = get_all_groups(grplist, 128);
            print_n_u16(num, grplist);
            break;
        case 'r': //remove all groups
            p = strtok(s+1, ",");
            mode = get_num_from_dec_str(p);
            p = strtok(NULL, ",");
            shortaddr = get_num_from_dec_str(p);
            del_all_groups(mode, shortaddr);
            break;
    }
}

void process_scene_command(char *s)
{
    uint16_t grpid;
    uint8_t  scnid;
    uint8_t i, num;
    uint16_t grplist[128];
    uint8_t scnlst[128];
    uint8_t scncnt;
    char *p = NULL;
    switch (s[0]) {
        case 'a':
            p = strtok(s+1, ",");
            scnid = get_num_from_dec_str(p);
            p = strtok(NULL, ",");
            grpid = get_num_from_dec_str(p);
            add_group_to_scene(scnid, grpid);
            break;
        case 'd':
            p = strtok(s+1, ",");
            scnid = get_num_from_dec_str(p);
            p = strtok(NULL, ",");
            grpid = get_num_from_dec_str(p);
            del_group_from_scene(scnid, grpid);
            break;
        case 'r':
            p = strtok(s+1, ",");
            grpid = get_num_from_dec_str(p);
            del_group_from_all_scene(grpid);
            break;
        case 'l': //list scene membership
            p = strtok(s+1, ",");
            if (p) {
                scnid = get_num_from_dec_str(p);
                num = get_scene_membership(scnid, grplist, 128);
                print_n_u16(num, grplist);
            }
            else {
                num = get_all_scenes(scnlst, 128);
                print_n_u8(num, scnlst);
            }
            break;
        case 'u':
            p = strtok(s+1, ",");
            grpid = get_num_from_dec_str(p);
            p = strtok(NULL, ",");
            scncnt = get_num_from_dec_str(p);
            for (i = 0; i < scncnt; i++)
            {
                p = strtok(NULL, ",");
                scnlst[i] = get_num_from_dec_str(p);
            }
            update_scenes_for_group ( grpid, scnlst, scncnt );
            break;

        case 'p': //print all scene
            print_all_scene();
            break;
        default:
            break;
    }
}

uint16_t get_num_from_dec_str(uint8_t *p)
{
    uint16_t ret = 0;
    while (*p) {
        ret = 10*ret + ctoi(*p++);
        }
    return ret;
}

uint16_t u8_to_u16(uint8_t msb, uint8_t lsb)
{
    return (uint16_t)((msb << 8) | lsb);
}

uint8_t get_extaddr(uint8_t *pext, uint8_t *pstr)
{
    uint8_t i;
    for ( i=0; i<8; i++ ) {
        *pext++ = get_num_byte(pstr);
        pstr += 2;
    }
    return 0;
}

uint8_t get_num_byte(uint8_t *s)
{
    return (uint8_t)(ctoi(*s) << 4 | ctoi(*(s+1)));
}

uint8_t ctoi(uint8_t c)
{
    if (c >= 'a') {
        return c - 'a' + 10;
    }
    return c - '0';
}

void print_n_u16(uint8_t num, uint16_t *p)
{
    uint8_t i;
    printf("Total: %d\n", num);
    if (num) {
        for ( i=0; i<num-1; i++) {
            printf("%6d,", p[i]);
        }
        printf("%6d\n", p[i]);
    }
}

void print_n_u8(uint8_t num, uint8_t *p)
{
    uint8_t i;
    printf("Total: %d\n", num);
    if (num) {
        for ( i=0; i<num-1; i++) {
            printf("%6d,", p[i]);
        }
        printf("%6d\n", p[i]);
    }
}
