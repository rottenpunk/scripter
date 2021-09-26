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
//            Module:  error.h
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

#define RC_OK               0
#define RC_OPENPCODE        1          // Can't open pcode module.
#define RC_PCODEBAD         2          // PCode module format is bad.
#define RC_NOMAIN           3          // Can't find main() function.
#define RC_INVALIDOP        4          // Invalid op code.
#define RC_CALLSTACKOVER    5          // Call stack overflow.
#define RC_CALLSTACKUNDER   6          // Call stack underflow.
#define RC_VALUESTACKOVER   7          // Value stack overflow.
#define RC_VALUESTACKUNDER  8          // Value stack underflow.
#define RC_TOOMANYPARMS     9          // Too many parms passed to function.
#define RC_TOOFEWPARMS     10          // Not enough parms passed to function.
#define RC_BADOPERATOR     11          // Invalid arithmetic operation.
#define RC_BADOPERAND1     12          // Operand 1 not integer.
#define RC_BADOPERAND2     13          // Operand 2 not integer.
#define RC_NOTINTEGER      14          // Switch argument not integer.
#define RC_FUNCTNOTFOUND   15          // Function not found.
#define RC_NOMEMORY        16          // Out of memory.
#define RC_BADFUNCPARM     17          // Bad parm to function.
#define RC_BADFUNCPARM1    18          // Bad parm 1 to function.
#define RC_BADFUNCPARM2    19          // Bad parm 2 to function.
#define RC_BADFUNCPARM3    20          // Bad parm 3 to function.
#define RC_BADFUNCPARM4    21          // Bad parm 4 to function.
#define RC_BADFUNCPARM5    22          // Bad parm 5 to function.
#define RC_VARUNDEFINED    23          // Variable undefined.