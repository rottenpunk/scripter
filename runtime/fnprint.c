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
//            Module:  fnprint.c
//
//             Title:  Generalized scripting language runtime function
//
//       Description:  This file contains the print() runtime function.
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

//#undef __near
//#define __near far   // Fix ctype.c so that __ctype[] is far.
#include <ctype.h>

#define NDIG   30


int  PrintfCommon(FILE* File, int ParmCount, DATA** Parm, DATA** Result, void* VarHandle);
char FAR * FAR FN_icvt( char FAR * cp, int FAR * ival );
int  FAR FN_pnum( FILE*          File,
                  unsigned long  i,                // Value to print.
                  int            f,                //
                  int            Width,
                  unsigned int   Sign,
                  unsigned int   Base,
                  int            UpCase,
                  int            IsPtr);


int  FN_PrintOne(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{

   if ((*Parm)->Flag1 & DATA_STRING) {
      printf("%s", (*Parm)->String);
   } else if ((*Parm)->Flag1 & DATA_INT) {
      printf("%ld", (*Parm)->Integer);
   }

   *Result = DupData(DataOne);

   return RC_OK;
}






int  FN_Printf(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   return PrintfCommon(stdout, ParmCount, Parm, Result, VarHandle);
}



int  FN_Fprintf(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   return PrintfCommon( (FILE*) Parm[ParmCount-1]->Integer,
                        ParmCount - 1,
                        Parm,
                        Result, 
                        VarHandle);
}



int  PrintfCommon(FILE* File, int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   int   i, j;
   char* p;
   char  Buffer[80];
   int   rc = 0;
   Bool  UpCase = False;
   int   Left;
   int   Fill;
   int   Base, Width, Prec, Sign, Len;
   Bool  IsPtr;



   i   = ParmCount - 1;
   p   = Parm[i--]->String;

   j = 0;
   while (*p != '\0') {

      if (*p != '%') {
         Buffer[j++] = *p;             // Copy char into temp buffer.
         p++;
         continue;
      } else {
         Base = 10;
         Width = 0;
         Sign = 0;
         Left = 0;
         Len = 1;                      // Number of "int" sized words.
         UpCase = 0;                   // Assume lower case hex.
         IsPtr = False;                // Assume not a pointer.

         p++;

         if (j || *p == '\0') {        // Print what's in buffer so far.
            if (j) {
               Buffer[j] = '\0';
               fprintf(File, "%s", Buffer);
            }
            j = 0;
         }

         if (*p == '\0')               // End of control string.
            break;

         if (*p == '-') {              // Left justify?
            p++;
            Left++;
         }

         if ( *p == '0' )              // Width start with 0?
            Fill++;                    // Then we may have to pad.

         if ( isdigit(*p) )            // Is width given?
            p = FN_icvt( (char FAR *)p, &Width ); // Yes, format from string.

         if (*p == '.')                // Is there a precision?
            p = FN_icvt( (char FAR *)p, &Prec);
         else
            Prec = Fill ? Width : 0;   // Otherwise calc default prec.

         if (*p == 'l') {              // Long parameter?
            if (*++p == '\0')          // Increment and check for end.
               continue;
            Len = 2;                   // Number of "int" sized words.
         }

         if (*p == '%') {              // If just % then...
               Buffer[j++] = *p;       // Copy char into temp buffer.
               break;
         }

         if (i < 0) {                  // No more parameters passed: Error!
            rc = 199;
            goto Error;
         }

         switch(*p) {

            case 's':
            case 'c':
               if (Parm[i]->Flag1 & DATA_INT) {
                  fprintf(File, "%ld", Parm[i]->Integer);
               } else {
                  fprintf(File, "%s", Parm[i]->String);
               }
               p++;
               i--;
               continue;               // Skip the rest.

            case 'd':
            case 'D':
               Sign = 1;
               break;

            case 'o':                  // Octal number.
            case 'O':
               Base = 8;
               break;

            case 'X':                  // Hex number uppercase.
               UpCase = 1;
            case 'x':                  // Hex number lower case.
               Base = 16;
               break;

            case 'P':                  // Far pointer uppercase.
               UpCase = 1;
            case 'p':                  // Far Pointer lowercase.
               Base = 16;
               Len = 2;                // 2 ints in size.
               IsPtr = True;           // Say it's a pointer.
               Width = 8;              // Force width to 8 positions.
               break;

            case 'u':                  // Unsigned int.
               break;

            default:
               fprintf(File, "%c%c", '%', *p);
               continue;
         }

         p++;

         if ( !(Parm[i]->Flag1 & DATA_INT)) {
            fprintf(File, "%s", Parm[i]->String);
            i--;
            continue;
         }

         //
         // Now format a number...
         //
         Width = FN_pnum( File,
                          Len == 1 ? (Sign ?  (long)Parm[i]->Integer  :
                                              ((unsigned long)Parm[i]->Integer ) ) :
                                     (Sign ? (long) Parm[i]->Integer  :
                                             (long) Parm[i]->Integer),
                          Prec, Width, Sign, Base, UpCase, IsPtr);

         while(Left-- > Width)         // Left justify?
            fprintf(File, " ");        // Then pad back with spaces.

         i--;
      }

   }

   if (j) {
      Buffer[j] = '\0';
      fprintf(File, "%s", Buffer);
   }

   *Result = DupData(DataInt);
   //(*Result)->Integer = Nbr of chars printed...?!?!?
Error:

   return rc;
}




//----------------------------------------------------------------------------
// Convert a number to an integer...
//----------------------------------------------------------------------------
char FAR * FAR FN_icvt( char FAR * cp, int FAR * ival )
{
    *ival = 0;
    while( isdigit(*cp) ) {             // While we have a number...
      *ival = (*ival * 10) + (*cp - '0');
        cp++;
   }
    return cp;
}

//----------------------------------------------------------------------------
// Format and output a number of some base...
//----------------------------------------------------------------------------
int  FAR FN_pnum(FILE*          File,
                 unsigned long  i,                // Value to print.
                 int            f,                //
                 int            Width,
                 unsigned int   Sign,
                 unsigned int   Base,
                 int            UpCase,
                 int            IsPtr)
{
   char*          cp;
   unsigned char  fw;
   char     buf[NDIG+1];


   if(f > NDIG)                        // Check for maximum length
      f = NDIG;

   if(Sign && (long)i < 0)             // Check sign.
      i = -(long)i;                    // make it positive.
   else
      Sign = 0;

   if(f == 0 && i == 0)
      f++;

   cp = &buf[NDIG];

   while(i || f > 0) {                 // Format each position of the number.
      if (UpCase)
         *--cp = "0123456789ABCDEF"[i%Base];
      else
         *--cp = "0123456789abcdef"[i%Base];
      i /= Base;
      f--;
   }

   fw = f = (&buf[NDIG] - cp) + Sign;
   if (fw < Width)
      fw = Width;
   while (Width-- > f)                 // Right justify?
      fprintf(File, " ");
   if (Sign) {                         // Add sign?
      fprintf(File, "-");
      f--;
   }

   if ( !IsPtr) {                      // If not a pointer being formatted...
      cp[f] = '\0';                    // Null terminate number string.
      fprintf(File, "%s", cp);         // Then print number.
   } else {                            // Format a pointer.

      while(f--) {
         if (f == 3)                   // 4 left, separate Segment and offset.
            fprintf(File, ":");
         fprintf(File, "%c", *cp++);
      }
   }
   return fw;
}


