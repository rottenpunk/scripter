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
//            Module:  global.h
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


//-----------------------------------------------------------------------------
// Public function names definition structure...
//-----------------------------------------------------------------------------

typedef struct PublicFunctions {
   struct _SymTab*   Name;
   USHORT            PCodeOffset;
} PUBFUNS;


//-----------------------------------------------------------------------------
// Global variables...
//-----------------------------------------------------------------------------

extern char*      Name;                // Program name, less extension.
extern char*      Path;                // Program Path & name, less ext.
extern int        Column;              // Current column number in source.
extern int        Line;                // Current line number in source.
extern BYTE       Options;             // Compiler options.
extern BYTE       Flags;               // Compiler flags.
extern TREE*      SymbolTree;          // Symbol table tree anchor.
extern SYMTAB*    Symbols;             // Chain of symbol entries in tree.
extern int        LastError;           // Last error code.
extern int        IntError;            // Internal error code.
extern int        ErrorInstance;       // Instance number of error.
extern int        ErrorCount;          // Count of errors encountered.
extern char       ErrorFile[9];        // Name of module that created error
extern USHORT     CurCodePool;         // Current offset into PCode pool.
extern USHORT     CurLitPool;          // Current offset into literal pool.
extern POOLITEM*  PCodeHead;           // Chain of PCode instructions.
extern POOLITEM*  PCodeTail;           // Chain of PCode instructions.
extern POOLITEM*  LiteralHead;         // Chain of literals (Data).
extern POOLITEM*  LiteralTail;         // Chain of literals (Data).
extern PUBFUNS    PubFuns[MAX_PUB_VARS]; // Table of public function names.
extern int        PubFunsCount;          // Count of public functions defined.
extern int        PubFunsLength;         // Length of all names/offsets in table.
extern USHORT     ExtVarCount;           // Count of external variables defined.
extern USHORT     ExtVars[MAX_EXT_VARS]; // Offset to ext var names in pool.
extern int        CurrentLoop;           // Current index into Breaks/Continues.
extern USHORT     Breaks[MAX_LOOPS];     // Array of break levels.
extern USHORT     Continues[MAX_LOOPS];  // Array of continue levels.
extern FILE*      ListFile;            // Listing File handle.


//-----------------------------------------------------------------------------
// Definitions for compiler options above...
//-----------------------------------------------------------------------------

#define OPT_DEBUG        0x01          // Go into debug mode.
#define OPT_MESSAGE      0x02          // Use Message() to log messages.
#define OPT_LOG          0x04          // Use LogMsg() to log messages.
#define OPT_COMPILE      0x08          // Compile only pcode.


//-----------------------------------------------------------------------------
// Definitions for compiler flags above...
//-----------------------------------------------------------------------------

#define GLOBAL_EOM       0x01          // End of source module reached.
#define GLOBAL_NULL      0x02          // Null Data put into literal pool.
#define GLOBAL_FIRST     0x04          // First statement already processed



