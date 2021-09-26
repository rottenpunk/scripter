//-----------------------------------------------------------------------------
//
//
//                       GENERALIZED LEXICAL ANALYZER
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
//            Module:  keywords.c
//
//             Title:  Generalized lexical analyzer
//
//       Description:  This file contains the keword table.
//
//            Author:  J.C. Overton
//
//              Date:  06/20/94
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


//-----------------------------------------------------------------------------
// Keyword table...
//-----------------------------------------------------------------------------

KeyWordTab KeyTab[] = {
   "auto",        AUTO,         KeyTab,
   "break",       BREAK,        KeyTab,
   "case",        CASE,         KeyTab,
   "char",        CHAR,         KeyTab,
   "const",       CONST,        KeyTab,
   "continue",    CONTINUE,     KeyTab,
   "default",     DEFAULT,      KeyTab,
   "do",          DO,           KeyTab,
   "double",      DOUBLE,       KeyTab,
   "else",        ELSE,         KeyTab,
   "enum",        ENUM,         KeyTab,
   "extern",      EXTERN,       KeyTab,
   "float",       FLOAT,        KeyTab,
   "for",         FOR,          KeyTab,
   "goto",        GOTO,         KeyTab,
   "if",          IF,           KeyTab,
   "int",         INT,          KeyTab,
   "long",        LONG,         KeyTab,
   "register",    REGISTER,     KeyTab,
   "return",      RETURN,       KeyTab,
   "short",       SHORT,        KeyTab,
   "signed",      SIGNED,       KeyTab,
   "sizeof",      SIZEOF,       KeyTab,
   "static",      STATIC,       KeyTab,
   "struct",      STRUCT,       KeyTab,
   "switch",      SWITCH,       KeyTab,
   "typedef",     TYPEDEF,      KeyTab,
   "void",        VOID,         KeyTab,
   "volatile",    VOLATILE,     KeyTab,
   "union",       UNION,        KeyTab,
   "unsigned",    UNSIGNED,     KeyTab,
   "while",       WHILE,        KeyTab,
   NULL,          0,            0
};
