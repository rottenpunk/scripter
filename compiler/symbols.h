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
//            Module:  symbols.h
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
// SYMTAB -- Symbol table entries and reference blocks...
//-----------------------------------------------------------------------------


struct _SymTab {
   struct _SymTab  *Next;              // Chain these off of GBL.Symbols.
   USHORT           PoolOff;           // Offset into literal pool.
   DATA            *Literal;           // Ptr to literal data.
   USHORT           Flags;             // Various flags.
   struct _SymRef  *LitRef;            // Chain of literal references.
   struct _SymRef  *VarRef;            // Chain of variables references.
   struct _SymRef  *LabRef;            // Chain of label references.
   struct _SymRef  *LabDefRef;         // Definition of the label itself.
   struct _Patch   *BackPatch;         // Chain of PCodes to backpatch.
};

typedef struct _SymTab SYMTAB;


//-----------------------------------------------------------------------------
// Flags for SymFlags above...
//-----------------------------------------------------------------------------

#define SYMBOL_INLITPOOL   0x01        // Symbol name is in literal pool.
#define SYMBOL_RESOLVED    0x02        // Symbol has been resolved.
#define SYMBOL_IN_DO_LOOP  0x04        // Symbol is in do loop.
#define SYMBOL_FUNCTION    0x08        // label represents a function.


//-----------------------------------------------------------------------------
// SYMREF -- Symbol reference block has info about a reference to a
//           symbol...
//-----------------------------------------------------------------------------

struct _SymRef {
   struct _SymRef   *Next;             // Next reference on chain.
   USHORT            PCodeOff;         // PCode's offset.
   USHORT            Type;             // Referenced symbol type.
   USHORT            Flags;            // Various flags.
   USHORT            Line;             // Line number in source.
   USHORT            Column;           // Column number in source.
};

typedef struct _SymRef SYMREF;


//-----------------------------------------------------------------------------
// Flags for RefFlags above...
//-----------------------------------------------------------------------------

#define REFER_RESOLVED     0x01        // Symbol reference is resolved.
#define REFER_LIT_FUNC     0x02        // Symbol ref is func name in quotes
#define REFER_BUILTIN      0x04        // Symbol reference is builtin func.
#define REFER_SIGNAL       0x08        // Symbol reference is SIGNAL call.


//-----------------------------------------------------------------------------
// Type of symbol being referenced...
//-----------------------------------------------------------------------------

#define REFER_LITERAL      1           // Reference is a literal.
#define REFER_LABEL        2           // Reference is a label name.
#define REFER_VARIABLE     3           // Reference is a variable name.

