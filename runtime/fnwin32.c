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
//            Module:  fnwin32.c
//
//             Title:  Generalized scripting language runtime function
//
//       Description:  This file contains general functions for the
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
#include "executor.h"

#include "windows.h"





//-----------------------------------------------------------------------------
// FN_Sleep() -- Call win32 sleep() function. Pass millisecs.
//-----------------------------------------------------------------------------
int  FN_Sleep(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{

   Sleep(Parm[0]->Integer);
   *Result = DupData(DataInt);
   return RC_OK;
}



//-----------------------------------------------------------------------------
// FN_MessageBox() -- Display windows message box with message...
//-----------------------------------------------------------------------------
int  FN_MessageBox(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   unsigned int Style = MB_OK;
   int          rc;


   //--------------------------------------------------------------------------
   // Check passed parameters...
   //--------------------------------------------------------------------------
   if (ParmCount == 2 || ParmCount == 3) {

      if ( !Parm[ParmCount - 1]->Flag1 & DATA_STRING)
         return RC_BADFUNCPARM1 + ParmCount - 1;

      if ( !Parm[ParmCount - 2]->Flag1 & DATA_STRING)
         return RC_BADFUNCPARM1 + ParmCount - 2;

      if (ParmCount == 3) {
         if (Parm[0]->Flag1 & DATA_INT)
            Style = (unsigned int) Parm[0]->Integer;
         else
            return RC_BADFUNCPARM3;
      }
   }  else
         return RC_BADFUNCPARM1;

   //--------------------------------------------------------------------------
   // Now, make win32 call...
   //--------------------------------------------------------------------------

   rc = MessageBox(NULL,
                   Parm[ParmCount - 1]->String,
                   Parm[ParmCount - 2]->String,
                   Style);


   *Result = DupData(DataInt);
   (*Result)->Integer = rc;
   return RC_OK;
}
