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
//            Module:  gen.h
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
// PATCH -- This structure represents a SYMREF label (and it's
//          related PCode instruction) to backpatch when the
//          label is finally resolved...
//-----------------------------------------------------------------------------

struct _Patch {
   struct _Patch    *Next;             // Next on chain to backpatch.
   SYMREF           *SymRef;           // Reference to symbol.
   struct _PoolItem *PoolItem;         // Pool item of PCode instruction.
};

typedef struct _Patch  PATCH;




//-----------------------------------------------------------------------------
// POOLITEM -- Is a link to an item that will go into either the
//             PCode pool or the literal pool...
//-----------------------------------------------------------------------------

struct _PoolItem {
   struct _PoolItem  *Next;            // Next Item on chain.
   BYTE               Length;          // Length of PCode instruction.
   char               Item[1];         // The item to go into the pool.
};

typedef struct _PoolItem POOLITEM;
