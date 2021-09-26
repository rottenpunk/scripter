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
#include <time.h>




static DATA DataNull = {DATA_STRING, 0, 0, ""};

long  Atol(char *);
void  TwoDigitFormat(char* s, int i);




//-----------------------------------------------------------------------------
// FN_Gets()
//-----------------------------------------------------------------------------
int  FN_Gets(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   int   rc = RC_OK;
   char  str[81];
   int   i;

   if ( (ParmCount == 1 && !(Parm[0]->Flag1 & DATA_VARNAME)) ||
         ParmCount > 1)
      return RC_BADFUNCPARM;

   gets(str);

   i = strlen(str);
   *Result = GMEM(sizeof(DATA) + i);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = i;
   memcpy((*Result)->String, str, i);

   if (ParmCount) {
      rc = VarAssign(VarHandle,        // Assign result to first parameter.
                     Parm[0]->VarName,
                     DupData(*Result));
   }

   return rc;
}



//-----------------------------------------------------------------------------
// FN_atox() -- convert ascii number to hexadecimal...
//-----------------------------------------------------------------------------

int  FN_Atox(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   int           Length = 0;
   unsigned char c = 0;
   char*         p;
   char*         s;


   //--------------------------------------------------------------------------
   // First, count number of hexadecimal chars there are...
   //--------------------------------------------------------------------------
   p = Parm[0]->String;                // Point to string to convert.
   while(1) {
      if (*p >= '0' && *p <= '9')
         ;
      else if (*p >= 'A' && *p <= 'F')
         ;
      else if (*p >= 'a' && *p <= 'f')
         *p = *p - 0x20;               // Make uppercase.
      else break;
      Length++;                        // Count how many hexadecimal chars.
   }

   //--------------------------------------------------------------------------
   // Now get memory for result string...
   //--------------------------------------------------------------------------
   *Result = GMEM(sizeof(DATA) + (Length+1)/2);
   (*Result)->Flag1  |= DATA_INT;
   (*Result)->StringLength = (Length+1)/2;

   p = Parm[0]->String;                // Point to string to convert.
   s = (*Result)->String;

   //--------------------------------------------------------------------------
   // Convert hexadecimal string and put into result string...
   //--------------------------------------------------------------------------
   while (Length) {
      if (Length % 2)                  // If odd numbered nibble...
         c <<= 4;                      // Shift previous value to high nibble.
      if (*p >= '0' && *p <= '9')
         c |= *p & 0x0f;               // Convert 0-9.
      else
         c |= *p - 0x41 + 10;          // Convert A-F.
      if (Length % 2)                  // For Every two nibbles...
         *s++ = c;                     // Store newly formed byte.
      Length--;
      p++;
   }

   return RC_OK;
}


//-----------------------------------------------------------------------------
// FN_atolCommon() -- common conversion routine: convert ascii number to integer...
//-----------------------------------------------------------------------------

int  FN_AtolCommon(int Length,
                   int ParmCount,
                   DATA* Parm[2],
                   DATA** Result,
                   void* VarHandle)
{

   *Result = GMEM(sizeof(DATA) + Length);
   (*Result)->Flag1  |= DATA_INT;

   if (Parm[0]->Flag1 & DATA_INT)
      (*Result)->Integer = Parm[0]->Integer;
   else if (Parm[0]->Flag1 & DATA_STRING)
      (*Result)->Integer = Atol(Parm[0]->String);

   //--------------------------------------------------------------------------
   // Store integer as hex number in string...
   //--------------------------------------------------------------------------
   if (Length = 2)
      *((int*)&((*Result)->String)) = (int) (*Result)->Integer;
   else
      *((long*)&((*Result)->String)) = (*Result)->Integer;

   return RC_OK;
}


//-----------------------------------------------------------------------------
// FN_atoi() -- convert ascii number to integer...
//-----------------------------------------------------------------------------
long  Atol(char *p)
{
   long result = 0;

   while (*p != '\0') {
      if (*p >= '0' && *p <= '9') {
         result *= 10;
         result += *p - 0x30;
      } else {
         break;
      }
      p++;
   }

   return result;
}


//-----------------------------------------------------------------------------
// FN_atoi() -- convert ascii number to integer...
//-----------------------------------------------------------------------------
int  FN_Atoi(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   return FN_AtolCommon(2, ParmCount, Parm, Result, VarHandle);
}

//-----------------------------------------------------------------------------
// FN_atol() -- convert ascii number to integer...
//-----------------------------------------------------------------------------
int  FN_Atol(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   return FN_AtolCommon(4, ParmCount, Parm, Result, VarHandle);
}



//-----------------------------------------------------------------------------
// FN_Dump() -- Use existing dump function to dump some memory to screen...
//-----------------------------------------------------------------------------

int  FN_Dump(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{

   Dump( Parm[2]->String,
         Parm[1]->Integer,
         Parm[2]->StringLength,
         (int) Parm[0]->Integer );

   *Result = DupData(DataOne);

   return RC_OK;
}


//-----------------------------------------------------------------------------
// FN_Trim() -- Trim trailing spaces from a string...
//-----------------------------------------------------------------------------
int  FN_Trim(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   char* p;
   int   i;


   i = Parm[0]->StringLength;          // Get length of string.
   p = Parm[0]->String + i - 1;        // Point to end of string.
   while (i) {
      if (*p != ' ')                   // Is this a space?
         break;                        // Yes. We found end of string.
      p--;
      i--;
   }

   *Result = GMEM(sizeof(DATA) + i);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = i;
   if (i)
      memcpy((*Result)->String, Parm[0]->String, i);

   return RC_OK;
}



//-----------------------------------------------------------------------------
// FN_Date() -- Return date in mm/dd/yyyy format...
//-----------------------------------------------------------------------------
int  FN_Date(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   char   Temp[11];                    // Space to format date into.
   time_t t;
   struct tm*    ts;

   time(&t);
   ts = localtime(&t);

   TwoDigitFormat( &(Temp[0]), 1 + ts->tm_mon);
   Temp[2] = '/';
   TwoDigitFormat( &(Temp[3]), ts->tm_mday);
   Temp[5] = '/';
   TwoDigitFormat( &(Temp[6]), 19 + ts->tm_year/100 );
   TwoDigitFormat( &(Temp[8]), ts->tm_year % 100);
   Temp[10] = '\0';

   *Result = GMEM(sizeof(DATA) + 10);
   memcpy( (*Result)->String, Temp, sizeof(Temp));
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = 10;

   return RC_OK;
}



//-----------------------------------------------------------------------------
// FN_Time() -- Return time in hh/mm/ss format...
//-----------------------------------------------------------------------------
int  FN_Time(int ParmCount, DATA** Parm, DATA** Result, void* VarHandle)
{
   char   Temp[9];                     // Space to format time into.
   time_t t;
   struct tm*    ts;

   time(&t);
   ts = localtime(&t);

   TwoDigitFormat( &(Temp[0]), ts->tm_hour);
   Temp[2] = ':';
   TwoDigitFormat( &(Temp[3]), ts->tm_min);
   Temp[5] = ':';
   TwoDigitFormat( &(Temp[6]), ts->tm_sec);
   Temp[8] = '\0';

   *Result = GMEM(sizeof(DATA) + 8);
   memcpy( (*Result)->String, Temp, 8);
   (*Result)->Flag1  |= DATA_STRING;
   (*Result)->StringLength = 8;

   return RC_OK;
}




//-----------------------------------------------------------------------------
// TwoDigitFormat() -- subroutine to format date and time digits...
//-----------------------------------------------------------------------------
void  TwoDigitFormat(char* s, int i)
{
   char Digits[10] = "0123456789";

   s[1] = Digits[i%10];
   i /= 10;
   s[0] = Digits[i%10];
}



//-----------------------------------------------------------------------------
// FN_Debug() -- Set on or off debugging...
//-----------------------------------------------------------------------------

extern Bool FAR DebugFlag;

int  FN_Debug(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{

   if (strcmp(Parm[0]->String, "on") == 0)
      DebugFlag = True;

   if (strcmp(Parm[0]->String, "off") == 0)
      DebugFlag = False;

   *Result = DupData(DataInt);
   return RC_OK;
}



//-----------------------------------------------------------------------------
// FN_System() -- Execute a system command...
//-----------------------------------------------------------------------------

int  FN_System(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   int rc;

   rc = system(Parm[0]->String);

   *Result = DupData(DataInt);
   (*Result)->Integer = rc;
   return RC_OK;
}



//-----------------------------------------------------------------------------
// FN_Remove() -- Delete a file...
//-----------------------------------------------------------------------------

int  FN_Remove(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   int rc;

   rc = remove(Parm[0]->String);

   *Result = DupData(DataInt);
   (*Result)->Integer = rc;
   return RC_OK;
}



//-----------------------------------------------------------------------------
// FN_Rename() -- Rename a file...
//-----------------------------------------------------------------------------

int  FN_Rename(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   int rc;

   rc = rename(Parm[1]->String, Parm[0]->String);

   *Result = DupData(DataInt);
   (*Result)->Integer = rc;
   return RC_OK;
}



//-----------------------------------------------------------------------------
// FN_VarSetSave() -- Save a set of variables into a variable save set file...
//-----------------------------------------------------------------------------

#define SAVE_SET_ID   "VSET"


int  FN_VarSetSave(int ParmCount, DATA* Parm[2], DATA** Result, void* VarHandle)
{
   DATA* Data;
   DATA* Data2;
   FILE* File;
   char*          Name;
   unsigned short Length;
   int            rc;
   int            i;

   //--------------------------------------------------------------------------
   // First, open the output save set file...
   //--------------------------------------------------------------------------
   if ((File = fopen( Parm[ParmCount-1]->String, "wb")) == NULL)
         return 10001;

   //--------------------------------------------------------------------------
   // Write out header ID...
   //--------------------------------------------------------------------------
   fwrite( SAVE_SET_ID, 1, sizeof(SAVE_SET_ID), File);  // Read ID.

   //--------------------------------------------------------------------------
   // Now, write out all the variables...
   //--------------------------------------------------------------------------
   i = ParmCount-2;
   while (i >= 0) {

      Data = Parm[i];

      if (Data->Flag1 & DATA_VARNAME) {     // If we have pointer to variable,

         Name = Data->VarName;              // Point to variable name.
         Data2 = Data;

      } else if (Data->Flag1 & DATA_STRING) { // Must be name of variable.

         if (Data->Flag1 & DATA_INT)
            return 10002;

         rc = VarFind( VarHandle,             // Locate variable.
                       Data->String,
                       &((void*)Data2) );

         if (rc)
            Data2 = &DataNull;

         Name = Data->String;               // Point to variable name.
      }

      Length = strlen(Name) + 1;         // Calculate name length w/ null term.
      if (Length > 256)                  // Check length is valid.
         return 10003;

      fwrite((char*) &Length, 1, sizeof(Length), File);
      fwrite(Name, 1, Length, File);
      Length = sizeof(DATA) + Data2->StringLength;
      fwrite((char*) &Length, 1, sizeof(Length), File);
      fwrite((char*) Data2, 1, Length, File);

      if( ferror(File))
         return 10004;

      i--;
   }

   fclose(File);
   *Result = DupData(DataInt);
   return RC_OK;
}



//-----------------------------------------------------------------------------
// FN_VarSetRestore() -- Restore a set of variables into a variable
//                       save set file...
//-----------------------------------------------------------------------------

int  FN_VarSetRestore( int ParmCount,
                       DATA* Parm[2],
                       DATA** Result,
                       void* VarHandle)
{
   FILE*          File;
   char           Name[257];
   unsigned short Length;
   DATA*          Data;
   int            rc;
   int            i;
   char*          VarName;
   char           TempBuff[64];


   //--------------------------------------------------------------------------
   // First, open the input save set file...
   //--------------------------------------------------------------------------
   if ((File = fopen( Parm[ParmCount-1]->String, "rb")) == NULL)
         return 10011;

   //--------------------------------------------------------------------------
   // Read in and check header ID...
   //--------------------------------------------------------------------------
   fread( Name, 1, sizeof(SAVE_SET_ID), File);  // Read ID.

   if (strncmp(Name, SAVE_SET_ID, sizeof(SAVE_SET_ID)) != 0) {
      fclose(File);
      return 10012;
   }

   //--------------------------------------------------------------------------
   // Now, read in variables one at a time. Try to match each variable with
   // a passed parameter.  If there is a match, then store variable's new value
   // from the file.  If there isn't a match, then ignore variable from file
   // and read in next variable...
   //--------------------------------------------------------------------------
   while (!feof(File)) {

      fread( (char*) &Length, 1, sizeof(Length), File);
      if (feof(File))
         break;

      if (Length > sizeof(Name)) {          // Make sure name length if valid.
         fclose(File);
         return 10013;
      }

      fread( Name, 1, Length, File);        // Read in null terminated name.
      if (feof(File))
         break;

      fread( (char*) &Length,               // Read in len of Data.
             1,
             sizeof(Length),
             File);
      if (feof(File))
         break;

      //-----------------------------------------------------------------------
      // Now see if var name matches one of the passed var name parms. There
      // are two cases of passed parms.  First case, passed parm is the var
      // itself - There should be a VarName pointer to actual name.  Second
      // case, passed parm is a constant that is the name of the variable...
      //-----------------------------------------------------------------------
      i = ParmCount - 2;

      while(i >= 0) {

         Data = Parm[i];                         // Point to parameter.
         if (Data->Flag1 & DATA_VARNAME) {       // If we have ptr to variable,

            VarName = Data->VarName;             // Point to variable name.

         } else if (Data->Flag1 & DATA_STRING) { // Constant name of variable?

            if (Data->Flag1 & DATA_INT) {        // Check for integer constant.
               fclose(File);                     // Yuk!
               return 10014;                     // Ditto!
            }

            VarName = Data->String;              // Point to constant as name.
         }

         if (strcmp(Name, VarName) == 0) {       // Is this a match?

            Data = GMEM(sizeof(DATA) + Length);  // Alloc a place to read it to.
            fread((char*)Data, 1, Length, File); // Read it in.

            rc = VarAssign( VarHandle,
                            Name,
                            Data );
            if (rc) {
               fclose(File);
               return 10015 + rc * 100000;
            }

            break;                               // Break out of loop.
         }

         if (i == 0) {                           // Was this the last parameter to check?
                                                 // Then we need to skip this variable from file.
            while(Length) {                      // Skip past this data item.
               fread( &TempBuff,
                      1,                         // Read a chunk at a time.
                      Length < sizeof(TempBuff) ? Length : sizeof(TempBuff),
                      File);
               if (Length < sizeof(TempBuff))
                  break;                         // If no more to read.
               Length -= sizeof(TempBuff);       // Calculate residual to read.
            }
            break;
         }

         i--;                                    // Try next parameter.
      }
   }

   fclose(File);

   *Result = DupData(DataInt);
   return RC_OK;
}
