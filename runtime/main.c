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

#include "executor.h"                  // General compiler include.



char    FileName[MAX_NAME_LENGTH];
char    FilePath[MAX_PATH_LENGTH];

Bool FAR DebugFlag = False;


//-----------------------------------------------------------------------------
// Main routine...
//-----------------------------------------------------------------------------

void  main(
   int         ArgC,                   // command line argument count.
   char       **ArgV )                 // command line argument ptrs.
{
   char      *Arg;                     // Ptr into command line args.
   TI         Ti;                      // Run instance structure.
   PI         Pi;                      // Program instance.

   int        rc;
   int        i;


   //--------------------------------------------------------------------------
   // Check command line arguments...
   //--------------------------------------------------------------------------

   CLEAR(Ti);                               // Clear run instance to zeros.
   CLEAR(Pi);                               // Clear program instance.

   while( --ArgC != 0 ) {                   // Go thru all args (decr cnt).
      ++ArgV;                               // Pnt to first/next arg.
      if( *(Arg = *ArgV) == '-' ||          // Do we have an option?
                 *Arg == '/') {             // Either UNIX style or MSDOS?
         switch(*++Arg) {                   // test flag...
            case 'd':                       // Debug option.
               Ti.Options |= OPT_DEBUG;
               DebugFlag = True;
               break;
            case 'o':                       //   append object code.
               break;
            default:
               fprintf(stderr,
                       "Invalid Flag specified on command line: %s\n",
                       *ArgV);
               exit(1);
         }

      } else {
         strncpy(FilePath, Arg, MAX_PATH_LENGTH-1);
         FilePath[MAX_PATH_LENGTH-1] = '\0';
         i = strlen(FilePath);
         if (i == 0) {
            fprintf(stderr, "No program name given\n");
            exit(1);
         }
         while (--i) {
            if (FilePath[i] == '.')
               break;
            if (FilePath[i] == '\\') {
               i = 0;
               break;
            }
         }
         if (i == 0)
            strcat(FilePath, ".p");
         SetName(Arg, FileName, MAX_NAME_LENGTH);
         Ti.Path = FilePath;
         rc = ReadModule(&Ti, &Pi);        // Read in pCode module.
         if (rc) {
            Error(&Ti);
            exit(1);
         }
         rc = VarInit( &(Ti.VarHandle));   // Init variable name space.
         rc = FindMain(&Ti, &Pi);          // Find main() entry point.
         if (rc) {
            Error(&Ti);
            exit(1);
         }
         rc = Execute(&Ti, &Pi);           // Execute pCode module.
         VarTerm(Ti.VarHandle);            // Terminate variable name space.
         if (rc)
            Error(&Ti);

      }

   }
   exit(1);
}

