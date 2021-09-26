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
//            Module:  execute.c
//
//             Title:  Generalized scripting language executor
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

#include "executor.h"
#include "conio.h"         // for debugging.


#define IP_USHORT(i)  ( *((USHORT*)(&IP[i])) )
#define DATA_PTR(i)   ( (DATA *) (Literals + i))

static DATA DataNull = {DATA_STRING, 0, 0, ""};


static Bool IfFalse( DATA* Value );

char DebugBuf[80];
#define DEBUGPRT(s)    if (DebugFlag) fprintf(stderr, "%d:%d - %s\n", Line, Column, s); \
                       if (DebugFlag) gets(DebugBuf); 

static void* NullPtr = NULL;
                    


int  Execute(TI* Ti, PI* Pi)
{
   // Keep local copy for speed...
   BYTE*    IP        = Ti->IP;
   BYTE*    PCode     = Pi->PCode;
   BYTE*    Literals  = Pi->Literals;
   void*    VarHandle = Ti->VarHandle;
   int      Line      = 0;
   int      Column    = 0;
   BYTE*    CallStack[MAX_CALL_STACK];        // Stack to store return address.
   int      CallStackIndex = MAX_CALL_STACK;  // Index into call stack.
   DATA*    ValStack[MAX_VALUE_STACK];        // Value stack.
   int      ValStackIndex = MAX_VALUE_STACK;  // index into value stack.
   DATA*    Data1;
   int      rc;                        // Return code from variable function.
   int      ArgCount = 0;              // Count of passed parms to function.
   int      i;
   struct SwitchTableEntry*  SwitchTable;


   VarModuleEnter(VarHandle);          // Init module level var name space.

   //--------------------------------------------------------------------------
   // Declare external variables...
   //--------------------------------------------------------------------------
   for (i = Pi->Header->ExternLength/sizeof(USHORT); i > 0; i-- ) {
      Data1 = (DATA*) (Literals + IP_USHORT(Pi->Externs[i]));
      VarExtern(VarHandle, Data1->String, NullPtr);
   }

   //--------------------------------------------------------------------------
   // Execute pCode program...
   //--------------------------------------------------------------------------
   while (1) {

      switch ( *IP ) {                 // Switch on instruction.

         //--------------------------------------------------------------------
         // End of module.
         //--------------------------------------------------------------------
         case PEOM:
            DEBUGPRT("PEOM");
            rc = RC_OK;
            goto ModuleExit;
            break;

         //--------------------------------------------------------------------
         // Branch.
         //--------------------------------------------------------------------
         case PBRANCH:
            DEBUGPRT("PBRANCH");
            IP = PCode + IP_USHORT(1); // New instruction pointer.
            break;

         //--------------------------------------------------------------------
         // Conditional branch.
         //--------------------------------------------------------------------
         case PIF:
            DEBUGPRT("PIF");
            if (ValStackIndex == MAX_VALUE_STACK) {
               rc = RC_VALUESTACKUNDER;
               goto ModuleExit;
            }
            if ( IfFalse( ValStack[ValStackIndex++] ) )
               IP = PCode + IP_USHORT(1);
            else
               IP += PIF_LEN;          // Increment next instruction counter.
            break;

         //--------------------------------------------------------------------
         // Arithmetic operation.
         //--------------------------------------------------------------------
         case POPER:
            DEBUGPRT("POPER");
                                       // Unary operations...
            if (IP[1] == NEG || IP[1] == UMINUS || IP[1] == NOT) {
               if (ValStackIndex == MAX_VALUE_STACK) {
                  rc = RC_VALUESTACKUNDER;
                  goto ModuleExit;
               }
               rc = Operation(IP[1], &Data1, ValStack[ValStackIndex], NULL);
               DataFree( ValStack[ValStackIndex++] );

            } else {                   // Else, two arg operations.
               if (ValStackIndex >= MAX_VALUE_STACK-1) {
                  rc = RC_VALUESTACKUNDER;
                  goto ModuleExit;
               }
               rc = Operation(IP[1],
                              &Data1,
                              ValStack[ValStackIndex+1],
                              ValStack[ValStackIndex] );

               DataFree( ValStack[ValStackIndex++] );
               DataFree( ValStack[ValStackIndex++] );
            }

            if (rc)
               goto ModuleExit;

            ValStack[--ValStackIndex] = Data1;
            IP += POPER_LEN;           // Increment next instruction counter.
            break;

         //--------------------------------------------------------------------
         // Assign value to variable.
         //--------------------------------------------------------------------
         case PASSIGN:
            DEBUGPRT("PASSIGN");
            if (ValStackIndex == MAX_VALUE_STACK) {
               rc = RC_VALUESTACKUNDER;
               goto ModuleExit;
            }
            rc = VarAssign(VarHandle,
                           DATA_PTR( IP_USHORT(1))->String,
                           (void*) ValStack[ValStackIndex++] );
            if (rc)
               goto ModuleExit;
            IP += PASSIGN_LEN;         // Increment next instruction counter.
            break;

         //--------------------------------------------------------------------
         // Push a constant.
         //--------------------------------------------------------------------
         case PCONST:
            DEBUGPRT("PCONST");
            if (ValStackIndex == 0) {
               rc = RC_VALUESTACKOVER;
               goto ModuleExit;
            }
            Data1 = DATA_PTR( IP_USHORT(1));     // Point to constant.
            Data1 -> Flag1 |= DATA_RONLY;        // Mark as read only, J.I.C.
            ValStack[--ValStackIndex] = DupData(Data1);   // Save on stack.
            IP += PCONST_LEN;          // Increment next instruction counter.
            break;

         //--------------------------------------------------------------------
         // Push a variable.
         //--------------------------------------------------------------------
         case PVAR:
            DEBUGPRT("PVAR");
            if (ValStackIndex == 0) {
               rc = RC_VALUESTACKOVER;
               goto ModuleExit;
            }
            rc = VarFind( VarHandle,
                          DATA_PTR( IP_USHORT(1))->String,
                          &((void*)Data1) );

            if (rc == VAR_NOTFOUND) {    // If var not yet defined...
               rc = RC_VARUNDEFINED;     // No, it's an error!
               goto ModuleExit;
            }

            if (Data1 == NULL)              // If variable hasn't been used yet (extern or auto)...
               Data1 = &DataNull;           // Use null data value.

            if (rc != 0 && rc != VAR_NOTFOUND)
               // rc = ???
               goto ModuleExit;

            if ((Data1 = DupData(Data1)) == NULL) {
               rc = RC_NOMEMORY;
               goto ModuleExit;
            }

            // For some functions we want to allow passing variable name
            // to function for assignment.  A case such as strcpy(str1, str2);
            // copies str2 to str1. We use the following to allow str1 to
            // point back to its variable name so that str1 can be reassigned.
            // The only time this is valid is for strings only...

            if ( !(Data1->Flag1 & DATA_INT) && (Data1->Flag1 & DATA_STRING)) {
               Data1->Flag1 |= DATA_VARNAME;
               Data1->VarName = DATA_PTR( IP_USHORT(1))->String;
            }

            ValStack[--ValStackIndex] = Data1;  // Pop onto value stack.

            IP += PVAR_LEN;            // Increment next instruction counter.
            break;

         //--------------------------------------------------------------------
         // Call a subroutine.
         //--------------------------------------------------------------------
         case PCALL:
            DEBUGPRT("PCALL");
            if (CallStackIndex == 0) {
               rc = RC_CALLSTACKOVER;      // Call stack overflow.
               goto ModuleExit;
            }
            CallStack[--CallStackIndex] = IP + PCALL_LEN;
            ArgCount = IP[1];
            IP = PCode + IP_USHORT(2); // New instruction pointer.
            break;

         //--------------------------------------------------------------------
         // Call a subroutine by name.
         //--------------------------------------------------------------------
         case PCALLEX:
            DEBUGPRT("PCALLEX");

            // First, check parm count against stack...

            if (IP[1] > (MAX_VALUE_STACK - ValStackIndex)) {
               rc = RC_VALUESTACKUNDER;
               goto ModuleExit;
            }

            // Now, call external function...

            rc = ExternFunction( DATA_PTR( IP_USHORT(2))->String,
                                 IP[1],
                                 &(ValStack[ValStackIndex]),
                                 &Data1,
                                 VarHandle);

            if (rc)                    // Was there an error?
               goto ModuleExit;

            i = IP[1];                 // Adjust value stack.
            while (i--)
               DataFree( ValStack[ValStackIndex++] );

            if (ValStackIndex == 0) {  // Check if room for more.
               rc = RC_VALUESTACKOVER;
               goto ModuleExit;
            }
            ValStack[--ValStackIndex] = Data1;  // Save result on stack.

            IP += PCALLEX_LEN;         // Increment next instruction counter.
            break;

         //--------------------------------------------------------------------
         // Enter a subroutine.
         //--------------------------------------------------------------------
         case PENTER:
            DEBUGPRT("PENTER");
            if (ArgCount < IP[1]) {
               rc = RC_TOOFEWPARMS;
               goto ModuleExit;
            }
            if (ArgCount > IP[1]) {
               rc = RC_TOOMANYPARMS;
               goto ModuleExit;
            }
            VarLocalEnter(VarHandle);
            IP += PENTER_LEN;
            break;

         //--------------------------------------------------------------------
         // Return from subroutine.
         //--------------------------------------------------------------------
         case PRETURN:
            DEBUGPRT("PRETURN");
            if (CallStackIndex == MAX_CALL_STACK) { // Ret at end of program?
               rc = RC_OK;             // Yes.
               goto ModuleExit;
            }
            IP = CallStack[CallStackIndex++];
            VarLocalExit(VarHandle);          // Free local variables.
            break;

         //--------------------------------------------------------------------
         // Set current source line number.
         //--------------------------------------------------------------------
         case PLINE:
            DEBUGPRT("PLINE");
            Line   = IP_USHORT(1);
            Column = IP_USHORT(3);
            IP += PLINE_LEN;           // Increment next instruction counter.
            break;

         //--------------------------------------------------------------------
         // Define a local (automatic) variable.
         //--------------------------------------------------------------------
         case PLOCAL:
            DEBUGPRT("PLOCAL");
            rc = VarLocal(VarHandle, DATA_PTR( IP_USHORT(1))->String);
            if(rc)
               goto ModuleExit;
            IP += PLOCAL_LEN;          // Increment next instruction counter.
            break;

         //--------------------------------------------------------------------
         // Pop off a value and throw away.
         //--------------------------------------------------------------------
         case PPOP:
            DEBUGPRT("PPOP");
            if (ValStackIndex == MAX_VALUE_STACK) {
               rc = RC_VALUESTACKUNDER;
               goto ModuleExit;
            }
            DataFree( ValStack[ValStackIndex++] );
            IP += PPOP_LEN;            // Increment next instruction counter.
            break;

         //--------------------------------------------------------------------
         // Array branch.
         //--------------------------------------------------------------------
         case PSWITCH:
            DEBUGPRT("PSWITCH");
            if (ValStackIndex == MAX_VALUE_STACK) {
               rc = RC_VALUESTACKUNDER;
               goto ModuleExit;
            }
            ArgCount = IP_USHORT(1);
            SwitchTable = (struct SwitchTableEntry*)(Literals + IP_USHORT(3));
            Data1 = ValStack[ValStackIndex++];
            if ( !(Data1->Flag1 & DATA_INT)) {
               rc = RC_NOTINTEGER;
               DataFree(Data1);       // Free Data value.
               goto ModuleExit;
            }
            IP += PSWITCH_LEN;        // Default address.
            for (i = 0; i < ArgCount; i++) {
               if (((long)SwitchTable->Value) == Data1->Integer) {
                  IP = PCode + SwitchTable->PCodeOffset;
                  break;
               }
               SwitchTable++;          // Bump to next table entry.
            }
            DataFree(Data1);          // Free Data value.
            break;

         //--------------------------------------------------------------------
         // Invalid op code.
         //--------------------------------------------------------------------
         default:
            rc = RC_INVALIDOP;
            goto ModuleExit;
      }
   }

ModuleExit:
   VarModuleExit(VarHandle);

   DEBUGPRT("Free remaining stack values\n");
   while (ValStackIndex < MAX_VALUE_STACK)   // Free remaining stack values.
      DataFree(ValStack[ValStackIndex++]);

   Ti->LastError = rc;                 // Return any error.
   Ti->Line      = Line;
   Ti->Column    = Column;
   return rc;
}


//-----------------------------------------------------------------------------
// IfFalse() -- Test data value for false and return True if so.  Free Data
//              value...
//-----------------------------------------------------------------------------

static Bool IfFalse( DATA* Data )
{
   Bool result = True;

   if ( Data->Flag1 & DATA_INT )
      if (Data->Integer != 0)
         result = False;

   DataFree(Data);
   return result;
}
