//-----------------------------------------------------------------------------
//
//
//                       GENERALIZED SCRIPTING LANGUAGE
//
//        (C) Copyright John C. Overton, Inc. 1994. All Rights Reserved.
//               Advanced Communication Development Tools, Inc
//
//        No part of this file may be duplicated, revised, translated,
//        localized or modified in any manner or compiled, linked or
//        uploaded or downloaded to or from any computer system without
//        the prior written consent of Advanced Communication Development
//        Tools, Inc, or John C. Overton.
//
//
//            Module:  errors.c
//
//             Title:  Generalized scripting language compiler
//
//       Description:  This file contains general definitions for the
//                     generalize scripting language compiler.
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
#include <stdarg.h>
#include "executor.h"


char* ErrorStrings[] = {
   "",                                            //  0
   "Can not open PCode module file",              //  1
   "Pcode file is in bad format",                 //  2
   "Can not find main() function",                //  3
   "Invalid op code",                             //  4
   "Call stack overflow",                         //  5
   "Call stack underflow",                        //  6
   "Value stack overflow",                        //  7
   "Value stack underflow",                       //  8
   "Too many parms passed to function",           //  9
   "Not enough parms passed to function",         // 10
   "Invalid arithmetic operation",                // 11
   "Operand 1 not integer",                       // 12
   "Operand 2 not integer",                       // 13
   "Switch argument not integer",                 // 14
   "Function not found",                          // 15
   "Out of memory",                               // 16
   "Bad parameter to function",                   // 17
   "Bad parameter 1 to function",                 // 18
   "Bad parameter 2 to function",                 // 19
   "Bad parameter 3 to function",                 // 20
   "Bad parameter 4 to function",                 // 21
   "Bad parameter 5 to function",                 // 22
   "Variable undefined",                          // 23
   ""                                             // 24
};

static int MaxErrorStrings = sizeof(ErrorStrings) / sizeof(ErrorStrings[0]);


//void Error(int rc, ...)
//{
//   va_list arg;
//
//   va_start(arg, rc);
//   vfprintf(stderr, ErrorStrings[rc], arg);
//   va_end(arg);
//   exit(1);
//}


void Error(TI* Ti)
{

   if (Ti->LastError < MaxErrorStrings)
      printf("Error #%d on Line %d Col %d: %s\n",
             Ti->LastError,
             Ti->Line,
             Ti->Column,
             ErrorStrings[Ti->LastError] );
   else
      printf("Error #%d on Line %d Col %d: %s\n",
             Ti->LastError,
             Ti->Line,
             Ti->Column,
             "Error from executed function" );     
}
