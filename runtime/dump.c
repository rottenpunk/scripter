//-----------------------------------------------------------------------------
//
//
//                       GENERALIZED DEBUG DUMP SERVICE
//
//     (C) Copyright John C. Overton, Inc. 1994, 1997. All Rights Reserved.
//               Advanced Communication Development Tools, Inc
//
//        No part of this file may be duplicated, revised, translated,
//        localized or modified in any manner or compiled, linked or
//        uploaded or downloaded to or from any computer system without
//        the prior written consent of Advanced Communication Development
//        Tools, Inc, or John C. Overton.
//
//
//            Module:  dump.c
//
//             Title:  Generalized debugging dump routine.
//
//       Description:  This file dumps a portion of memory to a file.
//
//            Author:  J.C. Overton
//
//              Date:  05/24/97
//
//  Change Log:
//
//  Date      Initials   Description
//  --------  --------   -------------------------------------------
//
//
//
//
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#undef __near
#define __near far                     // Fix to ctype[] to make far.
#include <ctype.h>


void Dump(char *Buffer, long Offset, int Length, int BreakEvery)
{
                        /*          111111111122222222223333333 */
                        /* 123456789012345678901234567890123456 */
    char   CharBuf[36];  /* xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx  */
    int    LineLen;
    int    i;
    char  *p, *s;
   unsigned char   c;
   static char HexTab[] = "0123456789ABCDEF";

    while (Length > 0) {

      if (BreakEvery)
         if ( !(Offset % BreakEvery))
            printf("\n");

      sprintf(CharBuf, "(%ld)", Offset);

        printf(
               " %04X%04X %-9s  ",
               (short)(Offset >> 16),
               (short)(Offset & 0xffff),
               CharBuf);

        if (Length > 16)
            LineLen  = 16;
        else
            LineLen  = Length;

      memset(CharBuf, ' ', sizeof(CharBuf));
      CharBuf[sizeof(CharBuf)-1] = '\0';

      p = Buffer;
      s = CharBuf;
        for (i = 0; i < LineLen; i++) {

         if (i > 0 && i % 4 == 0)
            s++;

         *s++ = HexTab[(*p & 0xf0) >> 4];
         *s++ = HexTab[(*p & 0x0f)];
         p++;
        }
      printf("%s", CharBuf);

      memset(CharBuf, ' ', sizeof(CharBuf));
      CharBuf[16] = '\0';

        for (i = 0; i < LineLen; i++) {
         c = Buffer[i];
            if (c < 127 && c >= 0x20 && isprint( c ) )
            CharBuf[i] = c;
            else
                CharBuf[i] = '.';
        }
        printf("  *%s*\n", CharBuf);

        Buffer += LineLen;
        Length -= LineLen;
        Offset += LineLen;
    }
}
