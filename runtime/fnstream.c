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
//            Module:  fnstream.c
//
//             Title:  Generalized scripting language function
//
//       Description:  This file contains standard i/o file routines.
//
//            Author:  J.C. Overton
//
//              Date:  12/26/97
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
// Open file...
//
// FILE* fopen(char *filename, char *mode);
//
//-----------------------------------------------------------------------------
int  FN_Fopen(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   //--------------------------------------------------------------------------
   // Try to open file now and return with file pointer (or null)...
   //--------------------------------------------------------------------------
   *Result = GMEM(sizeof(DATA));
   (*Result)->Integer = (long) fopen(Parm[1]->String, Parm[0]->String);
   (*Result)->Flag1 = DATA_INT;

   return RC_OK;
}


//-----------------------------------------------------------------------------
// Close File...
//
// int fclose(FILE *stream);
//
//-----------------------------------------------------------------------------
int  FN_Fclose(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   //--------------------------------------------------------------------------
   // Try to close file now...
   //--------------------------------------------------------------------------
   *Result = DupData(DataInt);
   (*Result)->Integer = (long) fclose( (FILE*) Parm[0]->Integer);

   return RC_OK;
}


//-----------------------------------------------------------------------------
// Read from File...
//
// size_t fread( void *buffer, size_t size, size_t count, FILE *stream);
//
//-----------------------------------------------------------------------------
int  FN_Fread(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   DATA*  Output;
   int    rc = RC_OK;

   //--------------------------------------------------------------------------
   // Get DATA string to read into...
   //--------------------------------------------------------------------------
   Output = (DATA*) GMEM(sizeof(DATA) +
                        (Parm[2]->Integer * Parm[1]->Integer) );

   *Result = DupData(DataInt);

   //--------------------------------------------------------------------------
   // Now do the read...
   //--------------------------------------------------------------------------
   (*Result)->Integer = (long) fread( (void*)  Output->String,
                                      (size_t) Parm[2]->Integer,
                                      (size_t) Parm[1]->Integer,
                                      (FILE*)  Parm[0]->Integer );

   //--------------------------------------------------------------------------
   // Store back the read data...
   //--------------------------------------------------------------------------
   if (Parm[3]->Flag1 & DATA_VARNAME) {

      Output->Flag1        = DATA_STRING;         // Indicate it's a string.
      Output->StringLength = (*Result)->Integer;  // Copy length of string.

      rc = VarAssign( VarHandle,
                      Parm[3]->VarName,
                      Output );
   } else
      FMEM(Output);                    // Output not used.  Free it up!

   return rc;
}



//-----------------------------------------------------------------------------
// Write to File...
//
// size_t fwrite(void *buffer, size_t size, size_t count, FILE *stream);
//
//-----------------------------------------------------------------------------
int  FN_Fwrite(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   *Result = DupData(DataInt);
   (*Result)->Integer = (long) fwrite( (void*)  Parm[3]->String,
                                       (size_t) Parm[2]->Integer,
                                       (size_t) Parm[1]->Integer,
                                       (FILE*)  Parm[0]->Integer );
   return RC_OK;
}


//-----------------------------------------------------------------------------
// Put a character to file...
//
// int fputc(int c, FILE *stream);
//
//-----------------------------------------------------------------------------
int  FN_Fputc(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   *Result = DupData(DataInt);

   //--------------------------------------------------------------------------
   // If in string format, take last character in string (usually only one
   // character), otherwise take integer...
   //--------------------------------------------------------------------------
   if (Parm[1]->Flag1 & DATA_STRING)
      (*Result)->Integer = (long) fputc( Parm[1]->String[Parm[1]->StringLength-1],
                                         (FILE*) Parm[0]->Integer );
   else
      (*Result)->Integer = (long) fputc( (char)  Parm[1]->Integer,
                                         (FILE*) Parm[0]->Integer );
   return RC_OK;
}


//-----------------------------------------------------------------------------
// Get a character from file...
//
// int fgetc(FILE *stream);
//
//-----------------------------------------------------------------------------
int  FN_Fgetc(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   char c;

   *Result = (DATA*) GMEM(sizeof(DATA) + 1);
   (*Result)->Flag1 = DATA_STRING | DATA_INT;
   c = fgetc( (FILE*) Parm[0]->Integer);
   (*Result)->Integer = (long) c;
   (*Result)->String[0] = c;
   (*Result)->StringLength = 1;

   return RC_OK;
}


//-----------------------------------------------------------------------------
// Seek to position in file...
//
// int fseek(FILE *stream, long offset, int origin);
//
//-----------------------------------------------------------------------------
int  FN_Fseek(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   *Result = DupData(DataInt);
   (*Result)->Integer = (long) fseek( (FILE*) Parm[2]->Integer,
                                              Parm[1]->Integer,
                                      (int)   Parm[0]->Integer );
   return RC_OK;
}


//-----------------------------------------------------------------------------
// Check to see if file is positioned at end...
//
// int feof(FILE *stream);
//
//-----------------------------------------------------------------------------
int  FN_Feof(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   *Result = DupData(DataInt);
   (*Result)->Integer = (long) feof( (FILE*) Parm[0]->Integer);

   return RC_OK;
}


//-----------------------------------------------------------------------------
// Check to see if file had an error...
//
// int ferror(FILE *stream);
//
//-----------------------------------------------------------------------------
int  FN_Ferror(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   *Result = DupData(DataInt);
   (*Result)->Integer = (long) ferror( (FILE*) Parm[0]->Integer);

   return RC_OK;
}


//-----------------------------------------------------------------------------
// Get a string from file...
//
// char * fgets(char *string, int length, FILE *stream);
//
//-----------------------------------------------------------------------------
int  FN_Fgets(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   int   Length;
   char* p;
   int   rc = RC_OK;

   //--------------------------------------------------------------------------
   // Allocate temporary string to read into...
   //--------------------------------------------------------------------------
   Length = (int) Parm[1]->Integer;
   p = GMEM( Length );

   //--------------------------------------------------------------------------
   // Now, read string into Data result buffer...
   //--------------------------------------------------------------------------
   fgets( p, Length, (FILE*) Parm[0]->Integer);
   Length = strlen(p); // Get actual length of string.

   //--------------------------------------------------------------------------
   // Now, allocate result string and move in string...
   //--------------------------------------------------------------------------
   *Result = (DATA*) GMEM(sizeof(DATA) + Length);
   (*Result)->StringLength = Length;
   (*Result)->Flag1       |= DATA_STRING;
   memcpy((*Result)->String, p, Length);
   FMEM(p);                            // Free temporary string.

   if (Parm[2]->Flag1 & DATA_VARNAME) {
      rc = VarAssign(VarHandle,        // Assign result to first parameter.
                     Parm[2]->VarName,
                     DupData(*Result));
   }

   return rc;
}


//-----------------------------------------------------------------------------
// put a string to file...
//
// int fputs(char *string, FILE *stream);
//
//-----------------------------------------------------------------------------
int  FN_Fputs(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   *Result = DupData(DataInt);
   (*Result)->Integer = fputs( Parm[1]->String,
                               (FILE*) Parm[0]->Integer );

   return RC_OK;
}



//-----------------------------------------------------------------------------
// Get a standard file handle: stdout, stdin, stderr, stdprn, etc...
//
// int fdup(char *string);
//
//-----------------------------------------------------------------------------
int  FN_Fdup(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{

   *Result = DupData(DataInt);

   if ( strcmp(Parm[0]->String, "stderr") == 0)
      (*Result)->Integer = (long) stderr;
   if ( strcmp(Parm[0]->String, "stdout") == 0)
      (*Result)->Integer = (long) stdout;
   if ( strcmp(Parm[0]->String, "stdin") == 0)
      (*Result)->Integer = (long) stdin;

   return RC_OK;
}
