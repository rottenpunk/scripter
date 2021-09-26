#define _PROGNAME "module.c"
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

#include <time.h>

#include "scripter.h"



//-----------------------------------------------------------------------------
// ModMake() -- Make pCode module from all the little compiler pieces.
//-----------------------------------------------------------------------------

PHEADER * ModMake( void )
{
   int       TotSize;                       // Total module size.
   PHEADER   Header;
   POOLITEM *PI;
   int       i;
   char      c;
   USHORT    PoolOffset;
   char      FmtArea[17];
   time_t    TimeNow;
   struct tm *tm_ptr;
   FILE     *Output;
   char      OutputName[MAX_FILE_NAME + 1 + 3 + 1];
   ULONG     Offset;
   BYTE      Buffer[512];

   CLEAR(Header);                           // Clear header.

   strcpy(OutputName, Path);                // Set up output pcode file name.
   i = strlen(OutputName);
   while (--i) {
      if (OutputName[i] == '.') {
         OutputName[i] = '\0';
         break;
      }
   }
   strcat(OutputName, ".p");


   if ((Output = fopen(OutputName, "wb")) == NULL) {
      SET_ERROR( RC_OBJECT_OPEN );
      return NULL;
   }


   //--------------------------------------------------------------------------
   // Allocate memory for module...
   //--------------------------------------------------------------------------
   TotSize = sizeof(PHEADER)  +             // Calculate total shared storage.
             CurCodePool  +                 // Add pCode pool size.
             CurLitPool   +                 // Add literal pool size.
             strlen(Name) +                 // Add size of program name.
             (ExtVarCount * sizeof(ExtVars[0])) + // Add size of ext tab.
             PubFunsLength;                 // Add size of Public functions tab.

   Header.Length  = TotSize;                // Total module size.
   MOVE(Header.BlockID, PHEADER_BLOCKID);   // Move in block id.


   //--------------------------------------------------------------------------
   // Set up module header...
   //--------------------------------------------------------------------------
   Header.NameLength = strlen(Name);        // Save length of name into header.

   time(&TimeNow);                          // Get time in timer units.
   tm_ptr = localtime(&TimeNow);            // Convert to local time.

   sprintf(FmtArea, "%02d/%02d/%02d", tm_ptr->tm_mon+1,
                                      tm_ptr->tm_mday,
                                      tm_ptr->tm_year);

   memcpy(Header.CompDate, FmtArea, sizeof(Header.CompDate));

   sprintf(FmtArea, "%02d:%02d:%02d", tm_ptr->tm_hour,
                                      tm_ptr->tm_min,
                                      tm_ptr->tm_sec);

   memcpy(Header.CompTime, FmtArea, sizeof(Header.CompTime));

   Header.PCodeOffset = sizeof(PHEADER) +          // pCode after program name.
                        Header.NameLength;
   Header.PCodeLength = CurCodePool;               // pCode pool size.

   if ( (Header.LiterLength = CurLitPool) ) {      // Next comes...
      Header.LiterOffset = Header.PCodeOffset +    // literal pool.
                           Header.PCodeLength;
   }

   Header.ExternLength = ExtVarCount * sizeof(ExtVars[0]);
   Header.ExternOffset = Header.LiterOffset + Header.LiterLength;

   Header.PublicOffset = Header.ExternOffset + Header.ExternLength;
   Header.PublicLength = PubFunsLength;

   fwrite(&Header, 1, sizeof(PHEADER) - 1, Output);      // Write out header.
   fwrite(Name,    1, Header.NameLength + 1, Output);    // Write out name.


   //--------------------------------------------------------------------------
   // Create pCode pool...
   //--------------------------------------------------------------------------

   PI = PCodeHead;                          // Start of pCode items.
   while (PI != NULL) {
      //for (i = 0; i < PI->Length; i++)    // Copy pCode instruction.
         // *p++ = PI->Item[i];
      fwrite(PI->Item, 1, PI->Length, Output);
      PI = PI->Next;                        // Next pool item.
   }


   //--------------------------------------------------------------------------
   // Create literal pool...
   //--------------------------------------------------------------------------

   PI = LiteralHead;                        // Start of literal items.
   while (PI != NULL) {
      //memcpy(p, PI->Item, PI->Length);    // Copy DATA item into module.
      //p += PI->Length;                    // Adjust module pointer.
      fwrite(PI->Item, 1, PI->Length, Output);
      PI = PI->Next;                        // Next pool item.
   }

   //--------------------------------------------------------------------------
   // Create externals table...
   //--------------------------------------------------------------------------

   fwrite(&ExtVars, 1, Header.ExternLength, Output);   // Write out ext tab.


   //--------------------------------------------------------------------------
   // Create public functions table...
   //--------------------------------------------------------------------------

   for (i = 0; i < PubFunsCount; i++) {

      fwrite( &(PubFuns[i].PCodeOffset),               // Output pcode offset.
              1,
              sizeof(PubFuns[0].PCodeOffset),
              Output);

      fwrite( &(PubFuns[i].Name->Literal->String),     // Output Name string.
              1,
              PubFuns[i].Name->Literal->StringLength + 1,
              Output);
   }


   //--------------------------------------------------------------------------
   // All done creating pCode file...
   //--------------------------------------------------------------------------

   fclose(Output);                     // Close pcode file.


   //--------------------------------------------------------------------------
   // Now, if debug mode, then re-open file and dump pieces to list file...
   //--------------------------------------------------------------------------

   if (CHECK_DEBUG()) {

      if ((Output = fopen(OutputName, "rb")) == NULL) {
         SET_ERROR( RC_OBJECT_OPEN );
         return NULL;
      }

      fread(Buffer, 1, sizeof(PHEADER) - 1, Output);
      fread(Buffer+sizeof(PHEADER)-1, 1, Header.NameLength + 1, Output);

      fprintf(ListFile, "\nPCode module header:\n");
      Dump(ListFile, (char *) &Buffer, 0, sizeof(PHEADER)+Header.NameLength, 0);

      fprintf(ListFile, "\nLiteral pool:\n");
      fseek(Output, Header.LiterOffset, SEEK_SET);
      Offset = 0;
      while (Header.LiterLength) {
         i = sizeof(Buffer);
         if ( (USHORT) i > Header.LiterLength)
            i = Header.LiterLength;
         fread( &Buffer, 1, i, Output);
         Dump(ListFile, Buffer, Offset, i, 0);
         Header.LiterLength -= i;
         Offset += i;
      }

      fprintf(ListFile, "\nPCode pool:\n");
      fseek(Output, Header.PCodeOffset, SEEK_SET);
      Offset = 0;
      while (Header.PCodeLength) {
         i = sizeof(Buffer);
         if ( (USHORT) i > Header.PCodeLength)
            i = Header.PCodeLength;
         fread( &Buffer, 1, i, Output);
         Dump(ListFile, Buffer, Offset, i, 0);
         Header.PCodeLength -= i;
         Offset += i;
      }

      fprintf(ListFile, "\nExternals table:\n");
      fseek(Output, Header.ExternOffset, SEEK_SET);
      Offset = 0;
      while (Header.ExternLength) {
         i = sizeof(Buffer);
         if ( (USHORT) i > Header.ExternLength)
            i = Header.ExternLength;
         fread( &Buffer, 1, i, Output);
         Dump(ListFile, Buffer, Offset, i, 0);
         Header.ExternLength -= i;
         Offset += i;
      }

      fprintf(ListFile, "\nPublic functions table:\n");
      fseek(Output, Header.PublicOffset, SEEK_SET);
      Offset = 0;
      while (Header.PublicLength) {
         fread( &PoolOffset, 1, 2, Output);
         fprintf(ListFile, "%04x  ", PoolOffset);
         i = 0;
         while(1) {
            fread( &c, 1, 1, Output);
            i++;
            if (c != '\0') {
               fprintf(ListFile, "%c", c);
            } else
               break;
         }
         fprintf(ListFile, "\n");

         Header.PublicLength -= i + sizeof(PoolOffset);
      }


      fclose(Output);
   }


   return (NULL);
}
