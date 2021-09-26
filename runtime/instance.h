//-----------------------------------------------------------------------------
//
//
//                GENERALIZED SCRIPTING LANGUAGE EXECUTOR
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
//            Module:  instance.h
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
// Program instance...
//-----------------------------------------------------------------------------

struct _PI {
   char*     Name;                     // Program name.
   PHEADER*  Header;                   // Header of current pCode executing.
   BYTE*     PCode;                    // Start of pcode pool.
   BYTE*     Literals;                 // Start of literal pool.
   USHORT*   Externs;                  // Start of externals table.
   PUBLICS*  Publics;                  // Start of public functions table.
};
typedef struct _PI  PI;


//-----------------------------------------------------------------------------
// Task instance...
//-----------------------------------------------------------------------------

struct _TI {
   char*     Path;                     // Path to pCode file.
   BYTE*     IP;                       // Current instruction pointer.
   void*     VarHandle;                // Variable handle.
   BYTE      Options;                  // Various runtime options.
   int       LastError;                // Last error code.
   int       Line;                     // Line number of last error.
   int       Column;                   // Column number of last error.
   PI*       Pi;                       // Starting program instance.
};
typedef struct _TI  TI;


#define OPT_DEBUG       0x01           // Debug option.
