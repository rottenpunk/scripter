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
//            Module:  funnames.h
//
//             Title:  Generalized scripting language compiler
//
//       Description:  This file contains all external function names.
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
// symbols.c
//-----------------------------------------------------------------------------

void      SymInit(  void);
SYMTAB*   SymFind(  char   *Name,   int NameLength);
SYMTAB*   SymAdd(   char   *Name,   int NameLength);
DATA*     SymLit(   char   *Name,   int Length);
USHORT    SymTable( BYTE   *Table,  int Length);
SYMREF*   SymRefer( SYMTAB *Symbol, int Type);
void      SymSetBP( SYMTAB *Symbol, SYMREF *Ref, POOLITEM *PI);
void      SymList(  void );

//-----------------------------------------------------------------------------
// symbols.c
//-----------------------------------------------------------------------------

Bool  Eval(Bool RVal);               // Evaluate an LVAL or an RVAL expression.

//-----------------------------------------------------------------------------
// errors.c
//-----------------------------------------------------------------------------

void  Error(void);
void  GetMsg(char *Buf, int MsgNbr, int Instance, char *FileName);
void  ErrorSet( int Error, int Line, char *File);
void  ErrorSym( ID  *Id);

//-----------------------------------------------------------------------------
// gen.c
//-----------------------------------------------------------------------------

POOLITEM  *GenPCode(int PCode, int P1, int P2);
void       GenPatch(POOLITEM  *PI, int Offset);
void       GenNull( void );
void       GenSetBP(POOLITEM *PI, PATCH **Anchor);
void       GenFreeBP(PATCH **Anchor);
void       GenBPAll(PATCH **Anchor, int  Offset);

//-----------------------------------------------------------------------------
// module.c
//-----------------------------------------------------------------------------

PHEADER * ModMake( void );

//-----------------------------------------------------------------------------
// statemen.c
//-----------------------------------------------------------------------------

void Parse(void);
Bool Statement( ID* Id );

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------


