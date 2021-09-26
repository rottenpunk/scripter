//-----------------------------------------------------------------------------
//
//
//                       GENERALIZED SCRIPTING LANGUAGE
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
#include <ctype.h>

#include "typedefs.h"
#include "config.h"
#include "tree.h"
#include "data.h"
#include "symbols.h"
#include "keywords.h"
#include "operator.h"
#include "scanner.h"
#include "gen.h"
#include "errors.h"
#include "pcode.h"
#include "global.h"
#include "funnames.h"
#include "dump.h"


#define GMEM(len) (calloc(len, 1))
#define FMEM(p)   (free(p))


//-----------------------------------------------------------------------------
// General useful compiler macros to reference Global Vars...
//-----------------------------------------------------------------------------

#define SET_ERROR(n)     ErrorSet(n, __LINE__, _PROGNAME)
#define SET_INT_ERROR(n) (IntError = n)  // Set internal error.
#define CHECK_ERROR()    (LastError)     // Check last error code.
#define SET_ERROR_SYMBOL(n) ErrorSym(n)      // Reset symbol in error.

#define LINE()           (Line)          // Return current line no
#define COLUMN()         (Column)        // Return current column.
#define PCODE_OFFSET()   (CurCodePool)   // Get cur pcode offset.
#define LITERAL_OFFSET() (CurLitPool)    // Get cur literal offset

#define CHECK_DEBUG()    (Options & OPT_DEBUG) // Debug option?


/*--------------------------------------------------------------------*/
/* Useful macros...                                                   */
/*--------------------------------------------------------------------*/

#define MOVE(a, b)  (memcpy((void *) a, (void *) b, \
                            sizeof(a) < strlen(b) ? sizeof(a) : strlen(b) ))
#define CLEAR(a)    (memset( (void *)&a, 0x00, sizeof(a)) )
#define BLANK(a)    (memset( (void *)&a, ' ', sizeof(a)) )




