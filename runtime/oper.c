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



int  ArithOper(  int Operator, DATA** Result, DATA* Data1, DATA* Data2);
int  StringOper( int Operator, DATA** Result, DATA* Data1, DATA* Data2);


//-----------------------------------------------------------------------------
// Operation() -- perform operation...
//-----------------------------------------------------------------------------

int  Operation( int Operator, DATA** Result, DATA* Data1, DATA* Data2)
{

   //--------------------------------------------------------------------------
   // Check if integer arithmetic operation or string operation is needed. If 
   // one or the other of the Data items is a string, then do string operation
   // but, if both are an integer, then do arithmetic operation.  Remember, a
   // Data item can be both an integer and a string - it may also be a string
   // of the formatted number...
   //--------------------------------------------------------------------------

   if ( (Data1->Flag1 & DATA_STRING || Data2->Flag1 & DATA_STRING) &&
        !(Data1->Flag1 & DATA_INT && Data2->Flag1 & DATA_INT))

      StringOper(Operator, Result, Data1, Data2);

   else

      ArithOper(Operator, Result, Data1, Data2);


   return RC_OK;
}




//-----------------------------------------------------------------------------
// ArithOper() -- Perform arithmetic operation...
//-----------------------------------------------------------------------------
int  ArithOper(  int Operator, DATA** Result, DATA* Data1, DATA* Data2)
{
   long   Integer1;
   long   Integer2;
   long   Integer3;


   Integer1 = Data1->Integer;          // Save integer 1.
   if (Data2 != NULL)                  // Is there a data 2?
      Integer2 = Data2->Integer;       // Yes, so save integer 2.

   switch (Operator) {
      case OR:
         Integer3 = Integer1 | Integer2;
         break;
      case LOGOR:
         Integer3 = Integer1 || Integer2;
         break;
      case AND:
         Integer3 = Integer1 & Integer2;
         break;
      case LOGAND:
         Integer3 = Integer1 && Integer2;
         break;
      case MINUS:
         Integer3 = Integer1 - Integer2;
         break;
      case PLUS:
         Integer3 = Integer1 + Integer2;
         break;
      case DIV:
         Integer3 = Integer1 / Integer2;
         break;
      case MULT:
         Integer3 = Integer1 * Integer2;
         break;
      case MOD:
         Integer3 = Integer1 % Integer2;
         break;
      case GT:
         Integer3 = Integer1 > Integer2;
         break;
      case LT:
         Integer3 = Integer1 < Integer2;
         break;
      case GE:
         Integer3 = Integer1 >= Integer2;
         break;
      case LE:
         Integer3 = Integer1 <= Integer2;
         break;
      case NOT:
         Integer3 = !Integer1;
         break;
      case NEG:
         Integer3 = ~Integer1;
         break;
      case EQEQ:
         Integer3 = Integer1 == Integer2;
         break;
      case SHL:
         Integer3 = Integer1 << Integer2;
         break;
      case SHR:
         Integer3 = Integer1 >> Integer2;
         break;
      case XOR:
         Integer3 = Integer1 ^ Integer2;
         break;
      case NOTEQ:
         Integer3 = Integer1 != Integer2;
         break;
      case UMINUS:
         Integer3 = -Integer1;
         break;
      default:
         return RC_BADOPERATOR;
   }

   *Result = GMEM(sizeof(DATA));       // Get space for result.
   if (*Result == NULL)                // Did we get some memory?
      return RC_NOMEMORY;
   (*Result)->Flag1 |= DATA_INT;       // Flag as Integer result.
   (*Result)->Integer = Integer3;      // Save result.
   return RC_OK;
}




//-----------------------------------------------------------------------------
// StringOper() -- perform string operation...
//-----------------------------------------------------------------------------
int  StringOper( int Operator, DATA** Result, DATA* Data1, DATA* Data2)
{
   char* Str1;
   int   Len1;
   char* Str2;
   int   Len2;
   char  TempString[40];               // A place to convert an int.
   int   TempLength;
   char* p;
   int   rc;


   if (Data2 == NULL)                  // Must have both operands.
      return RC_BADOPERAND2;

   if ( Data1->Flag1 & DATA_INT) {     // If operand 1 is an int, convert it.
      sprintf(TempString, "%d", Data1->Integer);
      Str1 = TempString;               // String 1 points to converted int.
      Len1 = strlen(TempString);
   } else {
      Str1 = Data1->String;
      Len1 = Data1->StringLength;
   }

   if ( Data2->Flag1 & DATA_INT) {     // If operand 2 is an int, convert it.
      sprintf(TempString, "%d", Data2->Integer);
      Str2 = TempString;               // String 1 points to converted int.
      Len2 = strlen(TempString);
   } else {
      Str2 = Data2->String;
      Len2 = Data2->StringLength;
   }


   switch (Operator) {
      case PLUS:
         *Result = GMEM(sizeof(DATA) + Len1 + Len2);
         if (*Result == NULL)                   // Did we get some memory?
            return RC_NOMEMORY;
         p = (*Result)->String;
         memcpy( p, Str1, Len1);
         p += Len1;
         memcpy( p, Str2, Len2);
         (*Result)->StringLength = Len1 + Len2;
         (*Result)->Flag1 |= DATA_STRING;       // Flag as string result.
         break;

      case EQEQ:
      case NOTEQ:
      case GT:
      case LT:
      case GE:
      case LE:
         *Result = DupData(DataInt);   // Get a place for boolean result.
         if (*Result == NULL)          // Did we get some memory?
            return RC_NOMEMORY;

         if (Len1 < Len2)              // Take length of shorter of strings.
            TempLength = Len1;
         else
            TempLength = Len2;

         rc = memcmp(Str1, Str2, TempLength);

         // Adjust return code if unequal lengths...
         if (rc == 0)                  // If operands are equal...
            if (Len1 != Len2)          // But lengths are not...
               if (Len1 > Len2)
                  rc = 1;              // Str1 is greater.
               else
                  rc = -1;             // Str2 is greater.

         (*Result)->Integer = 0;       // Assume False return.

         switch (Operator) {
            case EQEQ:
               if (rc == 0)
               (*Result)->Integer = 1;       // True return.
               break;
            case NOTEQ:
               if (rc != 0)
               (*Result)->Integer = 1;       // True return.
               break;
            case GT:
               if (rc > 0)
               (*Result)->Integer = 1;       // True return.
               break;
            case LT:
               if (rc < 0)
               (*Result)->Integer = 1;       // True return.
               break;
            case GE:
               if (rc >= 0)
               (*Result)->Integer = 1;       // True return.
               break;
            case LE:
               if (rc <= 0)
               (*Result)->Integer = 1;       // True return.
               break;
         }
         break;

      case SHL:
      case SHR:

      case OR:
      case AND:
      case XOR:

      case LOGOR:
      case LOGAND:

      case MINUS:
      case DIV:
      case MULT:
      case MOD:
      case NOT:
      case NEG:
      case UMINUS:
      default:
         return RC_BADOPERATOR;
   }

   return RC_OK;
}
