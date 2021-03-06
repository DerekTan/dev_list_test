/*
 * ===========================================================================
 *
 *       Filename:  test.h
 *
 *    Description:  for test
 *
 *        Version:  1.0
 *        Created:  2015/11/19 8:59:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DerekTan (TDK), tandake@gmail.com
 *   Organization:  SKLSSL.ORG
 *
 * ===========================================================================
 */

#ifndef TEST_H
#define TEST_H
#if 0
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;

#ifndef NULL
    #define NULL    0
#endif
#endif
typedef enum
{
  afAddrNotPresent = 0,
  afAddrGroup      = 1,
  afAddr16Bit      = 2,
  afAddr64Bit      = 3,
  afAddrBroadcast  = 15
} afAddrMode_t;

#endif
