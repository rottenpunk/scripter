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
//            Module:  scripter.h
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef __near
#define __near far   // Fix ctype[] so that it's far.


//#define DEBUG 0


#include "typedefs.h"
//#include "dump.h"
void Dump( char *Buffer,
           long Offset,
           int Length,
           int BreakEvery);
#include "pcode.h"
#include "data.h"
#include "operator.h"

#include "config.h"
#include "instance.h"
#include "errors.h"
#include "funnames.h"
#include "func.h"
#include "varapi.h"

#if defined(PMOS)
#include "standard.h"
#endif

extern Bool DebugFlag;


#define GMEM(l)  (calloc(1, l))
#define FMEM(p)  (free(p))

#define CLEAR(a)  (memset( (void*) &a, 0x00, sizeof(a)) )
#define BLANK(a)  (memset( (void*) &a, ' ',  sizeof(a)) )
