#define _PROGNAME "scripter.c"
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

#include "scripter.h"                  // General compiler include.

static char CPP[] = "scpp";            // Preprocessor execution name.
static char SE[]  = "se";              // Executor name.

static void  SetName( char *Arg);

static char  InputFileName[64];
static char  WrkFormattedString[64];
static char  WrkOutputFileName[100];
static char  CppPath[64];


//-----------------------------------------------------------------------------
// Main routine...
//-----------------------------------------------------------------------------

void  main(
   int         ArgC,                   // command line argument count.
   char       **ArgV )                 // command line argument ptrs.
{
   int        LoopChk = 0;
   char      *Arg;                     // Ptr into command line args.
   int        NameCnt = 0;
   FILE*      FIn;
   int        i;

   fprintf(stderr, "\nScripting Compiler Version 1.4\n\n");
   fprintf(stderr, "(C) Copyright John C. Overton, Inc. 1994, 1997-1998. All Rights Reserved.\n");
   fprintf(stderr, "Advanced Communication Development Tools, Inc\n");

   //--------------------------------------------------------------------------
   // Save program path name so that we can find preprocessor...
   //--------------------------------------------------------------------------

   strcpy(CppPath, ArgV[0]);           // Save program pathname.
   i = strlen(CppPath);                // Get length of path.
   while (--i) {
      if (CppPath[i] == '\\') {        // If last directory in path,
         i++;
         break;                        // Then break to erase name.
      }
   }
   CppPath[i] = '\0';                  // Then kill off the program name.

   //--------------------------------------------------------------------------
   // Check command line arguments...
   //--------------------------------------------------------------------------

   Options |= OPT_MESSAGE | OPT_LOG;

   while( --ArgC != 0 ) {                   // Go thru all args (decr cnt).
      ++ArgV;                               // Pnt to first/next arg.
      if( *(Arg = *ArgV) == '-' ||          // Do we have an option?
                 *Arg == '/') {             // Either UNIX style or MSDOS?
         switch(*++Arg) {                   // test flag...
            case 'd':                       // Debug option.
               Options |= OPT_DEBUG;
               break;
            case 'c':                       // Compile only?
               Options |= OPT_COMPILE;
               break;
            case 'I':                       // No object output option.
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

         strcpy(InputFileName, Arg);   // Save input file name.
         Path = InputFileName;         // Save ptr to it in global struct.
         SetName(Arg);                 // Parse file name and put in global.
         if (Options & OPT_DEBUG) {
            strcpy(WrkOutputFileName, Arg);
            strcat(WrkOutputFileName, ".LST");
            if ((ListFile = fopen(WrkOutputFileName, "w")) == NULL) {
               ListFile = stdout;
            }
         }

         strcpy(WrkOutputFileName, Arg);
         strcat(WrkOutputFileName, ".TMP");
         sprintf(WrkFormattedString,
                 "%s%s %s %s",
                 CppPath,
                 CPP,
                 InputFileName,
                 WrkOutputFileName);
         system(WrkFormattedString);

         if ((FIn = fopen(WrkOutputFileName, "r")) == NULL) {
            fprintf(stderr, "Can not open input file: %s\n", Arg);
            // SET_ERROR(RC_NOSOURCEFILE);
            // Error();
            exit(1);
         }
         ScanInit();                   // Initialize Lexical Analyzer.
         ScanFile(FIn);                // Set current input file.
         SymInit();                    // Initialize Symbol table services.
         if ( CHECK_ERROR() )          // Check for errors...
            Error();                   // If error, go try to process it.
         //--------------------------------------------------------------------
         // Here, we call Parse to process statements in the source module...
         //--------------------------------------------------------------------

         Parse();                      // Parse source module...

         if ( !CHECK_ERROR() && ErrorCount == 0) {   // If no errors...
            ModMake();                 // Make pCode object file.
            SymList();                 // Create symbol listing if DEBUG.
         } else {
            fprintf(stderr, "\nTotal errors: %d\n", ErrorCount);
         }
         ScanTerm();                   // Close source module.
         remove(WrkOutputFileName);    // Delete source module.

         //if ( !(Options & OPT_COMPILE)) {  // Load and go execution?
         //
         //   strcpy(WrkOutputFileName, Arg);
         //   strcat(WrkOutputFileName, ".p");
         //   sprintf(WrkFormattedString,
         //           "%s%s %s",
         //           CppPath,
         //           SE,
         //           WrkOutputFileName);
         //   system(WrkFormattedString);
         //
         //}
      }

   }

}

//-----------------------------------------------------------------------------
// Parse out the source pathname and save pointer to name...
//-----------------------------------------------------------------------------
static void SetName( char *Arg)
{
   int  i;

   i = strlen(Arg) - 1;

   while (i > 0) {
      if (Arg[i] == '.') {
         Arg[i] = '\0';
      }
      if (Arg[i] == '\\') {
         Name = &(Arg[i+1]);
      }

      i--;
   }
   if (Name == NULL)
      Name = Arg;

}
