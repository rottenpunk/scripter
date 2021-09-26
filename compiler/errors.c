#define _PROGNAME "errors.c"
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
//            Module:  scripter.h
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

#include "ctype.h"
#include "scripter.h"


char *ErrorMsgTable[] = {
   "",                                       //  0
   "Problem creating a symbol tree",         //  1
   "Could not add symbol into tree",         //  2
   "Maximum evaluation recursion",           //  3
   "Unbalanced Parenthesis",                 //  4
   "Expression error",                       //  5
   "Can not open input source file",         //  6
   "Can not open output pcode file",         //  7
   "Not implemented yet",                    //  8
   "Unmatched else",                         //  9
   "Lval can not be a constant",             //  10
   "General Syntax error",                   //  11
   "Duplicate label",                        //  12
   "Function parameter error",               //  13
   "Not allowed outside body of function",   //  14
   "Abrupt end of file",                     //  15
   "Expected semicolon",                     //  16
   "Invalid LVAL expression",                //  17
   "Invalid goto label",                     //  18
   "Expected left parenthesis",              //  19
   "Expected right parenthesis",             //  20
   "Expected while after do",                //  21
   "Expected a symbol",                      //  22
   "Too many external variables defined",    //  23
   "Illegal break",                          //  24
   "Illegal continue",                       //  25
   "Expected a left brace",                  //  26
   "Case value not integer",                 //  27
   "Expected colon after case/default",      //  28
   "Not in switch statement",                //  29
   "",                                       //  30
   ""
};



//-----------------------------------------------------------------------------
// Static local variables...
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Local static routine definitions for routines in this module...
//-----------------------------------------------------------------------------

static void  FindMessage( int MsgId, char *Msg, int MsgLen);
static void  FmtMsgId(    char  *Msg,
                          int    MsgId,
                          int    Instance,
                          char  *FileName);



//-----------------------------------------------------------------------------
// Error() -- Called to process an error in LastError and in
//            IntError...
//-----------------------------------------------------------------------------

void Error(void)
{
   char     Msg[80];
   char    *MsgPtr;
   char     MsgID[40];                 // ID format xxxnnn.xxxnnn
   char     MsgRC[20];                 // A place to format Internal error.
   int      MsgLen;
   int      i;


   fprintf(ListFile, "\n");
   ScanGetSource(&MsgPtr, &MsgLen);
   if (MsgLen > 0 && MsgLen < sizeof(Msg)-1) {
      strncpy(Msg, MsgPtr, MsgLen);
      Msg[MsgLen] = '\0';
      fprintf(ListFile, "%s\n", Msg);

      for (i = 0; i < Column - 1; i++)
         Msg[i] = '-';
      Msg[Column - 1] = '*';
      Msg[Column] = '\0';
      fprintf(ListFile, "%s\n", Msg);
   }

   FmtMsgId( MsgID, LastError, ErrorInstance, ErrorFile);

   MsgRC[0] = '\0';                    // Clear return code string.
   if (IntError)
      sprintf(MsgRC, ", rc = %02d", IntError);

   FindMessage(LastError, Msg, sizeof(Msg));
   fprintf(ListFile, "%s at Line %d, column %d: %s%s\n",
           MsgID, Line, Column, Msg, MsgRC);

   if (ListFile != stdout) {
      fprintf(stderr, "%s at Line %d, column %d: %s%s\n",
              MsgID, Line, Column, Msg, MsgRC);
   }

   LastError = 0;                      // Reset Error.
}



//-----------------------------------------------------------------------------
// GetMsg() -- Called to retrieve a formatted message, but not
//             display it...
//-----------------------------------------------------------------------------

void GetMsg(char *Buf, int MsgNbr, int Instance, char *FileName)
{
   char  MsgId[20];
   char  Msg[60];


   FmtMsgId( MsgId, MsgNbr, Instance, FileName);

   FindMessage(MsgNbr, Msg, sizeof(Msg));

   sprintf(Buf, "%s %s", MsgId, Msg);
}



//-----------------------------------------------------------------------------
// ErrorSet() -- Set an error code...
//-----------------------------------------------------------------------------

void  ErrorSet( int Error, int Line, char *File)
{

   ErrorCount++;                       // Keep count of errors.

   LastError     = Error;
   ErrorInstance = Line;
   memcpy(ErrorFile, File, 8);         // Save filename.

}



//-----------------------------------------------------------------------------
// ErrorSym() -- Reset symbol in error...
//-----------------------------------------------------------------------------

void  ErrorSym( ID  *Id)
{

   Line   = Id->Line;                  // For now, just set these.
   Column = Id->Column;

}



//-----------------------------------------------------------------------------
// FindMessage() -- Routine to load message table and extract error
//                  Message...
//-----------------------------------------------------------------------------

static void  FindMessage(int MsgId, char *Buf, int Len)
{


   Len--;                              // Leave room for null term.

   strncpy(Buf, ErrorMsgTable[MsgId], Len);

   Buf[Len] = '\0';

   return;
}




//-----------------------------------------------------------------------------
// FmtMsgId() -- Format message number prefix...
//-----------------------------------------------------------------------------

static void  FmtMsgId(    char  *Msg,
                          int    MsgId,
                          int    Instance,
                          char  *FileName)
{
   if (CHECK_DEBUG())
      sprintf( Msg, "Error #%03d (%s-#%d)", MsgId, FileName, Instance);
   else
      sprintf( Msg, "Error #%03d", MsgId);
}

