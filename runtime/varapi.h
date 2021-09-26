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
//            Module:  varapi.h
//
//             Title:  Generalized scripting language compiler executor
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
// Return codes...
//-----------------------------------------------------------------------------

#define VAR_OK              0          // Operator OK.
#define VAR_NOTFOUND        1          // Variable not found in any tree.
#define VAR_NOMODULELEVEL   2          // No module level established yet.
#define VAR_NOLOCALLEVEL    3          // No local level established yet.
#define VAR_TREECREATE      4          // Can't create variable tree.
#define VAR_STOREERROR      5          // Error trying to store variable.
#define VAR_NOMEMORY        6          // Out of memory.
#define VAR_NOEXTERNLEVEL   7		   // No extern level established yet.




//-----------------------------------------------------------------------------
// Function prototypes...
//-----------------------------------------------------------------------------

int VarInit(        void** Handle );
int VarTerm(        void* Handle );
int VarExtern(      void* Handle, char* Name, void* Value );
int VarAssign(      void* Handle, char* Name, void*  Value);
int VarFind(        void* Handle, char* Name, void** Value);
int VarModuleEnter( void* Handle );
int VarModuleExit(  void* Handle );
int VarLocalEnter(  void* Handle );
int VarLocalExit(   void* Handle );
int VarLocal(       void* Handle, char* Name );
int VarExternEnter( void* Handle);
int VarExternExit( void* Handle);
DATA * InitString(char *Variable, DATA *Result);
