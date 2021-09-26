#define _PROGNAME "eval.c"
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

#include "scripter.h"

//-----------------------------------------------------------------------------
// Important definitions local to this module...
//-----------------------------------------------------------------------------

#define MAX_EVAL_RECURSION  20         // Maximum recursion levels.


//-----------------------------------------------------------------------------
// Static local variables...
//-----------------------------------------------------------------------------

static ID  IDNull  = {EOM,   NULL, 0, 0}; // A temp ID for returns.
static int RecurCount = 0;                // Recursion level counter.


//-----------------------------------------------------------------------------
// Local static routine definitions for routines in this module...
//-----------------------------------------------------------------------------

static ID    Level2A(     ID   Id);
static ID    Level2B(     ID   Id);
static ID    Level3A(     ID   Id);
static ID    Level3B(     ID   Id);
static ID    Level3C(     ID   Id);
static ID    Level4A(     ID   Id);
static ID    Level4B(     ID   Id);
static ID    Level5(      ID   Id);
static ID    Level6(      ID   Id);
static ID    Level7(      ID   Id);
//static ID    Level8(      ID   Id);
static ID    Level9A(     ID   Id);
static ID    Level9B(     ID   Id);
static ID    Level10(     ID   Id);
static ID    Level11(     ID   Id);
static void  EvlFuncCall( ID   Id);



//-----------------------------------------------------------------------------
// Eval() -- Called to evaluate an expression...
//-----------------------------------------------------------------------------

Bool  Eval(Bool RVal)
{
   ID        Id;                       // Symbol type structure.
   ID        Id2;                      // Symbol type structure.
   int       Op;


   Scan( &Id );                        // Get next symbol.

   if (Id.ID == SYMBOL) {              // If expression starts with a symbol...

      Scan( &Id2 );
      switch (Id2.ID) {
         case PLUSEQ:    Op = PLUS;    break;
         case MINUSEQ:   Op = MINUS;   break;
         case MULTEQ:    Op = MULT;    break;
         case MODEQ:     Op = MOD;     break;
         case DIVEQ:     Op = DIV;     break;
         case SHREQ:     Op = SHR;     break;
         case SHLEQ:     Op = SHL;     break;
         case NEGEQ:     Op = NEG;     break;
         case OREQ:      Op = OR;      break;
         case ANDEQ:     Op = AND;     break;
         case XOREQ:     Op = XOR;     break;
         case EQ:        Op = EQ;      break;

         default:                      // Not an LVAL expression.
            ScanPushBack(&Id2);        // Push back second ID.
            goto RValExpr;             // Go process RVAL expression.
      }

      SymRefer(Id.Symbol, REFER_VARIABLE);     // Add symbol reference.
      Eval(False);                     // Evaluate expression.
      if (Op != EQ) {
         GenPCode(PVAR, Id.Symbol->PoolOff, 0);
         GenPCode(POPER, Op, 0);
      }
      GenPCode(PASSIGN, Id.Symbol->PoolOff, 0);      // Gen pCode assign.
      if (!RVal)
         GenPCode(PVAR, Id.Symbol->PoolOff, 0);      // Push it back on stack.
      return True;

   }

RValExpr:

   Id = Level2A(Id);                // Skip the LVAL expression check.
   if (RVal)
      GenPCode(PPOP, 0, 0);         // Pop off and throw away result.

   ScanPushBack( &Id );                // Push back the last token.

   return True;
}



//-----------------------------------------------------------------------------
// Logical OR...
//-----------------------------------------------------------------------------

static ID  Level2A(ID  Id)
{
   ID          Id2;                    // Symbol type structure.



   if(++RecurCount > MAX_EVAL_RECURSION) { // Check Recursion error.
      SET_ERROR(RC_ESTACK);
      return (IDNull);
   }

   Id = Level2B(Id);                   // Go to higher precedence rtn

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   while(Id.ID == LOGOR) {             // Are we to do OR, XOR?

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Get next symbol.

      if(CHECK_ERROR())                // Check for Scanner error.
         return (IDNull);              // IF TRUE then return.

      Id = Level2B(Id);                // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for Scanner error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for Scanner error.
         return (IDNull);              // IF TRUE then return.
   }

   RecurCount--;                       // Out of a recursion level.

   return(Id);                         // Return current symbol.
}



//-----------------------------------------------------------------------------
// Logical And...
//-----------------------------------------------------------------------------

static ID  Level2B(ID Id)
{
   ID         Id2;                     // Symbol type structure.


   Id = Level3A(Id);                   // Go to higher precedence rtn.

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   while(Id.ID == LOGAND) {            // Are we to do AND?

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level3A(Id);                // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.
   }

   return(Id);                         // Return current symbol.
}


//-----------------------------------------------------------------------------
// Bitwise OR...
//-----------------------------------------------------------------------------

static ID  Level3A(ID Id)
{
   ID         Id2;                     // Symbol type structure.

   Id = Level3B(Id);                   // Go to higher precedence rtn.

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   while(Id.ID == OR) {                // Are we to do OR?

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level3B(Id);                // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.
   }

   return(Id);                         // Return current symbol.
}


//-----------------------------------------------------------------------------
// Bitwise XOR...
//-----------------------------------------------------------------------------

static ID  Level3B(ID Id)
{
   ID         Id2;                     // Symbol type structure.

   Id = Level3C(Id);                   // Go to higher precedence rtn.

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   while(Id.ID == XOR) {               // Are we to do XOR?

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level3C(Id);                // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.
   }

   return(Id);                         // Return current symbol.
}


//-----------------------------------------------------------------------------
// Bitwise AND...
//-----------------------------------------------------------------------------

static ID  Level3C(ID Id)
{
   ID         Id2;                     // Symbol type structure.

   Id = Level4A(Id);                   // Go to higher precedence rtn.

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   while(Id.ID == AND) {               // Are we to do AND?

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level4A(Id);                // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.
   }

   return(Id);                         // Return current symbol.
}



//-----------------------------------------------------------------------------
// == and != operators...
//-----------------------------------------------------------------------------

static ID  Level4A(ID  Id)
{

   ID         Id2;                     // Symbol type structure.


   Id = Level4B(Id);                   // Go to higher precedence rtn.

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   while(Id.ID == EQEQ   ||            // Are we a comparison op?
         Id.ID == NOTEQ)   {

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level4B(Id);                // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.
   }

   return(Id);                         // Return current symbol.
}



//-----------------------------------------------------------------------------
// Other Comparison operators: < <= > >=
//-----------------------------------------------------------------------------

static ID  Level4B(ID  Id)
{

   ID         Id2;                     // Symbol type structure.


   Id = Level5(Id);                    // Go to higher precedence rtn.

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   while(Id.ID == GT     ||            // Are we a comparison op?
         Id.ID == LT     ||
         Id.ID == GE     ||
         Id.ID == LE) {

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level5(Id);                 // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.
   }

   return(Id);                         // Return current symbol.
}



//-----------------------------------------------------------------------------
// Shift operators: << >>
//-----------------------------------------------------------------------------

static ID  Level5(ID  Id)
{

   ID         Id2;                     // Symbol type structure.


   Id = Level6(Id);                    // Go to higher precedence rtn.

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   while(Id.ID == SHR    ||            // Are we a comparison op?
         Id.ID == SHL) {

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level6(Id);                 // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.
   }

   return(Id);                         // Return current symbol.
}



//-----------------------------------------------------------------------------
// Addition or Subtraction...
//-----------------------------------------------------------------------------

static ID  Level6(ID  Id)
{

   ID          Id2;                    // Symbol type structure.


   Id = Level7(Id);                    // Go to higher precedence rtn.

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   while(Id.ID == MINUS ||             // Are we to do plus or minus?
         Id.ID == PLUS)          {

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level7(Id);                 // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.
   }

   return(Id);                         // Return current symbol.
}



//-----------------------------------------------------------------------------
// Multiplication, Division, Integer division, Remainder division...
//-----------------------------------------------------------------------------

static ID  Level7(ID  Id)
{

   ID           Id2;                   // Symbol type structure.


   Id = Level9A(Id);                   // Go to higher precedence rtn.

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   while(Id.ID == MULT ||              // Are we to do multiply?...
         Id.ID == DIV   ||             // ...or division?...
         Id.ID == MOD)     {           // ...or remainder division?

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level9A(Id);                // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.
   }

   return(Id);                         // Return current symbol.
}







//-----------------------------------------------------------------------------
// Prefix operator: +, -, !, ~
//-----------------------------------------------------------------------------

static ID  Level9A(ID  Id)
{

   ID          Id2;                    // Symbol type structure.



   if(Id.ID == MINUS)   {              // Prefix minus?

      Id2 = Id;                        // Save symbol pointer.
      Id2.ID = UMINUS;                 // Make it UNARY MINUS.
      Scan( &Id );                     // Call Scan to get next symbol

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level9A(Id);                // Recursively check next sym.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

   } else if(Id.ID == NEG ||           // Prefix neg or not?
             Id.ID == NOT) {           // Prefix neg or not?

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Call to get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level9A(Id);                // Recursively check next sym.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(POPER, Id2.ID, 0);      // Output operation.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

   } else if (Id.ID == PLUS)   {       // Prefix Plus?

      //-----------------------------------------------------------------------
      // For UNARY pluses, just ignore them...
      //-----------------------------------------------------------------------

      Scan( &Id );                     // Call to get next symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id = Level9A(Id);                // Recursively check next sym.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

   } else {

      Id = Level9B(Id);                // Call higher precedence chk.

   }

   if(CHECK_ERROR())                   // Check for previous error.
      return (IDNull);                 // IF TRUE then return.

   return(Id);                         // Return current symbol.
}



//-----------------------------------------------------------------------------
// Increment/Decrement operators (Pre/Post)...
//-----------------------------------------------------------------------------

static ID  Level9B(ID  Id)
{
   ID          Id2;                    // Symbol type structure.
   ID          Id3;                    // Symbol type structure.
   USHORT      Oper;                   // A place to save operator.
   SYMTAB*     Symbol;                 // Pointer to symbol table entry.


   if(Id.ID == INCR ||                 // Prefix increment (++)?
      Id.ID == DECR)   {               // Prefix decrement (--)?

      if (Id.ID == INCR)               // Set correct operator.
         Oper = PLUS;
      else
         Oper = MINUS;

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Call Scan to get next symbol

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      if (Id.ID != SYMBOL) {
         SET_ERROR(RC_EXPECTSYMBOL);   // Error!
         return (IDNull);              // Return to caller.
      }

      Id3 = Id;                        // Save SYMBOL for assign.
      Id = Level11(Id);                // Go to SYMBOL level.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Symbol = SymAdd("1", 1);         // Add incr/decr value.
      Symbol->Literal->Integer  = 1;
      Symbol->Literal->Flag1   |= DATA_INT;
      GenPCode(PCONST, Symbol->PoolOff, 0);   // Output constant push.

      GenPCode(POPER, Oper, 0);        // Output operation.
      GenPCode(PASSIGN, Id3.Symbol->PoolOff, 0); // Output assign.
      GenPCode(PVAR,    Id3.Symbol->PoolOff, 0); // Output push.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      return(Id);                      // Return current symbol.
   }

   if (Id.ID != SYMBOL) {
      Id = Level10(Id);                // Call higher precedence chk.
      return(Id);                      // Return current symbol.
   }


   //--------------------------------------------------------------------------
   // Now check for Postfix ++ or --
   //--------------------------------------------------------------------------

   Scan( &Id2 );                       // Call Scan to get next symbol

   if(Id2.ID == INCR ||                // Postfix increment (++)?
      Id2.ID == DECR) {                // Postfix decrement (--)?

      if (Id2.ID == INCR)              // Set correct operator.
         Oper = PLUS;
      else
         Oper = MINUS;

      ScanPushBack(&Id2);              // Push back operator.

      Id2 = Level10(Id);               // Go to next level priority.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      GenPCode(PVAR,    Id.Symbol->PoolOff, 0); // Output push again.

      Symbol = SymAdd("1", 1);         // Add incr/decr value.
      Symbol->Literal->Integer  = 1;
      Symbol->Literal->Flag1   |= DATA_INT;
      GenPCode(PCONST, Symbol->PoolOff, 0);   // Output constant push.

      GenPCode(POPER, Oper, 0);        // Output operation.
      GenPCode(PASSIGN, Id.Symbol->PoolOff, 0); // Output assign.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Id2 = Id;                        // Save symbol pointer.
      Scan( &Id );                     // Call Scan to get next symbol

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      return(Id);                      // Return current symbol.

   }

   ScanPushBack(&Id2);                 // Pushback what ever it was.

   Id = Level10(Id);                   // Call higher precedence chk.

   return(Id);                         // Return current symbol.
}




//-----------------------------------------------------------------------------
// Parentheses?
//-----------------------------------------------------------------------------

static ID  Level10(ID  Id)
{

   if(Id.ID == LPAR) {                 // Left Parentheses?

      Eval(False);                     // Evaluate subexpression.

      Scan( &Id );                     // Get another symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      if(Id.ID != RPAR) {              // Check for close parens.
                                       // "Unmatched ( in expression".
         SET_ERROR(RC_PARENTHESIS);
         return (IDNull);              // Return to caller.
      }

      Scan( &Id );                     // Get symbol after ')'.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

   } else {

      Id = Level11(Id);                // Call higher precedence chk.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.
   }

   return(Id);                         // Return current symbol.
}



//-----------------------------------------------------------------------------
// Check for Literal String/Number or Variable name...
//-----------------------------------------------------------------------------

static ID  Level11(ID  Id)
{

   //--------------------------------------------------------------------------
   // Now we can parse a symbol...
   //--------------------------------------------------------------------------

   if( Id.ID == STRING        ||       // Are we one of these?
       Id.ID == NUMBER        ||
       Id.ID == FUNCT         ||
       Id.ID == SYMBOL)          {

      //-----------------------------------------------------------------------
      // If literal, generate PCONSTANT PCode...
      //-----------------------------------------------------------------------

      if( Id.ID == STRING   ||         // Do we have string literal?
          Id.ID == NUMBER)    {        // Do we have number lit?

         //--------------------------------------------------------------------
         // Add symbol reference...
         //--------------------------------------------------------------------

         SymRefer(Id.Symbol,           // Add reference to this lit.
                  REFER_LITERAL);      // Reference symbol type.

          if(CHECK_ERROR())            // Check for previous error.
             return (IDNull);          // IF TRUE then return.

         //--------------------------------------------------------------------
         // Gen PCode to stack constant on operand stack...
         //--------------------------------------------------------------------

         GenPCode(PCONST,              // Genereate PCONSTANT Pcode.
                  Id.Symbol->PoolOff,
                  0 );
      } else {

         //--------------------------------------------------------------------
         // If function, call EvlFuncCall to evaluate func name...
         //--------------------------------------------------------------------

         if(Id.ID == FUNCT)  {         // Do we have a function call?

            EvlFuncCall(Id);           // Go to evaluate Func call.

            if(CHECK_ERROR())          // Check for previous error.
               return (IDNull);        // IF TRUE then return.

         } else {

         //--------------------------------------------------------------------
         // If variable name generate PVARIABLE PCode...
         //--------------------------------------------------------------------

            if(Id.ID == SYMBOL){       // Variable name?

               //--------------------------------------------------------------
               // Add reference for symbol...
               //--------------------------------------------------------------

               SymRefer(Id.Symbol,     // Add reference variable name.
                        REFER_VARIABLE);  // Symbol type.

               if(CHECK_ERROR())       // Check for previous error.
                  return (IDNull);     // IF TRUE then return.

               //--------------------------------------------------------------
               // Gen PCode to stack variable's value on op stack...
               //--------------------------------------------------------------

               GenPCode(PVAR,          // Genereate PVARIABLE Pcode.
                        Id.Symbol->PoolOff,
                        0 );
            }
         }
      }

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

      Scan( &Id );                     // Get another symbol.

      if(CHECK_ERROR())                // Check for previous error.
         return (IDNull);              // IF TRUE then return.

   } else {

      //-----------------------------------------------------------------------
      // If we didn't get any symbols we have an expression error...
      //-----------------------------------------------------------------------
      SET_ERROR(RC_EXPRESSION);
      return (IDNull);
   }

   return(Id);                         // Return symbol.
}



//-----------------------------------------------------------------------------
// Handle function calls within an expression...
//-----------------------------------------------------------------------------

static void EvlFuncCall(ID  Id)

{

   ID           Id2;                   // Symbol type structure.
   int          nargs;                 // Nbr of args for function.
   SYMREF      *SymRef;                // Ptr to symbol ref entry.
   POOLITEM    *PI;                    // pCode pool item ptr.


   //--------------------------------------------------------------------------
   // Now process arguments to be passed to the function routine.
   // First, get first token after left par to see if we have a right
   // paren after it. If so we have no parameters in the list. Other-
   // wise we recusively call Eval() to process these args as expres-
   // sions. When we eventually get a close paren ')' then we can gen
   // the PFUNCT...
   //--------------------------------------------------------------------------

   nargs = 0;                          // Zero out argument counter.

   Scan( &Id2 );                       // Get next token after LPAR.
   if(CHECK_ERROR())                   // If error, just return.
      return;                          // Error will sift down.

   if(Id2.ID != RPAR) {                // If not right parens then
                                       // Go ahead and process parms.
      ScanPushBack( &Id2 );            // Push back that last token.

      while(1) {
         if(Eval(False) == FALSE)      // Call Eval() to stack Expr.
            break;                     // If no expression then break.

         if(CHECK_ERROR())             // If error, just return.
            return;                    // Error will sift down.

         nargs++;                      // Bump up argument counter.

         Scan( &Id2 );                 // Get expression stopper.

         if(CHECK_ERROR())             // If error, just return.
            return;                    // Error will sift down.

         if(Id2.ID == RPAR)            // If reached end of parms...
            break;                     // Out.

         if(Id2.ID == SEMIC) {
            SET_ERROR(RC_PARENTHESIS);
            return;
         }

         if(Id2.ID != COMMA) {
            SET_ERROR(RC_EXPRESSION);
            return;
         }
      }
   }

   //--------------------------------------------------------------------------
   // Add symbol to symbol table...
   //--------------------------------------------------------------------------

   SymRef = SymRefer(Id.Symbol,        // Add reference to function.
                     REFER_LABEL);     // Symbol type.

   if(CHECK_ERROR())                   // If error, just return.
      return;                          // Error will sift down.

   //--------------------------------------------------------------------------
   // Has function name been resolved? (Internal function). Must also
   // generate PCALL instruction.
   //--------------------------------------------------------------------------
   if(Id.Symbol->Flags & SYMBOL_RESOLVED) {

      SymRef->Flags |= REFER_RESOLVED; // Indicate it's resolved.

      GenPCode(PCALL,                  // Generate Function call.
               nargs,                  // Nbr parms on stack.
               Id.Symbol->LabDefRef->PCodeOff);

      return;                          // Return. Error may be set.
   }

   //--------------------------------------------------------------------------
   // Assume for now that Function is an external function...
   //--------------------------------------------------------------------------

   PI = GenPCode(PCALLEX,              // Generate External call.
                 nargs,                // Nbr of parms on stack.
                 Id.Symbol->PoolOff);  // Offset to function name.

   SymSetBP(Id.Symbol, SymRef, PI);    // Set to backpatch if resolved

   return;                             // Return. Error may be set.
}
