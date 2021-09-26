#define _PROGNAME "symbols.c"
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
//            Module:  symbols.c
//
//             Title:  Generalized scripting language
//
//       Description:  This file contains routines that manage a symbol table.
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

#include "scripter.h"                  // Other general definitions.


//-----------------------------------------------------------------------------
// Global variables...
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Static local variables...
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Local static routine definitions for routines in this module...
//-----------------------------------------------------------------------------

static Bool SortSyms(
         void           *KeyPtr,
         unsigned short  KeyLen,
         void           *DataPtr,
         unsigned short  DataLen,
         void           *Parm);



//-----------------------------------------------------------------------------
// SymInit() -- Initialize Symbol Table Services...
//-----------------------------------------------------------------------------

void SymInit(void)
{
   int     rc;

   if( (rc = TreeCreate( 5, 0, &SymbolTree)) != TREE_OK) {

      SET_ERROR(RC_SYMCREATE);
      SET_INT_ERROR(rc);
   }

   return;
}



//-----------------------------------------------------------------------------
// SymFind() -- Find a SYMTAB in the symbol table...
//-----------------------------------------------------------------------------

SYMTAB  * SymFind(char *Name, int NameLength)
{
   SYMTAB *Symbol;
   UINT    Length;
   int     rc;


   Symbol = NULL;                      // Clear before locateing.

   if ( (rc = TreeLocate( SymbolTree,  // Find in symbol tree.
                          Name,
                          NameLength,
                          (void **) &Symbol,
                          &Length ) ) == TREE_OK ) {
      return (Symbol);
   }

   return (NULL);                      // Not found in symbol tree.

}



//-----------------------------------------------------------------------------
// SymAdd() -- Add a symbol to symbol table. Return SYMTAB...
//-----------------------------------------------------------------------------

SYMTAB  * SymAdd(char *Name, int NameLength)
{
   SYMTAB   *Symbol;
   int       rc;


   if (  (Symbol = SymFind(Name, NameLength)) == NULL ) {

      Symbol = GMEM(sizeof(SYMTAB));   // Get memory for new SYMTAB.

      Symbol->Next = Symbols;          // Chain in new one.
      Symbols  = Symbol;

      //-----------------------------------------------------------------------
      // Add symbol to symbol tree...
      //-----------------------------------------------------------------------

      if ( (rc = TreeStore( SymbolTree,     // Add to symbol tree.
                            Name,
                            NameLength,
                            Symbol,         // Just store pointer.
                            0 )   ) != TREE_OK) {

         SET_ERROR(RC_SYMADD);
         SET_INT_ERROR(rc);
      }

      Symbol->PoolOff  = CurLitPool;        // Where it'll be in pool.

      Symbol->Literal = SymLit(Name, NameLength);
   }

   return (Symbol);
}



//-----------------------------------------------------------------------------
// SymLit() -- Add a literal to the literal pool...
//-----------------------------------------------------------------------------

DATA  * SymLit(char  *Name, int Length)
{
   POOLITEM *PI;
   DATA     *Data;


   //--------------------------------------------------------------------------
   // Now create POOLITEM and it's encapsulated DATA structure for
   // the literal pool...
   //--------------------------------------------------------------------------

   PI = GMEM(sizeof(POOLITEM) - 1 + sizeof(DATA) + Length);
   PI->Length          = sizeof(DATA) + Length;
   Data                = (DATA *) PI->Item;
   Data->Flag1        |= DATA_STRING;
   Data->StringLength  = Length;
   memcpy(Data->String, Name, Length);

   //--------------------------------------------------------------------------
   // Link POOLITEM onto literal pool chain...
   //--------------------------------------------------------------------------

   if (LiteralHead == NULL ) {
      LiteralHead = LiteralTail = PI;
   } else {
      LiteralTail->Next = PI;
      LiteralTail       = PI;
   }

   CurLitPool += PI->Length;           // Update current pool offset.

   return Data;
}



//-----------------------------------------------------------------------------
// SymTable() -- Add a table to the literal pool...
//-----------------------------------------------------------------------------

USHORT  SymTable(BYTE  *Table, int Length)
{
   POOLITEM *PI;
   USHORT    SaveCurLitPool = CurLitPool;

   //--------------------------------------------------------------------------
   // Now create POOLITEM and it's encapsulated DATA structure for
   // the literal pool...
   //--------------------------------------------------------------------------

   PI = GMEM(sizeof(POOLITEM) + Length);
   PI->Length          = Length;
   memcpy(PI->Item, Table, Length);

   //--------------------------------------------------------------------------
   // Link POOLITEM onto literal pool chain...
   //--------------------------------------------------------------------------

   if (LiteralHead == NULL ) {
      LiteralHead = LiteralTail = PI;
   } else {
      LiteralTail->Next = PI;
      LiteralTail       = PI;
   }

   CurLitPool += PI->Length;           // Update current pool offset.

   return SaveCurLitPool;              // Return offset into literal pool.
}



//-----------------------------------------------------------------------------
// SymRefer() -- Add a symbol reference to symbol table...
//-----------------------------------------------------------------------------

SYMREF  * SymRefer(SYMTAB *Symbol, int Type)
{
   SYMREF   *SymRef;


   SymRef = GMEM(sizeof(SYMREF));       // Get memory for new SYMREF.

   SymRef->Type     = Type;             // Save reference type.
   SymRef->Line     = Line;             // Save Line number.
   SymRef->Column   = Column;           // Save Column number.
   SymRef->PCodeOff = CurCodePool;      // Save current pCode offset.

   if (Symbol->Flags & SYMBOL_RESOLVED)
      SymRef->Flags |= REFER_RESOLVED;

   switch (Type) {

      case REFER_LITERAL:

         SymRef->Next   = Symbol->LitRef;
         Symbol->LitRef = SymRef;
         break;

      case REFER_LABEL:

         SymRef->Next   = Symbol->LabRef;
         Symbol->LabRef = SymRef;
         break;

      case REFER_VARIABLE:

         SymRef->Next   = Symbol->VarRef;
         Symbol->VarRef = SymRef;
         break;

   }
   return SymRef;
}



//-----------------------------------------------------------------------------
// SymSetBP() -- Add entry to backpatch chain for backpatching later.
//-----------------------------------------------------------------------------

void   SymSetBP(SYMTAB *Symbol, SYMREF *Ref, POOLITEM *PI)
{

   PATCH  *BP;

   BP = GMEM(sizeof(PATCH));

   BP->Next          = Symbol->BackPatch; // Link into chain.
   Symbol->BackPatch = BP;

   BP->SymRef        = Ref;               // Save associated reference.
   BP->PoolItem      = PI;                // Save pCode's link.

}



//-----------------------------------------------------------------------------
// SymList() -- Print a symbol table listing on log...
//-----------------------------------------------------------------------------

void   SymList( void )
{
   if (CHECK_DEBUG()) {
      fprintf(ListFile, "\nSymbol Table Listing:\n");
      fprintf(ListFile, "NAME                  LINE  LOCATION\n");
      fprintf(ListFile, "--------------------  ----  --------\n");
      TreeTraverse(SymbolTree, (TRAVFUNC) SortSyms, (void*) NULL);
   }

}



//-----------------------------------------------------------------------------
// SortSyms() -- Called by tree services with each entry in tree...
//-----------------------------------------------------------------------------

static Bool SortSyms(
   void           *KeyPtr,
   unsigned short  KeyLen,
   void           *DataPtr,
   unsigned short  DataLen,
   void           *Parm)
{
   SYMTAB         *Symbol;
   int             Line;

   Symbol = (SYMTAB *) DataPtr;
   if (CHECK_DEBUG() ) {
      Line = 0;
      if (Symbol->LabDefRef != NULL) {
         Line = Symbol->LabDefRef->Line;
      }
      if (Line)
         fprintf(ListFile, "%-20s %5d      %4X\n",
                 KeyPtr,
                 Line,
                 Symbol->PoolOff);
      else
         fprintf(ListFile, "%-20s            %4X\n",
                 KeyPtr,
                 Symbol->PoolOff);
   }
   return True;
}

