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
//            Module:  global.c
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
#include "scripter.h"



//-----------------------------------------------------------------------------
// Global variables...
//-----------------------------------------------------------------------------

char*      Name;                       // Program name, less extension.
char*      Path;                       // Program Path & name, less ext.
int        Column;                     // Current column number in source.
int        Line;                       // Current line number in source.
BYTE       Options;                    // Compiler options.
BYTE       Flags;                      // Compiler flags.
TREE*      SymbolTree;                 // Symbol table tree anchor.
SYMTAB*    Symbols;                    // Chain of symbol entries in tree.
int        LastError;                  // Last error code.
int        IntError;                   // Internal error code.
int        ErrorInstance;              // Instance number of error.
int        ErrorCount;                 // Count of errors encountered.
char       ErrorFile[9];               // Name of module that created error
USHORT     CurCodePool;                // Current offset into PCode pool.
USHORT     CurLitPool;                 // Current offset into literal pool.
POOLITEM*  PCodeHead;                  // Chain of PCode instructions.
POOLITEM*  PCodeTail;                  // Chain of PCode instructions.
POOLITEM*  LiteralHead;                // Chain of literals (Data).
POOLITEM*  LiteralTail;                // Chain of literals (Data).
PUBFUNS    PubFuns[MAX_PUB_VARS];      // Table of public function names.
int        PubFunsCount;               // Count of public functions defined.
int        PubFunsLength;              // Length of all names/offsets in table.
USHORT     ExtVarCount;                // Count of external variables defined.
USHORT     ExtVars[MAX_EXT_VARS];      // Offset to ext var names in pool.
int        CurrentLoop;                // Current index into Breaks/Continues.
USHORT     Breaks[MAX_LOOPS];          // Array of break levels.
USHORT     Continues[MAX_LOOPS];       // Array of continue levels.
FILE*      ListFile;                   // Listing File handle.

