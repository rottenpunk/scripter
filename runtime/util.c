//-----------------------------------------------------------------------------
//
//
//                       GENERALIZED SCRIPTING LANGUAGE
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
//            Module:  util.c
//
//             Title:  Generalized scripting language executor
//
//       Description:  This file contains some utility fubctions.
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

#include "executor.h"





//-----------------------------------------------------------------------------
// DupData() -- Duplicate a Data structure...
//-----------------------------------------------------------------------------

DATA*  DupData( DATA* Data )
{
   DATA*  Data1;
   int    Length;

   Length = sizeof(DATA) + Data->StringLength;
   if ((Data1 = GMEM( Length )) == NULL)
      return NULL;
   memcpy(Data1, Data, Length);
   Data1->Flag1 &= ~(DATA_VARNAME | DATA_RONLY);   // Don't copy this stuff.
   return Data1;
}




//-----------------------------------------------------------------------------
// DataFree() -- Free a Data structure...
//-----------------------------------------------------------------------------

int  DataFree( DATA* Data )
{
   if (Data->Flag1 & DATA_RONLY)       // If read only, then we don't free it.
      return RC_OK;

   FMEM(Data);
   return RC_OK;
}

