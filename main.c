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

void print_n_u16(uint8_t num, uint16_t *p);
uint8_t ctoi(uint8_t c);
uint8_t get_num_byte(uint8_t *s);
uint8_t get_extaddr(uint8_t *pext, uint8_t *pstr);
uint16_t u8_to_u16(uint8_t msb, uint8_t lsb);
uint16_t get_num_from_dec_str(uint8_t *p);
uint8_t do_cmd_str(char *instr, uint8_t len);

int main(void) {
    char instr[128];

    create_online_list();
    create_offline_list();

    do_cmd_str("a11,1122334455667788", 128);
    do_cmd_str("a2,1234567812345678", 128);
    do_cmd_str("a333,aabbccddeeff9900", 128);
    do_cmd_str("a1234,efaadefada001122", 128);
    do_cmd_str("a5555,1234123412341234", 128);

    for ( ;; ) {
        printf("[a]dd\n"
               "[d]elete\n"
               "[p]rint online devices\n"
               "[g]et group members\n"
               "[u]pdate group info\n"
               "add group [i]nfo\n"
               "[q]uit\n");

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
    uint8_t grpcnt;
    uint16_t grplist[128];
    uint8_t extaddr[8];
    uint8_t num;
    uint8_t i;
    uint16_t ret[128];
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
            add_online_dev(shortaddr, extaddr);
            break;
        case 'd':   //add_offline_dev
            if (NULL != (p = strchr(instr, ',')))
                *p = '\0';
            shortaddr = get_num_from_dec_str(instr+1);
            add_offline_dev(shortaddr);
            break;
        case 'p':   //print online dev
            online_dev_print();
            offline_dev_print();
            break;
        case 'g':   //get group member
            if (NULL != (p = strchr(instr, ',')))
                *p = '\0';
            shortaddr = get_num_from_dec_str(instr+1);
            num = get_group_member(shortaddr, ret);
            print_n_u16(num, ret);
            break;
        case 'i':   //add group info
            p = strtok(instr+1, ",");
            shortaddr = get_num_from_dec_str(p);
            p = strtok(NULL, ",");
            grplist[0] = get_num_from_dec_str(p);
            add_dev_group_info(shortaddr, grplist[0]);
            break;
        case 'u':   //update group info
            p = strtok(instr+1, ",");
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
        case 'q':
            return 1;
            break;
        default:
            break;
    }
    return 0;
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
