#define _PROGNAME "gen.c"
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
//            Module:  gen.c
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


/*--------------------------------------------------------------------*/
/* Global variables...                                                */
/*--------------------------------------------------------------------*/



/*--------------------------------------------------------------------*/
/* Static local variables...                                          */
/*--------------------------------------------------------------------*/

char *OpCodes[] = {
   "PEOM",                      /* 00 */
   "PBRANCH",                   /* 01 */
   "PIF",                       /* 02 */
   "POPER",                     /* 03 */
   "PASSIGN",                   /* 04 */
   "PCONST",                    /* 05 */
   "PVAR",                      /* 06 */
   "PCALL",                     /* 07 */
   "PCALLEX",                   /* 08 */
   "PENTER",                    /* 09 */
   "PRETURN",                   /* 10 */
   "PLINE",                     /* 11 */
   "PLOCAL",                    /* 12 */
   "PPOP",                      /* 13 */
   "PSWITCH"                    /* 14 */
};

char *Operators[] = {                   // Relative to 101.
   NULL,       /* 101 */                // Double quote operator.        "
   NULL,       /* 102 */                // Quote operator.               '
   NULL,       /* 103 */                // Colon operator.               :
   NULL,       /* 104 */                // Simicolon operator.           ;
   NULL,       /* 105 */                // Comma operator.               ,
   NULL,       /* 106 */                // Left parenthesis operator.    (
   NULL,       /* 107 */                // Right parenthesis operator.   )
   "|",        /* 108 */                // Binary Or operator.           |
   "||",       /* 109 */                // Logical Or operator.          ||
   "|=",       /* 110 */                // Binary Or equal operator.     |=
   "&",        /* 111 */                // Binary And operator.          &
   "&&",       /* 112 */                // Logical And operator.         &&
   "&=",       /* 113 */                // Binary And equal operator.    &=
   "-",        /* 114 */                // Minus operator.               -
   "+",        /* 115 */                // Plus operator.                +
   "/",        /* 116 */                // Divide operator.              /
   "*",        /* 117 */                // Multiply operator.            *
   "%",        /* 118 */                // Modulus remainder division op %
   "=",        /* 119 */                // Equal assignment operator.    =
   ">",        /* 120 */                // Greater than operator.        >
   "<",        /* 121 */                // Less than operator.           <
   ">=",       /* 122 */                // Greater than or equal op.     >=
   "<=",       /* 123 */                // Less than or equal operator.  <=
   "!",        /* 124 */                // Logical not operator.         !
   "!=",       /* 125 */                // Not equal operator.           !=
   "~",        /* 126 */                // Unary negate operator.        ~
   "~=",       /* 127 */                // Unary negate operator.        ~=
   "==",       /* 128 */                // Equal Comparison operator.    ==
   "<<",       /* 129 */                // Shift left operator.          <<
   "<<=",      /* 130 */                // Shift left equal operator.    <<
   ">>",       /* 131 */                // Shift right operator.         >>
   ">>=",      /* 132 */                // Shift right equal operator.   >>
   "++",       /* 133 */                // Plus Plus operator.           ++
   "--",       /* 134 */                // Minus Minus operator.         --
   "+=",       /* 135 */                // Plus equal operator.          +=
   "-=",       /* 136 */                // Minus equal operator.         -=
   "*=",       /* 137 */                // Multiply equal operator.      *=
   "/=",       /* 138 */                // Divide equal operator.        /=
   "%=",       /* 139 */                // Modulus equal operator.       %=
   NULL,       /* 140 */                // Left bracket operator.        [
   NULL,       /* 141 */                // right bracket operator.       ]
   NULL,       /* 142 */                // Left brace operator.          {
   NULL,       /* 143 */                // Right brace operator.         }
   "->",       /* 144 */                // Pointer operator.             ->
   "^",        /* 145 */                // Exclusive Or operator.        ^
   "^=",       /* 146 */                // Exclusive Or equal operator.  ^=
   ".",        /* 147 */                // Period operator.              .
   "...",      /* 148 */                // Dot Dot Dot operator.         ..
   "?",        /* 149 */                // Question mark operator.       ?
   NULL,       /* 150 */                // Backslash operator.
   "$",        /* 151 */                // Dollar operator.              $
   "#",        /* 152 */                // Pound operator.               #
   " "         /* 153 */                // Space.
   "-"         /* 154 */                // Unary minus.
};



//-----------------------------------------------------------------------------
// Local static routine definitions for routines in this module...
//----------------------------------------------------------------------------

static void LinkPCode(POOLITEM *PI);
static void DumpPCode(int PCode, int P1, int P2);


//-----------------------------------------------------------------------------
// GenPCode() -- Generate a PCode instruction...
//-----------------------------------------------------------------------------


POOLITEM  *GenPCode(int PCode, int P1, int P2)
{
   POOLITEM    *PI;
   BYTE        *pByte;
   USHORT      *pUShort;


   if (CHECK_DEBUG())
      DumpPCode(PCode, P1, P2);

   switch (PCode) {


      //    +-----+
      //    | OP  |
      //    +-----+

      case PEOM:
      case PPOP:
      case PRETURN:

         PI = GMEM(sizeof(POOLITEM));
         PI->Item[0] = PCode;
         PI->Length  = 1;
         LinkPCode(PI);
         break;


      //    +-----+------+
      //    | OP  | BYTE |
      //    +-----+------+

      case POPER:
      case PENTER:

         PI = GMEM(sizeof(POOLITEM) + 1);
         PI->Item[0] = PCode;
         pByte  = (BYTE *)(&(PI->Item[1]));
         *pByte = (BYTE) P1;
         PI->Length  = 2;
         LinkPCode(PI);
         break;


      //    +-----+-----------+
      //    | OP  |   SHORT   |
      //    +-----+-----------+

      case PBRANCH:
      case PIF:
      case PASSIGN:
      case PCONST:
      case PVAR:
      case PLOCAL:

         PI = GMEM(sizeof(POOLITEM) + 2);
         PI->Item[0] = PCode;
         pUShort  = (USHORT *)(&(PI->Item[1]));
         *pUShort = (USHORT) P1;
         PI->Length  = 3;
         LinkPCode(PI);
         break;


      //    +-----+------+-----------+
      //    | OP  | BYTE |   SHORT   |
      //    +-----+------+-----------+

      case PCALL:
      case PCALLEX:

         PI = GMEM(sizeof(POOLITEM) + 3);
         PI->Item[0] = PCode;
         pByte    = (BYTE *)(&(PI->Item[1]));
         *pByte   = (BYTE) P1;
         pUShort  = (USHORT *)(&(PI->Item[2]));
         *pUShort = (USHORT) P2;
         PI->Length  = 4;
         LinkPCode(PI);
         break;


      //    +-----+-----------+-----------+
      //    | OP  |   SHORT   |   SHORT   |
      //    +-----+-----------+-----------+

      case PLINE:
      case PSWITCH:

         PI = GMEM(sizeof(POOLITEM) + 4);
         PI->Item[0] = PCode;
         pUShort  = (USHORT *)(&(PI->Item[1]));
         *pUShort = (USHORT) P1;
         pUShort  = (USHORT *)(&(PI->Item[3]));
         *pUShort = (USHORT) P2;
         PI->Length  = 5;
         LinkPCode(PI);
         break;
   }

   return PI;
}


//-----------------------------------------------------------------------------
// GenPatch() -- BackPatch a PCode branch/call instruction...
//-----------------------------------------------------------------------------


void    GenPatch(POOLITEM  *PI, int Offset)
{
   USHORT     *pUShort;

   switch (PI->Item[0]) {

      case PBRANCH:
      case PIF:
         pUShort  = (USHORT *)(&(PI->Item[1]));
         *pUShort = (USHORT) Offset;
         break;

      case PCALL:
         pUShort  = (USHORT *)(&(PI->Item[2]));
         *pUShort = (USHORT) Offset;
         break;

      case PCALLEX:
         PI->Item[0] = PCALL;
         pUShort  = (USHORT *)(&(PI->Item[2]));
         *pUShort = (USHORT) Offset;
         break;

   }
}


#if 0
//-----------------------------------------------------------------------------
// GenNull() -- Genearete a PCONST to push a null constant on stack.
//-----------------------------------------------------------------------------
void    GenNull( void )
{
   DATA  *Data;

   if ( !(Flags & GLOBAL_NULL)) {      // Is one in literal pool?

      Flags |= GLOBAL_NULL;
      Null = CurLitPool;               // Save offset to null literal.
      Data = SymLit("", 0);            // Generate a null literal.
      Data->Flag1 |= DATA_NULL;        // Flag it as such.
   }

   GenPCode(PCONST, Null, 0);          // Generate push of null lit.

   return;
}
#endif


//-----------------------------------------------------------------------------
// GetSetBP() -- Allocate a backpatch structure and chain it of
//               caller's anchor...
//-----------------------------------------------------------------------------

static PATCH *PFree = NULL;            // Anchor of free PATCH structs.

void GenSetBP(POOLITEM *PI, PATCH **Anchor)
{
   PATCH  *P;


   if ((P = PFree) != NULL ) {    /* Allocate a PATCH structure.      */
      PFree = P->Next;            /* From free chain.                 */
   } else {
      P = GMEM(sizeof(PATCH));    /* From memory.                     */
   }

   P->PoolItem = PI;
   P->Next     = *Anchor;         /* Push onto chain.                 */
   *Anchor     = P;
}


//-----------------------------------------------------------------------------
// GenFreeBP() -- Free all left over backpatch structures...
//-----------------------------------------------------------------------------

void GenFreeBP(PATCH **Anchor)
{
   PATCH  *PI;

   while (*Anchor != NULL) {
      PI       = *Anchor;         /* Unchain from anchor.             */
      *Anchor  = PI->Next;
      PI->Next = PFree;           /* Chain onto free chain.           */
      PFree    = PI;
   }
}


//-----------------------------------------------------------------------------
// GenBPAll() -- Backpatch all items on a backpatch chain. This
//               version does not use SymRef the symbol reference.
//-----------------------------------------------------------------------------

void GenBPAll(PATCH **Anchor, int Offset)
{
   PATCH  *BP;

   while (*Anchor != NULL) {      /* For each reference...            */
      BP       = *Anchor;
      *Anchor  = (*Anchor)->Next;
      GenPatch(BP->PoolItem, Offset);
      BP->Next = PFree;           /* Chain onto free chain.           */
      PFree    = BP;
   }
}


//-----------------------------------------------------------------------------
// LinkPCode() -- Link a PCode instruction onto PCode item chain...
//                Adjust current PCode pool offset.
//-----------------------------------------------------------------------------

static void LinkPCode(POOLITEM *PI)
{

   if (PCodeHead == NULL) {
      PCodeHead = PCodeTail = PI;
   } else {
      PCodeTail->Next = PI;
      PCodeTail = PI;
   }

   CurCodePool += PI->Length;      /* Bump up current PCode offset*/
}



//-----------------------------------------------------------------------------
// DumpPCode() -- If DEBUG is on, dump PCODE to log...
//----------------------------------------------------------------------------

static void DumpPCode(int PCode, int P1, int P2)
{


   switch (PCode) {


      //    +-----+
      //    | OP  |
      //    +-----+

      case PEOM:
      case PPOP:
      case PRETURN:

         fprintf(ListFile, "%08lX  %02X             %s\n",
                (ULONG) CurCodePool,
                (BYTE)  PCode,
                OpCodes[PCode]);

         break;


      //    +-----+------+
      //    | OP  | BYTE |
      //    +-----+------+

      case PENTER:

         fprintf(ListFile, "%08lX  %02X %02X          %s\n",
                (ULONG) CurCodePool,
                (BYTE)  PCode,
                (BYTE)  P1,
                OpCodes[PCode]);

         break;

      case POPER:

         fprintf(ListFile, "%08lX  %02X %02X          %s  %s\n",
                (ULONG) CurCodePool,
                (BYTE)  PCode,
                (BYTE)  P1,
                OpCodes[PCode],
                Operators[P1-DQUOTE] );

         break;


      //    +-----+-----------+
      //    | OP  |   SHORT   |
      //    +-----+-----------+

      case PBRANCH:
      case PIF:
      case PASSIGN:
      case PCONST:
      case PVAR:
      case PLOCAL:

         fprintf(ListFile, "%08lX  %02X %04X        %s\n",
                (ULONG)  CurCodePool,
                (BYTE)   PCode,
                (USHORT) P1,
                OpCodes[PCode]);

         break;


      //    +-----+------+-----------+
      //    | OP  | BYTE |   SHORT   |
      //    +-----+------+-----------+

      case PCALL:
      case PCALLEX:

         fprintf(ListFile, "%08lX  %02X %02X   %04X   %s\n",
                (ULONG)  CurCodePool,
                (BYTE)   PCode,
                (BYTE)   P1,
                (USHORT) P2,
                OpCodes[PCode]);

         break;


      //    +-----+-----------+-----------+
      //    | OP  |   SHORT   |   SHORT   |
      //    +-----+-----------+-----------+

      case PLINE:
      case PSWITCH:

         fprintf(ListFile, "%08lX  %02X %04X %04X   %s\n",
                (ULONG)  CurCodePool,
                (BYTE)   PCode,
                (USHORT) P1,
                (USHORT) P2,
                OpCodes[PCode]);

         break;
   }
}
