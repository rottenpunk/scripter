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
//            Module:  mod.c
//
//             Title:  Generalized scripting language compiler
//
//       Description:  This file contains some utility routines.
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




//-----------------------------------------------------------------------------
// Find main() and set up IP to point to it...
//-----------------------------------------------------------------------------

int FindMain(TI* Ti, PI* Pi)
{
   USHORT   Length;
   int      i;
   PUBLICS* Pt;

   if ((Length = Pi->Header->PublicLength) == 0) {
      Ti->LastError = RC_NOMAIN;
      return RC_NOMAIN;                // Can't find main.
   }

   Pt = Pi->Publics;

   while (Length) {
      i = strlen(Pt->Name);
      if ( strcmp(Pt->Name, MAIN_FUNCTION) == 0) {
         Ti->IP = Pi->PCode + Pt->PCodeOffset;
         return RC_OK;
      }
      Pt = (PUBLICS*)((BYTE*)(Pt) + i);
      Length -= sizeof(PUBLICS) + i;
   }

   Ti->LastError = RC_NOMAIN;
   return RC_NOMAIN;
}

//-----------------------------------------------------------------------------
// Read in a pCode module and set up RunInstance...
//-----------------------------------------------------------------------------

int ReadModule(TI* Ti, PI* Pi)
{
   FILE*      FIn;

   if ((FIn = fopen(Ti->Path, "rb")) == NULL) {
      //fprintf(stderr, "Could not open pCode module: %s\n", Ti->Path);
      Ti->LastError = RC_OPENPCODE;
      return RC_OPENPCODE;
   }

   Pi->Header = GMEM(sizeof(PHEADER));
   fread(Pi->Header, 1, sizeof(PHEADER)-1, FIn);
   if (feof(FIn)) {
      fclose(FIn);
      Ti->LastError = RC_PCODEBAD;
      return RC_PCODEBAD;              // Pcode in bad format.
   }

   Pi->Name = GMEM(Pi->Header->NameLength+1);
   fread(Pi->Name, 1, Pi->Header->NameLength+1, FIn);
   if (feof(FIn)) {
      fclose(FIn);
      Ti->LastError = RC_PCODEBAD;
      return RC_PCODEBAD;              // Pcode in bad format.
   }

   if (Pi->Header->PCodeLength) {
      Pi->PCode     = GMEM(Pi->Header->PCodeLength);
      fread(Pi->PCode, 1, Pi->Header->PCodeLength, FIn);
   }
   if (feof(FIn)) {
      fclose(FIn);
      Ti->LastError = RC_PCODEBAD;
      return RC_PCODEBAD;              // Pcode in bad format.
   }

   if (Pi->Header->LiterLength) {
      Pi->Literals  = GMEM(Pi->Header->LiterLength);
      fread(Pi->Literals, 1, Pi->Header->LiterLength, FIn);
   }
   if (feof(FIn)) {
      fclose(FIn);
      Ti->LastError = RC_PCODEBAD;
      return RC_PCODEBAD;              // Pcode in bad format.
   }

   if (Pi->Header->ExternLength) {
      Pi->Externs   = GMEM(Pi->Header->ExternLength);
      fread(Pi->Externs, 1, Pi->Header->ExternLength, FIn);
   }
   if (feof(FIn)) {
      fclose(FIn);
      Ti->LastError = RC_PCODEBAD;
      return RC_PCODEBAD;              // Pcode in bad format.
   }

   if (Pi->Header->PublicLength) {
      Pi->Publics   = GMEM(Pi->Header->PublicLength);
      fread(Pi->Publics, 1, Pi->Header->PublicLength, FIn);
   }
   if (feof(FIn)) {
      fclose(FIn);
      Ti->LastError = RC_PCODEBAD;
      return RC_PCODEBAD;              // Pcode in bad format.
   }

   fclose(FIn);

   if (Ti->Options & OPT_DEBUG) {
      printf("\npCode header...\n");
      Dump((char*)Pi->Header,   0, sizeof(PHEADER)-1, 0);
      printf("\npCode name...\n");
      Dump((char*)Pi->Name,     0, Pi->Header->NameLength+1,   0);
      printf("\npCode pool...\n");
      Dump((char*)Pi->PCode,    0, Pi->Header->PCodeLength,  0);
      printf("\nLiteral pool...\n");
      Dump((char*)Pi->Literals, 0, Pi->Header->LiterLength,  0);
      printf("\nExternals table...\n");
      Dump((char*)Pi->Externs,  0, Pi->Header->ExternLength, 0);
      printf("\nPublics table...\n");
      Dump((char*)Pi->Publics,  0, Pi->Header->PublicLength, 0);
   }

   return RC_OK;
}



//-----------------------------------------------------------------------------
// Parse out the source pathname and save pointer to name...
//-----------------------------------------------------------------------------
void SetName( char *Arg, char *Saved, int MaxLength)
{
   int   i;
   char* Name = NULL;

   i = strlen(Arg) - 1;

   while (i > 0) {
      if (Arg[i] == '.') {
         Arg[i] = '\0';
      }
      if (Arg[i] == '\\' || Arg[i] == ':') {
         Name = &(Arg[i+1]);
      }

      i--;
   }
   if (Name == NULL)
      Name = Arg;

   strncpy(Saved, Name, MaxLength-1);     // Copy the name.
   Saved[MaxLength-1] = '\0';
}
