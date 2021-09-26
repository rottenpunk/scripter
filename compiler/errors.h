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
//            Module:  errors.h
//
//             Title:  Generalized scripting language compiler
//
//       Description:  This file contains general error definitions for
//                     the generalize scripting language compiler.
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

#define RC_SYMCREATE         1         // Problem creating a symbol tree.
#define RC_SYMADD            2         // Could not add symbol into tree.
#define RC_ESTACK            3         // Maximum evaluation recursion.
#define RC_PARENTHESIS       4         // Unbalanced Parenthesis.
#define RC_EXPRESSION        5         // Expression error.
#define RC_NOSOURCEFILE      6         // Can not open input source file.
#define RC_OBJECT_OPEN       7         // Can not open output pcode file.
#define RC_NOTIMP            8         // Not implemented yet.
#define RC_ELSE              9         // Unmatched else.
#define RC_CONSTANT          10        // Lval can not be a constant.
#define RC_SYNTAX            11        // General Syntax error.
#define RC_DUP_LABEL         12        // Duplicate label.
#define RC_FUNCPARM          13        // Function parm error.
#define RC_NOTBODY           14        // Not allowed outside body of function.
#define RC_ABRUPTEOF         15        // EOF while in function body.
#define RC_EXPECTSEMI        16        // Expected a semicolon.
#define RC_LVAL              17        // Invalid LVAL expression.
#define RC_GOTO              18        // Invalid goto label.
#define RC_EXPECTLPAR        19        // Expected a left parenthesis.
#define RC_EXPECTRPAR        20        // Expected a right parenthesis.
#define RC_EXPECTWHILE       21        // Expected a while after do.
#define RC_EXPECTSYMBOL      22        // Expected a symbol.
#define RC_TOOMANYEXTVARS    23        // Too many external variables defined.
#define RC_BADBREAK          24        // Illegal break.
#define RC_BADCONTINUE       25        // Illegal continue.
#define RC_EXPECTLBRACE      26        // Expected a left brace.
#define RC_BADCASE           27        // Case value not integer.
#define RC_EXPECTCOLON       28        // Expected a colon after case/default.
#define RC_NOTINSWITCH       29        // Not currently in a switch statement.
