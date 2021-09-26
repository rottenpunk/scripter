#define _PROGNAME "parser.c"
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
//            Module:  parser.c
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
// Static local routines in this module...
//-----------------------------------------------------------------------------

static void  Body(      void);         // Process function code body.
static Bool  Function(  ID  *Id);      // Process function declaration.
static void  Assign(    ID  *Id);      // Process assignment statement.
static void  Label(     ID  *Id);      // Process label.
static void  Local(     void );        // Process local variables.
static void  Extern(    void );        // Process external variables.
static void  Return(    void );        // Process return().
static void  Goto(      void );        // Process goto.
static void  If(        void );        // Process if.
static void  While(     void );        // Process while.
static void  Do(        void );        // Process do-while.
static void  For(       void );        // Process for.
static void  Switch(    void );        // Process switch.
static void  Break(     void );        // Process break.
static void  Continue(  void );        // Process continue.
static void  PushZero(  void );        // Push constant 0 on stack.
static Bool  AutoFunCall( ID *Id);    // Check and process auto function call.


//-----------------------------------------------------------------------------
// Parse() -- Will parse the program...
//-----------------------------------------------------------------------------

void Parse(void)
{
   ID   Id;

   Scan(&Id);

   while (!CHECK_ERROR()) {

      switch ( Id.ID) {
         case EXTERN:                  // External variable definition.
            Extern();                  // Process external definition.
            break;

         case CONST:                   // Constant definition.
            break;

         case FUNCT:
            if ( Function(&Id) ) {     // Process function declaration.
               Body();                 // Process function body.
               PushZero();             // Push something to return with.
               GenPCode(PRETURN, 0, 0);      // Return from function.
            }
            break;

         case EOM:
            GenPCode( PEOM, 0, 0);     // Spit out end-of-module code.
            Flags |= GLOBAL_EOM;       // Mark end of source processing.
            return;                    // Return to caller, at EOM.

         default:
            SET_ERROR(RC_NOTBODY);     // Anything else not allowed.
            break;
      }

      if ( CHECK_ERROR() )             // Check for errors...
         Error();                      // If error, go try to process it.

      Scan(&Id);                       // Get next token.
   }

   return;
}


//-----------------------------------------------------------------------------
// Body() -- Will parse the body of a function, processing statements...
//-----------------------------------------------------------------------------
static void Body(void)
{
   ID      Id;

   while ( !CHECK_ERROR() ) {          // While there's no error...

      do {
         Scan( &Id );                  // Get next symbol from source.
      } while (Id.ID == SEMIC);        // Ignore end of lines.

      if (Id.ID == RBRACE)             // End of Body.
         return;

      if ( Statement(&Id) )            // Parse a single statement.
         break;                        // End of source processing?

      if ( CHECK_ERROR() )             // Check for errors...
         Error();                      // If error, go try to process it.

      Flags |= GLOBAL_FIRST;           // Set first statement already done.
   }

}


//-----------------------------------------------------------------------------
// Statement() -- Will parse a single source statement...
//-----------------------------------------------------------------------------

Bool Statement( ID* Id )
{

   GenPCode(PLINE, LINE(), COLUMN());  // Generate LINE pCode.

   switch (Id->ID) {                   // Process statement...

      case EOM:
         SET_ERROR(RC_ABRUPTEOF);
         ScanPushBack(Id);
         return True;
         break;

      case LBRACE:
         Body();
         return False;
         break;

      case BREAK:
         Break();
         break;

      case CASE:
         SET_ERROR(RC_NOTINSWITCH);
         break;

      case CONTINUE:
         Continue();
         break;

      case DO:
         Do();
         return FALSE;
         break;

      case FOR:
         For();
         return FALSE;
         break;

      case GOTO:
         Goto();
         break;

      case SWITCH:
         Switch();
         return FALSE;
         break;

      case WHILE:
         While();
         return FALSE;
         break;

      case IF:
         If();
         return FALSE;
         break;

      case RETURN:
         Return();
         break;

      case ELSE:
         SET_ERROR(RC_ELSE);
         break;

      case SYMBOL:                     // We may have a PassScript extension...

          if ( !AutoFunCall(Id) ) {    // Check to see if this is a func call.
                                       // And, if not...
            ScanPushBack(Id);          // Push back function call for Eval().
            Eval(True);                // and go evaluate an LVAL expression.
         }
         break;

      case STRING:
         SET_ERROR(RC_CONSTANT);       // Statement cant start with constant.
         break;

      case NUMBER:
         SET_ERROR(RC_CONSTANT);       // Statement cant start with constant.
         break;

      case LABEL:
         Label(Id);                    // Go process label.
         return False;                 // No semicolon check.
         break;

      case FUNCT:
         ScanPushBack(Id);             // Push back function call for Eval().
         Eval(True);                   // Evaluate an LVAL expression.
         break;

      case AUTO:
         Local();                      // Define local variable(s).
         return FALSE;
         break;

      case EXTERN:
         Extern();                     // Define external variable(s).
         return FALSE;
         break;
                                       // The following are not supported...
      case CHAR:
      case CONST:
      case DOUBLE:
      case ENUM:
      case FLOAT:
      case INT:
      case LONG:
      case REGISTER:
      case SHORT:
      case SIGNED:
      case SIZEOF:
      case STATIC:
      case STRUCT:
      case TYPEDEF:
      case VOID:
      case VOLATILE:
      case UNION:
      case UNSIGNED:
         SET_ERROR(RC_NOTIMP);
         break;

      default:
         SET_ERROR(RC_SYNTAX);
         break;
   }

   if ( !ScanExpect(SEMIC))         // Statements are terminated with semi.
      SET_ERROR(RC_EXPECTSEMI);     // Didn't expect anything else.

   return (False);          /* return to caller, not at EOM.          */
}



//-----------------------------------------------------------------------------
// Local() -- define local variables...
//-----------------------------------------------------------------------------

static void  Local( void )
{
   ID      Id;

   do {
      Scan( &Id);

      if (Id.ID != SYMBOL) {
         SET_ERROR(RC_EXPECTSYMBOL);  // Anything else not allowed.
         break;
      }
      SymRefer(Id.Symbol, REFER_VARIABLE);
      GenPCode(PLOCAL, Id.Symbol->PoolOff, 0);

      Scan(&Id);                 // Get next identifier.
   } while (Id.ID == COMMA);

   if ( Id.ID != SEMIC)           // Statements are terminated w/ semi.
      SET_ERROR(RC_EXPECTSEMI);   // Didn't expect anything else.
}



//-----------------------------------------------------------------------------
// Extern() -- define external variables...
//-----------------------------------------------------------------------------

static void  Extern( void )
{
   ID      Id;

   do {
      Scan(&Id);                       // Get next identifier.
      if (Id.ID != SYMBOL) {
         SET_ERROR(RC_EXPECTSYMBOL);   // Anything else not allowed.
         break;
      }
      SymRefer(Id.Symbol, REFER_VARIABLE);
      if (ExtVarCount >= MAX_EXT_VARS) {
         SET_ERROR(RC_TOOMANYEXTVARS);
         break;
      }
                                       // Add to external table...
      ExtVars[ExtVarCount++] = Id.Symbol->PoolOff;
      Scan(&Id);                       // Get next identifier.
   } while (Id.ID == COMMA);

   if ( Id.ID != SEMIC)                // Statements are terminated w/ semi.
      SET_ERROR(RC_EXPECTSEMI);        // Didn't expect anything else.
}



//-----------------------------------------------------------------------------
// Function() -- Process function declaration. Returns True if function is
//               not a prototype...
//-----------------------------------------------------------------------------

static Bool  Function( ID  *Id)
{
   ID      Id2;                        // Symbol type structure;
   int     nargs;
   USHORT  ArgArray[MAX_ARGS];


   GenPCode(PLINE, LINE(), COLUMN());  // Generate LINE pCode.

   Label(Id);                          // Process function label.

   if(CHECK_ERROR())                   // If error, just return.
      return  False;                   // Error will sift up.

   nargs = 0;

   Scan( &Id2 );                       // Get next token after LPAR.
   if(CHECK_ERROR())                   // If error, just return.
      return  False;                   // Error will sift up.

   while (Id2.ID != RPAR) {
      if (Id2.ID != SYMBOL) {
         SET_ERROR(RC_FUNCPARM);
         break;
      }
      SymRefer(Id2.Symbol, REFER_VARIABLE);
      ArgArray[nargs++] = Id2.Symbol->PoolOff;
      if(CHECK_ERROR())                // If error, just return.
         return  False;                // Error will sift up.
      Scan(&Id2);
      if (Id2.ID == RPAR)
         break;
      if (Id2.ID != COMMA)
         SET_ERROR(RC_FUNCPARM);
      Scan(&Id2);
   }

   if(CHECK_ERROR())                   // If error, just return.
      return  False;                   // Error will sift up.

   Scan( &Id2 );                       // Scan for start of function body.

   if ( Id2.ID != LBRACE) {            // Should start function body.

   }
   GenPCode(PENTER, nargs, 0);
   while (nargs--) {
      SymRefer(Id->Symbol, REFER_VARIABLE);
      GenPCode(PLOCAL,  ArgArray[nargs], 0);
      GenPCode(PASSIGN, ArgArray[nargs], 0);
   }
   return True;
}



//-----------------------------------------------------------------------------
// Switch() -- process switch...
//-----------------------------------------------------------------------------

static void Switch( void )
{
   ID          Id;
   POOLITEM   *SwitchPI;               // Branch to PSWITCH.
   USHORT      Exit;                   // Offset to exit branch.
   POOLITEM   *ExitPI;                 // Exit branch to back-patch.
   POOLITEM   *EndPI;                  // Exit branch to back-patch.
   struct SwitchTableEntry SwitchTable[MAX_CASES];
   int         SwitchIndex = 0;
   USHORT      Default = 0xffff;       // Offset to start of default code.



   //--------------------------------------------------------------------------
   // First, process switch expression...
   //--------------------------------------------------------------------------

   if ( !ScanExpect(LPAR) ) {          // Left paran next?
      SET_ERROR(RC_EXPECTLPAR);        // Didn't expect anything else.
      return;
   }

   Eval(False);                        // Evaluate the conditional expression.

   if(CHECK_ERROR())                   // If error, just return.
      return;                          // Error will sift up.

   if ( !ScanExpect(RPAR) ) {          // Right paran next?
      SET_ERROR(RC_EXPECTRPAR);        // Didn't expect anything else.
      return;
   }

   SwitchPI = GenPCode(PBRANCH, 0, 0); // Generate branch to PSWITCH.

   //--------------------------------------------------------------------------
   // Generate PBRANCH around all case statements...
   //--------------------------------------------------------------------------

   Exit   = PCODE_OFFSET();            // Save exit branch offset.
   ExitPI = GenPCode(PBRANCH, 0, 0);   // Generate exit branch.

   if ( !ScanExpect(LBRACE) ) {        // Left brace next?
      SET_ERROR(RC_EXPECTRPAR);        // Didn't expect anything else.
      return;
   }

   //--------------------------------------------------------------------------
   // Prepare to process case statements...
   //--------------------------------------------------------------------------

   Breaks[CurrentLoop]    = Exit;      // Save break pcode offset.
   Continues[CurrentLoop] = 0xffff;    // Continues are not allowed.
   CurrentLoop++;                      // Increment loop level.

   Scan(&Id);
   while (Id.ID != RBRACE) {

      if (Id.ID == CASE) {
         Scan(&Id);                    // Get value;
         if (Id.ID != NUMBER ||
             !(Id.Symbol->Literal->Flag1 & DATA_INT)) {
            SET_ERROR(RC_BADCASE);     // Not valid.
            return;
         }
         SwitchTable[SwitchIndex].Value       = Id.Symbol->Literal->Integer;
         SwitchTable[SwitchIndex].PCodeOffset = PCODE_OFFSET();
         SwitchIndex++;                // To next entry in table.
         if ( !ScanExpect(COLON))      // Case is terminated with colon.
            SET_ERROR(RC_EXPECTCOLON); // Didn't expect anything else.

      } else if (Id.ID == DEFAULT) {
         Default = PCODE_OFFSET();     // Save offset to default code.

         if ( !ScanExpect(COLON))      // default is terminated with colon.
            SET_ERROR(RC_EXPECTCOLON); // Didn't expect anything else.

      } else {
         Statement(&Id);               // Process a statement.
         if(CHECK_ERROR())             // If error, just return.
            return;                    // Error will sift up.
      }
      Scan(&Id);                       // Get next identifier from next line.
   }

   //--------------------------------------------------------------------------
   // Now, set up switch table and PSWITCH instruction...
   //--------------------------------------------------------------------------

   CurrentLoop--;                      // Decrement loop level.

   EndPI = GenPCode(PBRANCH, 0, 0);    // Extra exit branch just in case.

   GenPatch(SwitchPI, PCODE_OFFSET()); // Patch previous branch to here.

   GenPCode(PSWITCH, SwitchIndex, CurLitPool);  // Generate switch pcode.
                                       // Add switch table to literal pool.
   SymTable((BYTE *) &SwitchTable, SwitchIndex * sizeof(SwitchTable[0]));

   if (Default != 0xffff)              // If there is default code...
      GenPCode(PBRANCH, Default, 0);   // Generate branch to it.

   GenPatch( ExitPI, PCODE_OFFSET() ); // Patch previous exit branch to here.
   GenPatch(  EndPI, PCODE_OFFSET() ); // Patch previous end  branch to here.

}



//-----------------------------------------------------------------------------
// For() -- process for...
//-----------------------------------------------------------------------------

static void For( void )
{
   ID          Id;                     // Id for body of while.
   int         Top;                    // PCode offset to top of loop.
   int         Last;                   // Last part.
   POOLITEM   *IfPI;                   // Pointer to PIF pcode instruction.
   POOLITEM   *Br1PI;                  // Pointer to PIF pcode instruction.
   USHORT      Exit;
   POOLITEM   *ExitPI;


   if ( !ScanExpect(LPAR) ) {          // Left paran next?
      SET_ERROR(RC_EXPECTLPAR);        // Didn't expect anything else.
      return;
   }

   //--------------------------------------------------------------------------
   // Initialization part...
   //--------------------------------------------------------------------------

   do {

      Eval(True);                      // Evaluate the assignment expression.

      if(CHECK_ERROR())                // If error, just return.
         return;                       // Error will sift up.

   } while ( ScanPeek() == COMMA );    // If comma, then more assignments.

   if ( !ScanExpect(SEMIC) ) {         // semicolon next?
      SET_ERROR(RC_EXPECTSEMI);        // Didn't expect anything else.
      return;
   }

   //--------------------------------------------------------------------------
   // Conditional part. Not an LVAL expression, so uses Eval() instead of
   // Expression()...
   //--------------------------------------------------------------------------

   Top  = PCODE_OFFSET();              // Get current PCode offset.

   Eval(False);                        // Evaluate the conditional expression.

   IfPI = GenPCode( PIF, 0, 0);        // Jump out of loop if FALSE;

   if ( !ScanExpect(SEMIC) ) {         // semicolon next?
      SET_ERROR(RC_EXPECTSEMI);        // Didn't expect anything else.
      return;
   }

   Br1PI = GenPCode( PBRANCH, 0, 0);   // Jump to body part.

   //--------------------------------------------------------------------------
   // Generate exit branch for break...
   //--------------------------------------------------------------------------

   Exit   = PCODE_OFFSET();            // Save exit branch offset.
   ExitPI = GenPCode(PBRANCH, 0, 0);   // Generate exit branch.

   //--------------------------------------------------------------------------
   // End of loop expression part...
   //--------------------------------------------------------------------------

   Last  = PCODE_OFFSET();             // Get current PCode offset.

   do {

      Eval(True);                      // Evaluate the assignment expression.

      if(CHECK_ERROR())                // If error, just return.
         return;                       // Error will sift up.

   } while ( ScanPeek() == COMMA );    // If comma, then more assignments.

   if ( !ScanExpect(RPAR) ) {          // Right parenthesis next?
      SET_ERROR(RC_EXPECTRPAR);        // Didn't expect anything else.
      return;
   }

   GenPCode( PBRANCH, Top, 0);         // Jump to top of loop.

   //--------------------------------------------------------------------------
   // Process body...
   //--------------------------------------------------------------------------

   GenPatch( Br1PI, PCODE_OFFSET() );  // Patch previous PBRANCH to here.

   Breaks[CurrentLoop]    = Exit;      // Save break pcode offset.
   Continues[CurrentLoop] = Top;       // Save continue pcode offset.
   CurrentLoop++;                      // Increment loop level.

   Scan(&Id);
   Statement(&Id);                     // Process a statement.

   if(CHECK_ERROR())                   // If error, just return.
      return;                          // Error will sift up.

   CurrentLoop--;                      // Decrement loop level.

   GenPCode( PBRANCH, Last, 0);        // Branch to last part of loop.

   GenPatch( IfPI,   PCODE_OFFSET() ); // Patch previous PIF to here.
   GenPatch( ExitPI, PCODE_OFFSET() ); // Patch previous exit branch to here.

}



//-----------------------------------------------------------------------------
// While() -- process while...
//-----------------------------------------------------------------------------

static void While( void )
{
   ID          Id;                     // Id for body of while.
   POOLITEM   *IfPI;                   // Pointer to PIF pcode instruction.
   USHORT      Top;                    // PCode offset to top of loop.
   POOLITEM   *TopPI;
   USHORT      Exit;
   POOLITEM   *ExitPI;


   TopPI  = GenPCode(PBRANCH, 0, 0);   // Gen branch around exit branch.
   Exit   = PCODE_OFFSET();            // Save exit branch offset.
   ExitPI = GenPCode(PBRANCH, 0, 0);   // Generate exit branch.
   Top    = PCODE_OFFSET();            // Get current PCode offset.
   GenPatch( TopPI, Top );             // Patch First branch to here.
   GenPCode(PLINE, LINE(), COLUMN());  // Generate LINE pCode.

   if ( !ScanExpect(LPAR) ) {          // Left paran next?
      SET_ERROR(RC_EXPECTLPAR);        // Didn't expect anything else.
      return;
   }

   Eval(False);                        // Evaluate the conditional expression.

   if(CHECK_ERROR())                   // If error, just return.
      return;                          // Error will sift up.

   if ( !ScanExpect(RPAR) ) {          // Right paran next?
      SET_ERROR(RC_EXPECTRPAR);        // Didn't expect anything else.
      return;
   }

   IfPI = GenPCode( PIF, 0, 0);

   Breaks[CurrentLoop]    = Exit;      // Save break pcode offset.
   Continues[CurrentLoop] = Top;       // Save continue pcode offset.
   CurrentLoop++;                      // Increment loop level.

   Scan(&Id);
   Statement(&Id);                     // Process a statement.

   CurrentLoop--;                      // Decrement loop level.

   if(CHECK_ERROR())                   // If error, just return.
      return;                          // Error will sift up.

   GenPCode( PBRANCH, Top, 0);         // Branch to top of loop.

   GenPatch( IfPI,   PCODE_OFFSET() ); // Patch previous PIF to here.
   GenPatch( ExitPI, PCODE_OFFSET() ); // Patch previous exit branch to here.
}



//-----------------------------------------------------------------------------
// Do() -- process do-while...
//-----------------------------------------------------------------------------

static void Do( void )
{
   ID          Id;                     // Id for body of while.
   int         Top;                    // PCode offset to top of loop.
   POOLITEM   *IfPI;                   // Pointer to PIF pcode instruction.
   POOLITEM   *TopPI;                  // Pointer to PBRANCH pcode instruction.
   USHORT      Exit;                   // Offset to exit PBRANCH.
   POOLITEM   *ExitPI;                 // Pointer to exit PBRANCH pcode instr.


   TopPI  = GenPCode( PBRANCH, 0, 0);  // Branch to test part of loop.
   Exit   = PCODE_OFFSET();            // Save exit branch offset.
   ExitPI = GenPCode(PBRANCH, 0, 0);   // Generate exit branch.
   Top    = PCODE_OFFSET();            // Get current PCode offset.
   GenPatch( TopPI, Top );             // Patch previous PBRANCH to here.

   Breaks[CurrentLoop]    = Exit;      // Save break pcode offset.
   Continues[CurrentLoop] = Top;       // Save continue pcode offset.
   CurrentLoop++;                      // Increment loop level.

   Scan(&Id);
   Statement(&Id);                     // Process a statement.

   CurrentLoop--;                      // Decrement loop level.

   if(CHECK_ERROR())                   // If error, just return.
      return;                          // Error will sift up.

   if ( !ScanExpect(WHILE) ) {         // While next?
      SET_ERROR(RC_EXPECTWHILE);       // Didn't expect anything else.
      return;
   }

   if ( !ScanExpect(LPAR) ) {          // Left paran next?
      SET_ERROR(RC_EXPECTLPAR);        // Didn't expect anything else.
      return;
   }

   Eval(False);                        // Evaluate the conditional expression.

   if(CHECK_ERROR())                   // If error, just return.
      return;                          // Error will sift up.

   if ( !ScanExpect(RPAR) ) {          // Right paran next?
      SET_ERROR(RC_EXPECTRPAR);        // Didn't expect anything else.
      return;
   }

   IfPI = GenPCode( PIF, 0, 0);        // If not true then branch out of loop.

   GenPCode( PBRANCH, Top, 0);         // Branch to top of loop.

   GenPatch( IfPI,   PCODE_OFFSET() ); // Patch previous PIF to here.
   GenPatch( ExitPI, PCODE_OFFSET() ); // Patch previous exit branch to here.

}



//-----------------------------------------------------------------------------
// break() -- Process break...
//-----------------------------------------------------------------------------

static void Break( void )
{
   if (CurrentLoop < 0) {              // Not in a loop, then error.
      SET_ERROR(RC_BADBREAK);
      return;
   }

   GenPCode(PBRANCH, Breaks[CurrentLoop-1], 0);
}



//-----------------------------------------------------------------------------
// continue() -- Process continue...
//-----------------------------------------------------------------------------

static void Continue( void )
{
   int  i = CurrentLoop - 1;

   if (i < 0) {                        // Not in a loop, then error.
      SET_ERROR(RC_BADCONTINUE);
      return;
   }

   while (Continues[i] == 0xffff) {    // If currently in switch, then get
      i--;                             // previous do/while/for loop.
      if (i < 0) {                     // No previous do/while/for loop.
         SET_ERROR(RC_BADCONTINUE);
         return;
      }
   }

   GenPCode(PBRANCH, Continues[i], 0);
}



//-----------------------------------------------------------------------------
// if() -- Process if...
//-----------------------------------------------------------------------------

static void If( void )
{
   ID          Id;
   POOLITEM   *IfPI;
   POOLITEM   *ElsePI;


   if ( !ScanExpect(LPAR) ) {          // Left paran next?
      SET_ERROR(RC_EXPECTLPAR);        // Didn't expect anything else.
      return;
   }

   Eval(False);                        // Evaluate the conditional expression.

   if(CHECK_ERROR())                   // If error, just return.
      return;                          // Error will sift up.

   if ( !ScanExpect(RPAR) ) {          // Right paran next?
      SET_ERROR(RC_EXPECTRPAR);        // Didn't expect anything else.
      return;
   }

   IfPI = GenPCode( PIF, 0, 0);

   Scan(&Id);
   Statement(&Id);                     // Process a statement.

   if(CHECK_ERROR())                   // If error, just return.
      return;                          // Error will sift up.

   if ( ScanPeek()  != ELSE) {          // Followed by an ELSE?

      GenPatch( IfPI, PCODE_OFFSET() ); // No, just patch if to here.

   } else {

      Scan(&Id);                       // Eat the ELSE.
      ElsePI = GenPCode(PBRANCH, 0, 0);    // Generate branch around else code.
      GenPatch( IfPI, PCODE_OFFSET() );    // Patch previous PIF to here.
      Scan(&Id);                           // Get start of statement.
      Statement(&Id);                      // Go parser the statement.
      if(CHECK_ERROR())                // If error, just return.
         return;                       // Error will sift up.
      GenPatch(ElsePI, PCODE_OFFSET() );   // Patch that PBRANCH to here.
   }


}



//-----------------------------------------------------------------------------
// Label() -- Process a label...
//-----------------------------------------------------------------------------

static void Label( ID   *Id )
{
   int          Offset;
   SYMREF      *SymRef;
   PATCH       *BP;


   Offset  = PCODE_OFFSET();           // Get current PCode offset.
   Offset -= PLINE_LEN;                // Backup to PLINE instruction.

   SymRef = SymRefer(Id->Symbol, REFER_LABEL);   // Make reference.
   SymRef->PCodeOff = Offset;          // Save pCode offset of label.

   //--------------------------------------------------------------------------
   // Check if already defined. If not, make it thee definition...
   //--------------------------------------------------------------------------
   if (Id->Symbol->LabDefRef != NULL) {          // Already defined?
      SET_ERROR(RC_DUP_LABEL);
      return;
   }

   Id->Symbol->LabDefRef = SymRef;                // Save definition.
   Id->Symbol->Flags |= SYMBOL_RESOLVED;

   //--------------------------------------------------------------------------
   // If a function, then mark as such and add to publics table...
   //--------------------------------------------------------------------------

   if (Id->ID == FUNCT) {
      Id->Symbol->Flags |= SYMBOL_FUNCTION;       // Remember it's a function.
      PubFuns[PubFunsCount].Name        = Id->Symbol;
      PubFuns[PubFunsCount].PCodeOffset = Offset;
      PubFunsCount++;
      PubFunsLength += sizeof(PubFuns[0].PCodeOffset) +
                       Id->Symbol->Literal->StringLength + 1;
   }

   //--------------------------------------------------------------------------
   // Now, check to see if we need to backpatch any references...
   //--------------------------------------------------------------------------

   BP = Id->Symbol->BackPatch;         // References to backpatch.

   while (BP != NULL) {                // For each reference...
      if ( !(BP->SymRef->Flags & (REFER_RESOLVED | REFER_LIT_FUNC))) {
         GenPatch(BP->PoolItem, Offset);   // Back patch this pCode.
      }
      BP->SymRef->Flags |= REFER_RESOLVED; // Mark ref resolved.

      BP = BP->Next;                   // Next backpatch reference.
   }

   return;
}



//-----------------------------------------------------------------------------
// Return() -- Process RETURN statement...
//-----------------------------------------------------------------------------

static void  Return( void )
{

   if ( ScanPeek() == SEMIC ) {        // If there is'nt anything to return.
      PushZero();
      GenPCode( PRETURN, 0, 0);              // And, generate return.
      return;
   }

   Eval(False);                        // Evaluate expression.

   if (CHECK_ERROR())
      return;

   GenPCode( PRETURN, 0, 0);

   return;

}



//-----------------------------------------------------------------------------
// Goto() -- Process goto statement...
//-----------------------------------------------------------------------------

static void  Goto( void )
{
   ID      Id;
   SYMREF *SymRef;
   POOLITEM  *PI;

   Scan(&Id);
   if (Id.ID != SYMBOL) {
      SET_ERROR(RC_GOTO);              // Then error.
      return;
   }
   SymRef = SymRefer(Id.Symbol, REFER_LABEL);
   if (Id.Symbol->Flags & SYMBOL_RESOLVED) {
      GenPCode(PBRANCH, Id.Symbol->LabDefRef->PCodeOff, 0);
   } else {
      PI = GenPCode(PBRANCH, 0, 0);
      SymSetBP(Id.Symbol, SymRef, PI);
   }

   return;
}



//-----------------------------------------------------------------------------
// PushZero() -- Push a constant integer "0" onto stack...
//-----------------------------------------------------------------------------

static void  PushZero( void )
{
   SYMTAB*  Symbol;

   Symbol = SymAdd("0", 1);            // Generate a constant one to push
   Symbol->Literal->Integer = 0;
   Symbol->Literal->Flag1   |= DATA_INT;
   GenPCode( PCONST, Symbol->PoolOff, 0); // Push constant one onto stack.

}



//-----------------------------------------------------------------------------
// AutoFunCall() -- Check and process an auto function call...
//
// This is a PassScript extension to allow a different type of function call.
// If a symbol is followed by something other than an assignment operator, then
// asume that we have the name of a function to call followed by one or more
// parameters separated by a comma...
//-----------------------------------------------------------------------------
static Bool  AutoFunCall( ID *Id)
{
    ID        Id2;                      // Symbol type struct for looking ahead.
   int       parms = 0;                // Count of passed parameters.
   SYMREF*   SymRef;                   // Ptr to symbol reference entry.
   POOLITEM* PI;                       // Pcode Poolitem ptr for back patching.


   //--------------------------------------------------------------------------
   // Now, if the next thing is an assignment, then we shouldn't be here...
   //--------------------------------------------------------------------------
   Scan( &Id2 );

   if ( Id2.ID == PLUSEQ  ||
        Id2.ID == MINUSEQ ||
        Id2.ID == MULTEQ  ||
        Id2.ID == MODEQ   ||
        Id2.ID == DIVEQ   ||
        Id2.ID == SHREQ   ||
        Id2.ID == SHLEQ   ||
        Id2.ID == NEGEQ   ||
        Id2.ID == OREQ    ||
        Id2.ID == ANDEQ   ||
        Id2.ID == XOREQ   ||
        Id2.ID == EQ     )  {
      ScanPushBack( &Id2 );
      return False;
   }

   //--------------------------------------------------------------------------
   // Check to see if there is at least one parm. Check for something other
   // than semicolin and if so, then start looking for parms by repeatedly
   // calling Eval() to evaluate parameter expressions...
   //--------------------------------------------------------------------------

   if (Id2.ID != SEMIC) {              // We got something other than a semicol.

      ScanPushBack( &Id2);             // Push back whatever it was that we got,
                                       // So that it can be evaluated by Eval().
      while(1) {                       // Now, start evaluating parameter exprs.

         //--------------------------------------------------------------------
         // Call Eval() to evaluate an expression parameter.  I don't think
         // that Eval() will return False anymore, which means that there
         // wasn't an expression to parse, but, just in case...
         //--------------------------------------------------------------------

         if (Eval(False) == False)     // Evaluate a parameter expression.
            break;                     // No parameter expression to evaluate.

         if (CHECK_ERROR())            // Was there an error during evaluation?
            return True;               // Yes, then just return.
        
           parms++;                      // Keep count of parameters.

         Scan( &Id2 );                 // Next thing should be a comma or semic.

         if (Id2.ID != COMMA) {        // However, if not comma...
            ScanPushBack( &Id2 );      // Then no more parms to eval.
            if (Id2.ID != SEMIC) {
               Id2.ID = SEMIC;         // Fake a semicolin.
               ScanPushBack( &Id2 );   // And push it on stack.
            }   
            break;
         }
      }
   }

   //-----------------------------------------------------------------------
   // Now, add reference to function to call...
   //-----------------------------------------------------------------------

   SymRef = SymRefer(Id->Symbol,    // Add reference to function.
                     REFER_LABEL);

   if (CHECK_ERROR())               // If error, just return.
      return True;

   //-----------------------------------------------------------------------
   // Has function name been resolved? (Internal function).  If so, then
   // generate PCALL instruction...
   //-----------------------------------------------------------------------
   if (Id->Symbol->Flags & SYMBOL_RESOLVED) {

      SymRef->Flags |= REFER_RESOLVED;    // Indicate reference is resolved.

      GenPCode( PCALL,
                parms,
                Id->Symbol->LabDefRef->PCodeOff);

      return True;
   }

   //-----------------------------------------------------------------------
   // Otherwise, assume that function is an external function...
   //-----------------------------------------------------------------------
   PI = GenPCode( PCALLEX,               // Generate external call.
                  parms,                 // Number of parms passed on stack.
                  Id->Symbol->PoolOff);  // Offset to function name.

   SymSetBP( Id->Symbol, SymRef, PI);    // Set to back patch, if necessary.

   return True;
}
