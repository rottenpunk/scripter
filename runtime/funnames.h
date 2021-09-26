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
//            Module:  funname.h
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
// execute.c
//-----------------------------------------------------------------------------

int    Execute(  TI*   Ti,   PI*  Pi);
int    DataFree( DATA* Data );
DATA*  DupData(  DATA* Data );


//-----------------------------------------------------------------------------
// oper.c
//-----------------------------------------------------------------------------

int    Operation( int Operator, DATA** Result, DATA* Data1, DATA* Data2);


//-----------------------------------------------------------------------------
// errors.c
//-----------------------------------------------------------------------------

//void Error(int rc, ...);
void Error(TI* Ti);                    // Print out last error.


//-----------------------------------------------------------------------------
// mod.c
//-----------------------------------------------------------------------------

int  FindMain(TI* Ti, PI* Pi);
int  ReadModule(TI* Ti, PI* Pi);
void SetName( char *Arg, char *Saved, int MaxLength);


//-----------------------------------------------------------------------------
// util.c
//-----------------------------------------------------------------------------

DATA*  DupData(  DATA* Data );
int    DataFree( DATA* Data );


//-----------------------------------------------------------------------------
// func.c
//-----------------------------------------------------------------------------

int    ExternFunction( char*   Name,       // Function name.
                       int     ParmCount,  // Passed parm count.
                       DATA**  Data,       // Array of passed parms.
                       DATA**  Data1,      // Returned value.
                       void*   VarHandle); // Handle for variables.

