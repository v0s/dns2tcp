/*
** Copyright (C) 2006 Olivier DEMBOUR
** $Id: base64.c,v 1.4.4.5 2010/06/07 13:36:25 dembour Exp $
**
** This file is part of Brute ike
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with This program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* took from Apache 1.3.27 */


#include <ctype.h>
#include <string.h>
#ifdef _WIN32
#include "mywin32.h"
#endif

/*char base64_padding = '=';*/

static const char basis_64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char pr2six[256] =
{
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

int base64_encode(char *string, char *encoded, int len)
{
    int i;                      
    char *p;

    p = encoded;
    for (i = 0; i < len - 2; i += 3) {
        *p++ = basis_64[(string[i] >> 2) & 0x3F];
        *p++ = basis_64[((string[i] & 0x3) << 4) |
                        ((int) (string[i + 1] & 0xF0) >> 4)];
        *p++ = basis_64[((string[i + 1] & 0xF) << 2) |
                        ((int) (string[i + 2] & 0xC0) >> 6)];
        *p++ = basis_64[string[i + 2] & 0x3F];
    }
    if (i < len) {
        *p++ = basis_64[(string[i] >> 2) & 0x3F];
        if (i == (len - 1)) {
            *p++ = basis_64[((string[i] & 0x3) << 4)];
            /**p++ = base64_padding;*/
        }
        else {
            *p++ = basis_64[((string[i] & 0x3) << 4) |
                            ((int) (string[i + 1] & 0xF0) >> 4)];
            *p++ = basis_64[((string[i + 1] & 0xF) << 2)];
        }
        /**p++ = base64_padding;*/
    }

    *p++ = '\0';
    return (int) (p - encoded);
}

int  base64_decode(unsigned char *bufplain, const char *bufcoded)
{   
    int nbytesdecoded;
    register const unsigned char *bufin;
    register unsigned char *bufout;
    register int nprbytes;
    
    bufin = (const unsigned char *) bufcoded;
    while (pr2six[*(bufin++)] <= 63);
    nprbytes = (int) ((bufin - (const unsigned char *) bufcoded) - 1);
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;
    
    bufout = (unsigned char *) bufplain;
    bufin = (const unsigned char *) bufcoded;
    
    while (nprbytes > 4) {
        *(bufout++) = 
            (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
        *(bufout++) = 
            (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
        *(bufout++) = 
            (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
        bufin += 4; 
        nprbytes -= 4;
    }
    
    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1) {
        *(bufout++) = 
            (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    }
    if (nprbytes > 2) {
        *(bufout++) = 
            (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    }
    if (nprbytes > 3) {
        *(bufout++) = 
            (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    }
    
    nbytesdecoded -= (4 - nprbytes) & 3;
    return nbytesdecoded;
}

static const char basis_32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

int base32_encode(char *string, char *encoded, int len)
{
    int i, index, digit;
    int currByte, nextByte;
    char *p;

    p = encoded;
    for (i = 0; i < len;) {
        currByte = (unsigned char)string[i];
        
        /* Is the current digit going to span a byte boundary? */
        if (i + 1 < len) {
            nextByte = (unsigned char)string[i + 1];
        } else {
            nextByte = 0;
        }

        /* Encode the current block */
        for (index = 0; index < 8 && (i * 8 + index * 5) < (len * 8); index++) {
            if ((i * 8 + index * 5) < (len * 8)) {
                if (index == 0) {
                    digit = currByte >> 3;
                } else if (index == 1) {
                    digit = ((currByte & 0x07) << 2) | (nextByte >> 6);
                } else if (index == 2) {
                    digit = (nextByte >> 1) & 0x1F;
                } else if (index == 3) {
                    digit = ((nextByte & 0x01) << 4);
                    if ((i + 2) < len) {
                        currByte = (unsigned char)string[i + 2];
                        digit |= (currByte >> 4);
                    }
                } else if (index == 4) {
                    digit = ((currByte & 0x0F) << 1);
                    if ((i + 3) < len) {
                        nextByte = (unsigned char)string[i + 3];
                        digit |= (nextByte >> 7);
                    }
                } else if (index == 5) {
                    digit = (nextByte >> 2) & 0x1F;
                } else if (index == 6) {
                    digit = ((nextByte & 0x03) << 3);
                    if ((i + 4) < len) {
                        currByte = (unsigned char)string[i + 4];
                        digit |= (currByte >> 5);
                    }
                } else if (index == 7) {
                    digit = currByte & 0x1F;
                }

                *p++ = basis_32[digit];
            }
        }
        i += 5;
    }

    *p = '\0';
    return (int)(p - encoded);
}

int base32_decode(unsigned char *bufplain, const char *bufcoded)
{
    int i, index, digit;
    int currByte, nextByte;
    int nbytesdecoded;
    int len = strlen(bufcoded);
    unsigned char *bufout = bufplain;
    const unsigned char *bufin = (const unsigned char *)bufcoded;

    for (i = 0; i < len; i += 8) {
        currByte = 0;
        nextByte = 0;

        for (index = 0; index < 8 && (i + index) < len; index++) {
            if (bufin[i + index] >= 'A' && bufin[i + index] <= 'Z') {
                digit = bufin[i + index] - 'A';
            } else if (bufin[i + index] >= '2' && bufin[i + index] <= '7') {
                digit = bufin[i + index] - '2' + 26;
            } else if (bufin[i + index] >= 'a' && bufin[i + index] <= 'z') {
                digit = bufin[i + index] - 'a';
            } else {
                return -1; // Invalid character
            }

            switch (index) {
            case 0:
                currByte = digit << 3;
                break;
            case 1:
                currByte |= (digit >> 2);
                *bufout++ = currByte;
                currByte = (digit & 0x03) << 6;
                break;
            case 2:
                currByte |= digit << 1;
                break;
            case 3:
                currByte |= (digit >> 4);
                *bufout++ = currByte;
                currByte = (digit & 0x0F) << 4;
                break;
            case 4:
                currByte |= (digit >> 1);
                *bufout++ = currByte;
                currByte = (digit & 0x01) << 7;
                break;
            case 5:
                currByte |= digit << 2;
                break;
            case 6:
                currByte |= (digit >> 3);
                *bufout++ = currByte;
                currByte = (digit & 0x07) << 5;
                break;
            case 7:
                currByte |= digit;
                *bufout++ = currByte;
                break;
            }
        }
    }

    nbytesdecoded = (int)(bufout - bufplain);
    return nbytesdecoded;
}
