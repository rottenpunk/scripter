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
//            Module:  fnstring.c
//
//             Title:  Generalized scripting language runtime function
//
//       Description:  This file contains general functions s for the
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


//-----------------------------------------------------------------------------
// FN_Strcpy() -- Simulate strcpy() c function...
//-----------------------------------------------------------------------------
int  FN_Strcpy(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   int  rc;

   *Result = GMEM(sizeof(DATA) + Parm[0]->StringLength);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = Parm[0]->StringLength;
   memcpy((*Result)->String, Parm[0]->String, Parm[0]->StringLength);

   if ( Parm[1]->Flag1 & DATA_VARNAME ) {
      rc = VarAssign(VarHandle,        // Assign result to first parameter.
                     Parm[1]->VarName,
                     DupData(*Result));
   }

   return rc;
}

//-----------------------------------------------------------------------------
// FN_Strncpy() -- Simulate strncpy() c function...
//-----------------------------------------------------------------------------
int  FN_Strncpy(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   int  rc;

   *Result = GMEM(sizeof(DATA) + Parm[1]->StringLength);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = Parm[1]->StringLength;
   memcpy((*Result)->String, Parm[1]->String, (int) Parm[0]->Integer);

   if ( Parm[1]->Flag1 & DATA_VARNAME )  {
      rc = VarAssign(VarHandle,        // Assign result to first parameter.
                     Parm[2]->VarName,
                     DupData(*Result));
   }

   return rc;
}


//-----------------------------------------------------------------------------
// FN_Strchr() -- Simulate strchr() c function...
//-----------------------------------------------------------------------------
int  FN_Strchr(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   int   i;
   char* p;

   if (Parm[0]->StringLength != 1 )
      return RC_BADFUNCPARM;

   i = Parm[1]->StringLength;
   p = Parm[1]->String;
   while (i) {
      if (*p == Parm[0]->String[0])     // Is this a match?
         break;                        // Yes. Break out to return remainder.
      p++;
      i--;
   }

   *Result = GMEM(sizeof(DATA) + i);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = i;
   if (i)
      memcpy((*Result)->String, p, i);

   return RC_OK;
}


//-----------------------------------------------------------------------------
// FN_Strcmp() -- Simulate strcmp() c function...
//-----------------------------------------------------------------------------
int  FN_Strcmp(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   *Result = DupData(DataInt);
   (*Result)->Integer = strcmp(Parm[1]->String, Parm[0]->String);

   return RC_OK;
}


//-----------------------------------------------------------------------------
// FN_Strncmp() -- Simulate strncmp() c function...
//-----------------------------------------------------------------------------
int  FN_Strncmp(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   *Result = DupData(DataInt);
   (*Result)->Integer = (long) strncmp(Parm[2]->String,
                                       Parm[1]->String,
                                       (int) Parm[0]->Integer);
   return RC_OK;
}


//-----------------------------------------------------------------------------
// FN_Strcat() -- Simulate strcat() c function...
//-----------------------------------------------------------------------------
int  FN_Strcat(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   int Length;
   int rc;

   Length = Parm[0]->StringLength + Parm[1]->StringLength;

   *Result = GMEM(sizeof(DATA) + Length);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = Length;
   memcpy((*Result)->String, Parm[1]->String, Parm[1]->StringLength);
   memcpy( & ((*Result)->String[ Parm[1]->StringLength ]),
           Parm[0]->String,
           Parm[0]->StringLength);

   if (Parm[1]->Flag1 & DATA_VARNAME) {

      rc = VarAssign(VarHandle,        // Assign result to first parameter.
                     Parm[1]->VarName,
                     DupData(*Result));
   }

   return RC_OK;
}

//-----------------------------------------------------------------------------
// FN_Strncat() -- Simulate strncat c function...
//-----------------------------------------------------------------------------
int  FN_Strncat(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   int Length;
   int rc;

   Length = (int) Parm[0]->Integer;
   if (Length > (int) Parm[1]->StringLength)
      Length = Parm[1]->StringLength;

   *Result = GMEM(sizeof(DATA) + Parm[2]->StringLength + Length);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = Parm[2]->StringLength + Length;
   memcpy((*Result)->String, Parm[2]->String, Parm[2]->StringLength);
   memcpy( & ((*Result)->String[ Parm[2]->StringLength ]),
           Parm[1]->String,
           Length);

   if (Parm[2]->Flag1 & DATA_VARNAME) {

      rc = VarAssign(VarHandle,        // Assign result to first parameter.
                     Parm[2]->VarName,
                     DupData(*Result));
   }

   return RC_OK;
}



//-----------------------------------------------------------------------------
// FN_Strlen() -- Simulate strlen() c function...
//-----------------------------------------------------------------------------
int FN_Strlen(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   *Result = DupData(DataInt);
   if (Parm[0]->Flag1 & DATA_STRING) {
      (*Result)->Integer = Parm[0]->StringLength;
   } else {
      (*Result)->Integer = 1;
   }

   return RC_OK;
}


//-----------------------------------------------------------------------------
// FN_Strspn() -- Simulate strspn() c function...
//-----------------------------------------------------------------------------
int  FN_Strspn(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   *Result = DupData(DataInt);
   (*Result)->Integer = strspn(Parm[1]->String, Parm[0]->String);

   return RC_OK;
}


//-----------------------------------------------------------------------------
// FN_Strlwr() -- Simulate strlwr() c function...
//-----------------------------------------------------------------------------
int FN_Strlwr(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   unsigned int i;
   int          rc;

   *Result = GMEM(sizeof(DATA) + Parm[0]->StringLength);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = Parm[0]->StringLength;
   memcpy((*Result)->String, Parm[0]->String, Parm[0]->StringLength);

   // Now lower case the string...
   i = 0;
   while (i < Parm[0]->StringLength) {
      if ((*Result)->String[i] >= 'A'&& (*Result)->String[i] <= 'Z')
         (*Result)->String[i] = (*Result)->String[i] + 0x20;
      i++;
   }

   if (Parm[0]->Flag1 & DATA_VARNAME) {
      rc = VarAssign(VarHandle,        // Assign result to first parameter.
                     Parm[0]->VarName,
                     DupData(*Result));
   }

   return RC_OK;
}


//-----------------------------------------------------------------------------
// FN_Strupr() -- Simulate strupr() c function...
//-----------------------------------------------------------------------------
int FN_Strupr(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   unsigned int i;
   int          rc;

   *Result = GMEM(sizeof(DATA) + Parm[0]->StringLength);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = Parm[0]->StringLength;
   memcpy((*Result)->String, Parm[0]->String, Parm[0]->StringLength);

   // Now upper case the string...
   i = 0;
   while (i < Parm[0]->StringLength) {
      if ((*Result)->String[i] >= 'a'&& (*Result)->String[i] <= 'z')
         (*Result)->String[i] = (*Result)->String[i] - 0x20;
      i++;
   }

   if (Parm[0]->Flag1 & DATA_VARNAME) {
      rc = VarAssign(VarHandle,        // Assign result to first parameter.
                     Parm[0]->VarName,
                     DupData(*Result));
   }

   return RC_OK;
}


//-----------------------------------------------------------------------------
// FN_Strtok() -- Somewhat similar to c's strtok() function but modifies
//             var1 so that we can be somewhat re-entrant.
//
//             var3 = strtok(var1, var2)
//                  var1 - is that variable to extract the next token from.
//                         var1 is modified. The first token is wacked off.
//                  var2 - is a set of separators.
//                  var3 - is the returned first token.
// In c's version of strtok(), the first call to strtok() saves the first
// parameter. In succeding calls, NULL is passed as the first parameter. We
// don't save the first parameter, but modify it so that it can be passed next
// time. If you want to save the first parameter, you should save it in another
// variable.
//-----------------------------------------------------------------------------
int  FN_Strtok(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   int     rc;
   char*   p;
   char*   s;
   char*   t;
   int     i;
   int     j;
   DATA*   Data1;

   s = Parm[1]->String;                // s is our string pointer.
   i = Parm[1]->StringLength;          // i is our residual length of the strng.

   //--------------------------------------------------------------------------
   // First, skip any leading separators to get to start of token...
   //--------------------------------------------------------------------------
   do {
      p = Parm[0]->String;             // Pnt to separators which are null termd
      do {
         if (*s == *p)                 // Is this one of the seps?
            break;                     // Yes, then skip to next pos in string.
         p++;                          // Bump to next separator.
      } while (*p != '\0');            // Check against all separators.

      if (*p == '\0')                  // This position is not a separator.
         break;                        // So break out. We have start of token.
      s++;                             // Skip all leading separators.
      i--;
   } while (i);                        // While there is more to the string.

   //--------------------------------------------------------------------------
   // Next, we have start of token, so scan for end of token...
   //--------------------------------------------------------------------------
   t = s;                              // Save start of token.
   do {
      p = Parm[0]->String;             // Pnt to separators which are null termd
      do {
         if (*s == *p)                 // Is this one of the seps?
            break;                     // Yes, then we have found end of token.
         p++;                          // Bump to next separator.
      } while (*p != '\0');            // Check against all separators.

      if (*p != '\0')                  // We found a position that's a separator.
         break;
      s++;                             // Still looking for end of token.
      i--;
   } while (i);                        // While there is more to the string.

   //--------------------------------------------------------------------------
   // Now, allocate string for result and copy into result...
   //--------------------------------------------------------------------------
   j = s - t;                          // Calculate token length.
   *Result = GMEM(sizeof(DATA) + j);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = j;
   memcpy((*Result)->String, t, j);

   //--------------------------------------------------------------------------
   // Save new parameter 1...
   //--------------------------------------------------------------------------
   if (Parm[1]->Flag1 & DATA_VARNAME) {

      Data1 = GMEM(sizeof(DATA) + i);
      Data1->Flag1  |= DATA_STRING;
      Data1->StringLength = i;
      memcpy(Data1->String, s, i);
      rc = VarAssign(VarHandle,           // Assign result to first parameter.
                     Parm[1]->VarName,
                     DupData(Data1));
   }

   return rc;
}


//-----------------------------------------------------------------------------
// FN_Stridx() -- This is a non-standard string function to return a portion
//                of the input string:
//
//  stridx(string, start offset, length);
//
//  returns: string
//
//-----------------------------------------------------------------------------
int  FN_Stridx(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   int Length;
   int Offset;
   int InputLength;

   //--------------------------------------------------------------------------
   // Verify length is ok. Else, calculate correct length...
   //--------------------------------------------------------------------------
   Length = (int) Parm[0]->Integer;       // Passed length parameter.
   Offset = (int) Parm[1]->Integer;       // Passed starting offset.
   InputLength = Parm[2]->StringLength;  // Input string length.

   if (InputLength < Length + Offset) {
      if (InputLength < Offset)
         Length = 0;
      else
         Length = InputLength - Offset;
   }

   *Result = GMEM(sizeof(DATA) + Length);
   (*Result)->Flag1        = DATA_STRING;
   (*Result)->StringLength = Length;
   memcpy((*Result)->String, &(Parm[2]->String[Offset]), Length);

   return RC_OK;
}
