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
uint16_t get_shortaddr(uint8_t *p);

int main(void) {
    char instr[128];
    uint16_t shortaddr;
    uint8_t grpcnt;
    uint16_t grplist[128];
    uint8_t extaddr[8];
    uint8_t num;
    uint16_t ret[128];
    char *p;

    create_online_list();
    create_offline_list();

    for ( ;; ) {
        printf("[a]dd\n"
               "[d]elete\n"
               "[p]rint online devices\n"
               "[g]et group members\n"
               "[i]nfo for group\n"
               "[q]uit\n");
        if ( fgets(instr, 128, stdin) ) {
            if (NULL != (p = strchr(instr, '\n')))
                *p = '\0';
            switch (instr[0]) {
                case 'a':   //add_online_dev
                    p = strchr(instr, ',');
                    *p = '\0';
                    shortaddr = get_shortaddr(instr+1);
                    get_extaddr(extaddr, p+1);
                    add_online_dev(shortaddr, extaddr);
                    break;
                case 'd':   //add_offline_dev
                    if (NULL != (p = strchr(instr, ',')))
                        *p = '\0';
                    shortaddr = get_shortaddr(instr+1);
                    add_offline_dev(shortaddr);
                    break;
                case 'p':   //print online dev
                    online_dev_print();
                    offline_dev_print();
                    break;
                case 'g':   //get group member
                    if (NULL != (p = strchr(instr, ',')))
                        *p = '\0';
                    shortaddr = get_shortaddr(instr+1);
                    num = get_group_member(shortaddr, ret);
                    print_n_u16(num, ret);
                    break;
                case 'i':   //add group info
                    break;
                case 'u':   //update group info
                    break;
                case 'q':
                    goto Exit;
                    break;
                default:
                    break;
            }

        }
    }
Exit:
    return 0;
}

uint16_t get_shortaddr(uint8_t *p)
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
