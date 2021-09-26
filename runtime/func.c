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
//            Module:  func.c
//
//             Title:  Generalized scripting language executor
//
//       Description:  This file contains routines to interface with
//                     external functions.
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


struct t_DataOne {
   DATA  Data;
   BYTE  Pad[1];
} _DataOne = {
  {DATA_STRING | DATA_INT, 1, (char*)1, "1"},
  0
};
DATA* DataOne = (DATA*) &_DataOne;

DATA  _DataInt = {DATA_INT, 0, 0, ""};
DATA* DataInt = &_DataInt;


//-----------------------------------------------------------------------------
// External Function table...
//-----------------------------------------------------------------------------

int FN_PrintOne( int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Printf(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Fprintf(  int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Gets(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Atox(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Atoi(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Atol(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Dump(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Trim(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Date(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Time(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Debug(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_System(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Remove(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Rename(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_VarSetSave(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_VarSetRestore( int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);

int FN_Strcpy(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strncpy(  int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strlen(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strspn(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strcmp(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strncmp(  int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strlwr(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strupr(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strcat(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strncat(  int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strtok(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Strchr(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Stridx(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);

int FN_Fopen(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Fclose(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Fread(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Fwrite(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Fputc(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Fgetc(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Fseek(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Feof(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Ferror(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Fputs(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Fgets(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Fdup(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);

#ifdef PMOS
int FN_InByte(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_OutByte(  int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_Snooze(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_SysTime(  int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_TraTime(  int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_GetDate(  int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
#endif

#ifdef HAPI
int FN_HapiOption(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_HapiStart(     int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_HapiStop(      int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_HapiConnect(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_HapiDisconnect(int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_HapiWaitFor(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_HapiSend(      int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_HapiReceive(   int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
#endif

#ifdef WIN32
// Win32 functions...
int FN_Sleep(         int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
int FN_MessageBox(    int ParmCount, DATA* Parms[], DATA** Result, void* VarHandle);
#endif


typedef Bool (*FUNCTION) ( int,
                           DATA**,
                           DATA**,
                           void*);


typedef struct _Functions {
   char*     Name;
   FUNCTION  Function;
   char      ParmCount;
   char      ParmTypes[4];
} FUNC;


FUNC  FuncTable[] = {

 //  function              nbr parm 1       parm 2       parm 3      parm 4
 //  name      function  parms type         type         type        type
 //  --------- ----------- --- -----------  ------------ ----------- ----------
   {"print",   FN_PrintOne, 1, 0,           0,           0,           0 },
   {"printf",  FN_Printf,   0, 0,           0,           0,           0 },
   {"fprintf", FN_Fprintf,  0, 0,           0,           0,           0 },
   {"gets",    FN_Gets,     0, 0,           0,           0,           0 },
   {"atox",    FN_Atox,     1, DATA_STRING, 0,           0,           0 },
   {"atoi",    FN_Atoi,     1, 0,           0,           0,           0 },
   {"atol",    FN_Atol,     1, 0,           0,           0,           0 },
   {"dump",    FN_Dump,     3, DATA_STRING, DATA_INT,    DATA_INT,    0 },
   {"trim",    FN_Trim,     1, DATA_STRING, 0,           0,           0 },
   {"date",    FN_Date,     0, 0,           0,           0,           0 },
   {"time",    FN_Time,     0, 0,           0,           0,           0 },
   {"debug",   FN_Debug,    1, DATA_STRING, 0,           0,           0 },
   {"system",  FN_System,   1, DATA_STRING, 0,           0,           0 },
   {"remove",  FN_Remove,   1, DATA_STRING, 0,           0,           0 },
   {"rename",  FN_Rename,   2, DATA_STRING, DATA_STRING, 0,           0 },
   {"VarSetSave",       FN_VarSetSave,
                            0, 0,           0,           0,           0 },
   {"VarSetRestore",    FN_VarSetRestore,
                            0, 0,           0,           0,           0 },

   {"strcpy",  FN_Strcpy,   2, DATA_STRING, DATA_STRING, 0,           0 },
   {"strncpy", FN_Strncpy,  3, DATA_STRING, DATA_STRING, DATA_INT,    0 },
   {"strchr",  FN_Strchr,   2, DATA_STRING, DATA_STRING, 0,           0 },
   {"strcmp",  FN_Strcmp,   2, DATA_STRING, DATA_STRING, 0,           0 },
   {"strncmp", FN_Strncmp,  3, DATA_STRING, DATA_STRING, DATA_INT,    0 },
   {"strcat",  FN_Strcat,   2, DATA_STRING, DATA_STRING, 0,           0 },
   {"strncat", FN_Strncat,  3, DATA_STRING, DATA_STRING, DATA_INT,    0 },
   {"strlen",  FN_Strlen,   1, DATA_STRING, 0,           0,           0 },
   {"strspn",  FN_Strspn,   2, DATA_STRING, DATA_STRING, 0,           0 },
   {"strlwr",  FN_Strlwr,   1, DATA_STRING, 0,           0,           0 },
   {"strupr",  FN_Strupr,   1, DATA_STRING, 0,           0,           0 },
   {"strtok",  FN_Strtok,   2, DATA_STRING, DATA_STRING, 0,           0 },
   {"stridx",  FN_Stridx,   3, DATA_STRING, DATA_INT,    DATA_INT,    0 },

   {"fopen",   FN_Fopen,    2, DATA_STRING, DATA_STRING, 0,           0 },
   {"fclose",  FN_Fclose,   1, DATA_INT,    0,           0,           0 },
   {"fread",   FN_Fread,    4, DATA_STRING, DATA_INT,    DATA_INT,    DATA_INT},
   {"fwrite",  FN_Fwrite,   4, DATA_STRING, DATA_INT,    DATA_INT,    DATA_INT},
   {"fputc",   FN_Fputc,    2, 0,           DATA_INT,    0,           0 },
   {"fgetc",   FN_Fgetc,    1, DATA_INT,    0,           0,           0 },
   {"fseek",   FN_Fseek,    3, DATA_INT,    DATA_INT,    DATA_INT,    0 },
   {"feof",    FN_Feof,     1, DATA_INT,    0,           0,           0 },
   {"ferror",  FN_Ferror,   1, DATA_INT,    0,           0,           0 },
   {"fgets",   FN_Fgets,    3, 0,           DATA_INT,    DATA_INT,    0 },
   {"fputs",   FN_Fputs,    2, DATA_STRING, DATA_INT,    0,           0 },
   {"fdup",    FN_Fdup,     1, DATA_STRING, 0,           0,           0 },

#ifdef PMOS
   {"inb",     FN_InByte,   1, DATA_INT,    0,           0,           0 },
   {"outb",    FN_OutByte,  2, DATA_INT,    DATA_INT,    0,           0 },
   {"snooze",  FN_Snooze,   1, DATA_INT,    0,           0,           0 },
   {"system_time",
               FN_SysTime,  0, 0,           0,           0,           0 },
   {"trace_time",
               FN_TraTime,  0, 0,           0,           0,           0 },
   {"get_date_time",
               FN_GetDate,  0, 0,           0,           0,           0 },
#endif

#ifdef HAPI
   {"HapiOption",       FN_HapiOption,
                            1, DATA_INT,    0,           0,           0 },
   {"HapiStart",        FN_HapiStart,
                            2, DATA_STRING, DATA_INT,    0,           0 },
   {"HapiStop",         FN_HapiStop,
                            0, 0,           0,           0,           0 },
   {"HapiConnect",      FN_HapiConnect,
                            0, 0,           0,           0,           0 },
   {"HapiDisconnect",   FN_HapiDisconnect,
                            0, 0,           0,           0,           0 },
   {"HapiWaitFor",      FN_HapiWaitFor,
                            3, DATA_INT,    DATA_INT,    DATA_STRING, 0 },
   {"HapiSend",         FN_HapiSend,
                            3, DATA_INT,    DATA_INT,    DATA_STRING, 0 },
   {"HapiReceive",      FN_HapiReceive,
                            4, DATA_INT,    DATA_INT,    DATA_INT,    DATA_STRING},
#endif

#ifdef WIN32
   {"Sleep",   FN_Sleep,    1, DATA_INT,    0,           0,           0 },
   {"MessageBox",       FN_MessageBox,
                            0, 0,           0,           0,           0 },
#endif

   {NULL,      NULL,        0, 0,           0,           0,           0 }
};


//-----------------------------------------------------------------------------
// Local routines...
//-----------------------------------------------------------------------------

static int  FindFuncEntry( char* Name, int ParmCount, FUNC** BifEntry );



//-----------------------------------------------------------------------------
// Call an external function...
//-----------------------------------------------------------------------------

int    ExternFunction( char*   Name,       // Function name.
                       int     ParmCount,  // Passed parm count.
                       DATA**  Parms,      // Array of passed parms.
                       DATA**  Data1,      // Returned value.
                       void*   VarHandle)  // Handle for variables.
{
   FUNC*      FuncEntry;
   int        rc;
   int        i;
   int        j;


   //--------------------------------------------------------------------------
   // See if function is in function table...
   //--------------------------------------------------------------------------
   if ((rc = FindFuncEntry( Name,
                            ParmCount,
                            &FuncEntry )) == RC_OK) {

      //-----------------------------------------------------------------------
      // Verify parm count...
      //-----------------------------------------------------------------------
      if (FuncEntry->ParmCount) {  // Should we check parm count?

         if (FuncEntry->ParmCount < ParmCount)
            return RC_TOOMANYPARMS;

         if (FuncEntry->ParmCount > ParmCount)
            return RC_TOOFEWPARMS;

         //--------------------------------------------------------------------
         // Verify parm types. Check only the first 4. Parms are in
         // reverse order...
         //--------------------------------------------------------------------
         j = ParmCount - 1;
         i = 0;
         while (i < ParmCount && i < 4) {

            if (FuncEntry->ParmTypes[i] != 0)
               if ((FuncEntry->ParmTypes[i] & Parms[j]->Flag1) == 0)
               return RC_BADFUNCPARM1 + i;

            i++;
            j--;
         }
      }

      //-----------------------------------------------------------------------
      // Call function...
      //-----------------------------------------------------------------------
      rc = (FuncEntry->Function)( ParmCount,
                                  Parms,
                                  Data1,
                                  VarHandle );
   }

   return (rc);
}




//-----------------------------------------------------------------------------
// FindFuncEntry() -- Find a function table entry...
//-----------------------------------------------------------------------------

static int  FindFuncEntry( char* Name, int ParmCount, FUNC** FuncEntry )
{
   int   i;

   for (i = 0; FuncTable[i].Name != NULL; i++ ) {

      if (strcmp(FuncTable[i].Name, Name) == 0) {

         *FuncEntry = &FuncTable[i];
         return RC_OK;
      }
   }

   return RC_FUNCTNOTFOUND;
}



