//-----------------------------------------------------------------------------
//
//
//                       GENERALIZED LEXICAL ANALYZER
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
//            Module:  scanner.c
//
//             Title:  Generalized lexical analyzer
//
//       Description:  This file contains routines that make up the generalized
//                     lexical analyzer. The routines parse ascii strings
//                     loosely based on the C language and based on a keyword
//                     table.
//
//            Author:  J.C. Overton
//
//              Date:  06/20/94
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

#include "keywords.c"                  // Include keyword table.


//-----------------------------------------------------------------------------
// Definitions used in this module...
//-----------------------------------------------------------------------------

#define LEX_EOF         -1             // End of source file.
#define LEX_EOL         -2             // End of source line.



//-----------------------------------------------------------------------------
// Type table...
//-----------------------------------------------------------------------------

static unsigned char  IdentType[256] = {

   /*0x00*/ 0,       /*0x01*/ 0,      /*0x02*/ 0,      /*0x03*/ 0,
   /*0x04*/ 0,       /*0x05*/ 0,      /*0x06*/ 0,      /*0x07*/ 0,
   /*0x08*/ 0,       /*0x09*/ 0,      /*0x0a*/ 0,      /*0x0b*/ 0,
   /*0x0c*/ 0,       /*0x0d*/ 0,      /*0x0e*/ 0,      /*0x0f*/ 0,

   /*0x10*/ 0,       /*0x11*/ 0,      /*0x12*/ 0,      /*0x13*/ 0,
   /*0x14*/ 0,       /*0x15*/ 0,      /*0x16*/ 0,      /*0x17*/ 0,
   /*0x18*/ 0,       /*0x19*/ 0,      /*0x1a*/ 0,      /*0x1b*/ 0,
   /*0x1c*/ 0,       /*0x1d*/ 0,      /*0x1e*/ 0,      /*0x1f*/ 0,

   /*0x20*/ SPACE,   /*  ! */ NOT,    /*  " */ DQUOTE, /*  # */ POUND,
   /*  $ */ DOLLAR,  /*  % */ MOD,    /*  & */ AND,    /*  ' */ QUOTE,
   /*  ( */ LPAR,    /*  ) */ RPAR,   /*  * */ MULT,   /*  + */ PLUS,
   /*  , */ COMMA,   /*  - */ MINUS,  /*  . */ DOT,    /*  / */ DIV,

   /*  0 */ NUMBER,  /*  1 */ NUMBER, /*  2 */ NUMBER, /*  3 */ NUMBER,
   /*  4 */ NUMBER,  /*  5 */ NUMBER, /*  6 */ NUMBER, /*  7 */ NUMBER,
   /*  8 */ NUMBER,  /*  9 */ NUMBER, /*  : */ COLON,  /*  ; */ SEMIC,
   /*  < */ LT,      /*  = */ EQ,     /*  > */ GT,     /*  ? */ QM,

   /*0x40*/ SPACE,   /*  A */ SYMBOL, /*  B */ SYMBOL, /*  C */ SYMBOL,
   /*  D */ SYMBOL,  /*  E */ SYMBOL, /*  F */ SYMBOL, /*  G */ SYMBOL,
   /*  H */ SYMBOL,  /*  I */ SYMBOL, /*  J */ SYMBOL, /*  K */ SYMBOL,
   /*  L */ SYMBOL,  /*  M */ SYMBOL, /*  N */ SYMBOL, /*  O */ SYMBOL,

   /*  P */ SYMBOL,  /*  Q */ SYMBOL, /*  R */ SYMBOL, /*  S */ SYMBOL,
   /*  T */ SYMBOL,  /*  U */ SYMBOL, /*  V */ SYMBOL, /*  V */ SYMBOL,
   /*  X */ SYMBOL,  /*  Y */ SYMBOL, /*  Z */ SYMBOL, /*  [ */ LBRACK,
   /*  \ */ BSLASH,  /*  ] */ RBRACK, /*  ^ */ XOR,    /*  _ */ SYMBOL,

   /*  ` */ 0,       /*  a */ SYMBOL, /*  b */ SYMBOL, /*  c */ SYMBOL,
   /*  d */ SYMBOL,  /*  e */ SYMBOL, /*  f */ SYMBOL, /*  g */ SYMBOL,
   /*  h */ SYMBOL,  /*  i */ SYMBOL, /*  j */ SYMBOL, /*  k */ SYMBOL,
   /*  l */ SYMBOL,  /*  m */ SYMBOL, /*  n */ SYMBOL, /*  o */ SYMBOL,

   /*  p */ SYMBOL,  /*  q */ SYMBOL, /*  r */ SYMBOL, /*  s */ SYMBOL,
   /*  t */ SYMBOL,  /*  u */ SYMBOL, /*  v */ SYMBOL, /*  v */ SYMBOL,
   /*  x */ SYMBOL,  /*  y */ SYMBOL, /*  z */ SYMBOL, /*  { */ LBRACE,
   /*  | */ OR,      /*  } */ RBRACE, /*  ~ */ NEG,    /*0x7f*/ 0,

   /*0x80*/ 0,       /*0x81*/ 0,      /*0x82*/ 0,      /*0x83*/ 0,
   /*0x84*/ 0,       /*0x85*/ 0,      /*0x86*/ 0,      /*0x87*/ 0,
   /*0x88*/ 0,       /*0x89*/ 0,      /*0x8a*/ 0,      /*0x8b*/ 0,
   /*0x8c*/ 0,       /*0x8d*/ 0,      /*0x8e*/ 0,      /*0x8f*/ 0,

   /*0x90*/ 0,       /*0x91*/ 0,      /*0x92*/ 0,      /*0x93*/ 0,
   /*0x94*/ 0,       /*0x95*/ 0,      /*0x96*/ 0,      /*0x97*/ 0,
   /*0x98*/ 0,       /*0x99*/ 0,      /*0x9a*/ 0,      /*0x9b*/ 0,
   /*0x9c*/ 0,       /*0x9d*/ 0,      /*0x9e*/ 0,      /*0x9f*/ 0,

   /*0xa0*/ 0,       /*0xa1*/ 0,      /*0xa2*/ 0,      /*0xa3*/ 0,
   /*0xa4*/ 0,       /*0xa5*/ 0,      /*0xa6*/ 0,      /*0xa7*/ 0,
   /*0xa8*/ 0,       /*0xa9*/ 0,      /*0xaa*/ 0,      /*0xab*/ 0,
   /*0xac*/ 0,       /*0xad*/ 0,      /*0xae*/ 0,      /*0xaf*/ 0,

   /*0xb0*/ 0,       /*0xb1*/ 0,      /*0xb2*/ 0,      /*0xb3*/ 0,
   /*0xb4*/ 0,       /*0xb5*/ 0,      /*0xb6*/ 0,      /*0xb7*/ 0,
   /*0xb8*/ 0,       /*0xb9*/ 0,      /*0xba*/ 0,      /*0xbb*/ 0,
   /*0xbc*/ 0,       /*0xbd*/ 0,      /*0xbe*/ 0,      /*0xbf*/ 0,

   /*0xc0*/ 0,       /*0xc1*/ 0,      /*0xc2*/ 0,      /*0xc3*/ 0,
   /*0xc4*/ 0,       /*0xc5*/ 0,      /*0xc6*/ 0,      /*0xc7*/ 0,
   /*0xc8*/ 0,       /*0xc9*/ 0,      /*0xca*/ 0,      /*0xcb*/ 0,
   /*0xcc*/ 0,       /*0xcd*/ 0,      /*0xce*/ 0,      /*0xcf*/ 0,

   /*0xd0*/ 0,       /*0xd1*/ 0,      /*0xd2*/ 0,      /*0xd3*/ 0,
   /*0xd4*/ 0,       /*0xd5*/ 0,      /*0xd6*/ 0,      /*0xd7*/ 0,
   /*0xd8*/ 0,       /*0xd9*/ 0,      /*0xda*/ 0,      /*0xdb*/ 0,
   /*0xdc*/ 0,       /*0xdd*/ 0,      /*0xde*/ 0,      /*0xdf*/ 0,

   /*0xe0*/ 0,       /*0xe1*/ 0,      /*0xe2*/ 0,      /*0xe3*/ 0,
   /*0xe4*/ 0,       /*0xe5*/ 0,      /*0xe6*/ 0,      /*0xe7*/ 0,
   /*0xe8*/ 0,       /*0xe9*/ 0,      /*0xea*/ 0,      /*0xeb*/ 0,
   /*0xec*/ 0,       /*0xed*/ 0,      /*0xee*/ 0,      /*0xef*/ 0,

   /*0xf0*/ 0,       /*0xf1*/ 0,      /*0xf2*/ 0,      /*0xf3*/ 0,
   /*0xf4*/ 0,       /*0xf5*/ 0,      /*0xf6*/ 0,      /*0xf7*/ 0,
   /*0xf8*/ 0,       /*0xf9*/ 0,      /*0xfa*/ 0,      /*0xfb*/ 0,
   /*0xfc*/ 0,       /*0xfd*/ 0,      /*0xfe*/ 0,      /*0xff*/ 0,
};




//-----------------------------------------------------------------------------
// Static local variables...
//-----------------------------------------------------------------------------

static int   c      = ' ';             // Current character.
static int   Ident  = SEMIC;           // Current Identifier - End of Line.
static int   LastIdent;                // Last symbol returned by Lex.

static int   CurLine     = 0;          // Current character line number.
static int   IDLine      = 0;          // Current character line number.
static int   CurColumn   = 1;          // Current character column number.
static int   IDColumn    = 1;          // Current character column number.

static Bool  FirstTime   = True;       // Get first line from program.

static int   SaveChar;                 // Save current char for PushBack().

static char  CurSym[251];              // Current symbol name.
static int   CurSymLen;                // Length of current symbol name.
static long  CurInt;                   // Conversion to integer number.
static int   CurError = SCAN_OK;       // Save any error from scan.

                                       // Current Keyword table.
static KeyWordTab* CurKeyWordTab = KeyTab;
static KeyWordTab* TopKeyWordTab = KeyTab;

static ID    SaveIDs[3];               // Array of push backed IDs.
static int   SaveIDIndex = 0;          // Index into push backed ID array.

static SYMTAB* CurSymTab;              // Symbol table entry ptr.

static char  Record[512];              // A place to read in current line.
static int   RecLen = 0;               // Current line's length.
static char *CurLinePtr;               // Current pointer into Record.
static FILE *FileArray[MAX_INPUT_FILES];  // Input file handles.
static int   FileIndex = -1;           // No open files, yes.


//-----------------------------------------------------------------------------
// Local static routine definitions for routines in this module...
//-----------------------------------------------------------------------------

static int  NextID(      void );
static int  NextChar(    void );
static void PushBack(    void );
static void GetName(     void );
static Bool GetNumber(   void );
static void GetLiteral(  int  );
static int  FindKeyWord( void );
static Bool CnvNumber(   int  );



//-----------------------------------------------------------------------------
// Initialize Lexical analyzer. File must be opened already. pass file handle.
//-----------------------------------------------------------------------------

int  ScanInit( void )
{

   FileIndex = -1;                     // Reset input file handle index.

   CurKeyWordTab = KeyTab;             // Save pointer to keyword table.
   TopKeyWordTab = KeyTab;

   return SCAN_OK;
}


//-----------------------------------------------------------------------------
// Set current input file stream...
//-----------------------------------------------------------------------------

int  ScanFile( FILE* FIn)
{

   if (FileIndex + 1 > MAX_INPUT_FILES)
      return SCAN_TOOMANYFILES;

   FileIndex++;                        // Reset input file handle index.
   FileArray[FileIndex] = FIn;         // Save File Handle.

   return SCAN_OK;
}


//-----------------------------------------------------------------------------
// Set current scan line.  Just insert line as current line...
//-----------------------------------------------------------------------------

int  ScanLine(char* Line)
{
   int   Len;


   if ((Len = strlen(Line)) >= sizeof(Record))
      return SCAN_SRCLONG;

   memcpy(Record, Line, Len);          // Save passed line to be parsed.
   Record[Len] = '\0';                 // Null terminate line.
   RecLen = Len;                       // Save length.
   CurColumn = 1;                      // Start at first column.
   CurLinePtr = Record;                // Set current pointer.

   return SCAN_OK;
}


//-----------------------------------------------------------------------------
// Set keyword scan table...
//-----------------------------------------------------------------------------

KeyWordTab* ScanSetKeyTab( KeyWordTab* KeyTab )
{
   KeyWordTab* PrevKeyTab = TopKeyWordTab;

   CurKeyWordTab = KeyTab;             // Save pointer to keyword table.
   TopKeyWordTab = KeyTab;

   return PrevKeyTab;                  // Return Previous Keyword table.
}


//-----------------------------------------------------------------------------
// Terminate Lexical analyzer. Close source file...
//-----------------------------------------------------------------------------

int  ScanTerm( void )
{

   do {
      fclose( FileArray[FileIndex]);   // Close all open input files.
   } while(FileIndex-- > 0);

   return SCAN_OK;
}


//-----------------------------------------------------------------------------
// ScanGetSource() -- Return ptr and length of current line...
//-----------------------------------------------------------------------------

int  ScanGetSource( char **s, int  *Len)
{

   *s   = Record;
   *Len = RecLen;

   return SCAN_OK;
}



//-----------------------------------------------------------------------------
// ScanNextLine() -- Advance to next clause in source module...
//-----------------------------------------------------------------------------

int  ScanNextLine( void )
{
   int   NestComments = 0;


   CurError = SCAN_OK;                 // Reset current error.

   do {
      do {
         NextChar();
         if (c == '/') {               // Comments...
            NextChar();
            if (c == '*') {
               NestComments++;
               NextChar();
            }
         }
         if (NestComments) {
            if (c == '*') {
               NextChar();
               if (c == '/') {
                  NestComments--;
                  NextChar();
               }
            }
            if (c == LEX_EOF) {
               return SCAN_UNMATCHED;
            }
         }
      } while (NestComments);
   } while (c != LEX_EOL);

   CurKeyWordTab = TopKeyWordTab;      // Reset keyword table.
   Ident = SEMIC;                      // Last symbol was SEMIC.

   return CurError;                    // Return with any errors.
}



//-----------------------------------------------------------------------------
// Lexical analyzer routine...
//-----------------------------------------------------------------------------

int  Scan( ID  *Id)
{


   CurError = SCAN_OK;                 // Reset current error.

   if (SaveIDIndex) {                  // Is there a push backed ID?

      *Id = SaveIDs[ --SaveIDIndex ];  // Return push backed ID.
      IDLine    = Id->Line;            // Restore Line and Column.
      IDColumn  = Id->Column;
      Ident     = Id->ID;              // Restore last identifier #.
      return SCAN_OK;
   }

   Id->ID     = NextID();              // Get next identifier.
   Id->Line   = IDLine;
   Id->Column = IDColumn;
   Id->Symbol = CurSymTab;

   Line   = IDLine;
   Column = IDColumn;

   return CurError;                    // Return with any errors.
}



//-----------------------------------------------------------------------------
// ScanExpect() -- Scan next token and check it for a match...
//-----------------------------------------------------------------------------

Bool  ScanExpect( int Token)
{
   ID Id;

   if (Scan(&Id)) {                    // Scan next token.
      ScanPushBack(&Id);               // Error, push back error.
      return False;                    // Return. never expect errors!
   }

   if (Id.ID == Token)                 // Is this the token we were expecting?
      return True;                     // Yes, so eat it and return.

   ScanPushBack(&Id);                  // Pushback what we didn't expect.
   return False;                       // Tell caller.
}



//-----------------------------------------------------------------------------
// ScanPeek() -- Peek at next token...
//-----------------------------------------------------------------------------

int  ScanPeek( void )
{
   ID Id;
   int      Token;

   Scan(&Id);                          // Scan next token.

   Token = Id.ID;                      // Is this the token we were expecting?

   ScanPushBack(&Id);                  // Pushback what ever it was.

   return Token;                       // Return caller.
}



//-----------------------------------------------------------------------------
// ScanPushBack() -- Push back an identifier for next time...
//-----------------------------------------------------------------------------

int   ScanPushBack( ID  *Id)
{

   SaveIDs[ SaveIDIndex++ ] = *Id;     // Save ID in array.

   return SCAN_OK;
}



//-----------------------------------------------------------------------------
// ScanIsEOC() -- Check to see if we are at end of clause, that is,
//                the next identifier is a SEMIColon...
//-----------------------------------------------------------------------------

Bool  ScanIsEOC(void)
{
   ID   Id;

   Scan( &Id );                        // Get next identifier.

   if (Id.ID    == SEMIC &&            // Is this the semicolon?
       CurError == SCAN_OK) {          // And no errors?
      return True;                     // Yes.
   }

   ScanPushBack( &Id );                // Push back identifier.

   return False;                       // We're not at end of clause.
}


//-----------------------------------------------------------------------------
// NextID() -- Get Next identifier...
//-----------------------------------------------------------------------------

static int  NextID( void )
{
   int   TempIdent;
   int   NestComments = 0;
   Bool  CheckAbutts  = False;


   LastIdent = Ident;                  // Save last identifier.
   CurSymTab = NULL;                   // Clear symbol table entry ptr.


NextSymbol:                            // Come back here if after comment.

   //--------------------------------------------------------------------------
   // Now, get start of next token, and check for EOL or EOF...
   //--------------------------------------------------------------------------

   IDLine   = CurLine;                 // Save current line number.
   IDColumn = CurColumn;               // Save current column number.

   do {
      while (NextChar() == ' ');       // Gobble up spaces.

      if (c == LEX_EOF)                // If at end of source file...
         return (EOM);                 // then return END-OF-MODULE ID.

      if (c == LEX_EOL)                // End of line. Just go to next line.
         goto NextSymbol;

      //if ((c == LEX_EOL) && LastIdent != SEMIC) {    // End of line and we
      //                               // haven't sent semicolon yet...
      //   Ident = SEMIC;              // Send back end of line char.
      //   CurKeyWordTab = TopKeyWordTab; /* Reset Keyword table.
      //   return (SEMIC);             // Send semicolon.
      //}

   } while (c == LEX_EOL);             // If end of line, then bypass it.

   IDLine   = CurLine;                 // Save current line number.
   IDColumn = CurColumn;               // Save current column number.



   //--------------------------------------------------------------------------
   // Now, process start of token. Look at "IdentType" of first char
   // and process based at that...
   //--------------------------------------------------------------------------

   switch ( Ident = IdentType[c]) {

      case DQUOTE:                               /* Literal string.   */
      case QUOTE:                                /* Literal string.   */
         GetLiteral(Ident);
         Ident = STRING;                         /* Assume STRING.    */

         CurSymTab = SymAdd(CurSym, CurSymLen);  /* Add symbol.       */
         break;

      case COLON:
         break;

      case SEMIC:
         CurKeyWordTab = TopKeyWordTab;  /* Reset Keyword table.        */
         break;

      case COMMA:
         //while(NextChar() == ' ');     /* Gobble whitespace.          */
         //if (c == LEX_EOL)             /* Comma is continuation...    */
         //   goto NextSymbol;           /* Continue to next line.      */
         //PushBack();                   /* Otherwise, pushback char.   */
         break;                        /* And return with comma.      */

      case LPAR:
         break;
      case RPAR:
         break;
      case LBRACE:
         break;
      case RBRACE:
         break;
      case LBRACK:
         break;
      case RBRACK:
         break;
      case QM:
         break;
      case BSLASH:
         break;
      case DOLLAR:
         break;

      case OR:
         switch (NextChar()) {
            case '|':                  /*  ||  */
               Ident = LOGOR;
               break;
            case '=':                  /*  |=  */
               Ident = OREQ;
               break;
            default:
               PushBack();
               break;
         }
         break;

      case AND:
         switch (NextChar()) {
            case '&':                  /*  &&  */
               Ident = LOGAND;
               break;
            case '=':                  /*  &=  */
               Ident = ANDEQ;
               break;
            default:
               PushBack();
               break;
         }
         break;

      case XOR:
         switch (NextChar()) {
            case '=':                  /*  ^=  */
               Ident = XOREQ;
               break;
            default:
               PushBack();
               break;
         }
         break;

      case MINUS:
         switch (NextChar()) {
            case '-':                  /*  --  */
               Ident = DECR;
               break;
            case '=':                  /*  -=  */
               Ident = MINUSEQ;
               break;
            case '>':                  /*  ->  */
               Ident = PTR;
               break;
            default:
               PushBack();
               break;
         }
         break;

      case PLUS:
         switch (NextChar()) {
            case '+':                  /*  ++  */
               Ident = INCR;
               break;
            case '=':                  /*  +=  */
               Ident = PLUSEQ;
               break;
            default:
               PushBack();
               break;
         }
         break;

      case DIV:
         switch ( NextChar() ) {
            case '*':                   /* Comments...                */
               NestComments++;          /* Track nested comments.     */
               NextChar();              /* Next char after comm start */
               do {                     /* Now, look for...           */

                  switch (c) {
                     case '/':          /* ...start of embedded cmt...*/
                        NextChar();
                        if (c == '*') {
                           NestComments++;
                           NextChar();
                        }
                        break;

                     case '*':          /* ...end of comment...       */
                        NextChar();
                        if (c == '/') {
                           NestComments--;
                           NextChar();
                        }
                        break;

                     case LEX_EOF:      /* ...End of file...          */
                        CurError = SCAN_UNMATCHED;
                        return 0;

                     default:           /* Just something in comment. */
                        NextChar();

                  }
               } while (NestComments);
               PushBack();                        /* Push back last 1 */
               goto NextSymbol;                   /* Go get next sym. */
               break;

            case '/':                   /* C++ style comment */
               do {
                  NextChar();
                  if (c == LEX_EOF)
                     return EOM;
               } while (c != LEX_EOL);
               goto NextSymbol;
               break;

            case '=':                   /* /=  */
               Ident = DIVEQ;
               break;

            default:
               PushBack();              /* Just divide. Push back.    */
               break;
         }
         break;

      case MULT:                      /* *  */
         if ( NextChar() == '=')
            Ident = MULTEQ;           /* *= */
         else
            PushBack();
         break;

      case MOD:                       /* %  */
         if ( NextChar() == '=')
            Ident = MODEQ;            /* %= */
         else
            PushBack();
         break;
                                      /* =  */
      case EQ:
         if ( NextChar() == '=')
            Ident = EQEQ;             /* == */
         else
            PushBack();
         break;

      case GT:                        /* >  */
         switch ( NextChar() ) {
            case '>':                 /* >> or >>= */
               if ( NextChar() == '=')
                  Ident = SHREQ;      /* >>= */
               else {
                  PushBack();
                  Ident = SHR;        /* >> */
               }
               break;
            case '=':                 /* >= */
               Ident = GE;
               break;
            default:
               PushBack();
               break;
         }
         break;

      case LT:
         switch (NextChar()) {
            case '<':                 /* << or <<= */
               if (NextChar() == '=')
                  Ident = SHLEQ;      /* <<= */
               else {
                  PushBack();
                  Ident = SHL;        /* << */
               }
               break;
            case '=':                 /* <= */
               Ident = LE;
               break;
            default:
               PushBack();
               break;
         }
         break;

      case NOT:
         switch (NextChar()) {
            case '=':                 /* !=  */
               Ident = NOTEQ;
               break;
            default:
               PushBack();
               break;
         }
         break;

      case NEG:
         switch (NextChar()) {
            case '=':                 /* ~=  */
               Ident = NEGEQ;
               break;
            default:
               PushBack();
               break;
         }
         break;

      case DOT:
         if (NextChar() == '.')
            if (NextChar() == '.')
               Ident = DDD;
            else
               PushBack();             // Can't look ahead two chars.
         else
            PushBack();
         break;

      case SYMBOL:
         GetName();                    // Scan rest of symbol name.

         if ((TempIdent = FindKeyWord())) {    // Match keyword.
            Ident = TempIdent;                 // Yes, it is a keyword.
            break;

         } else {

            while (NextChar() == ' ');    // Now, what about character after?

            if (c == ':') {               // A Label?
               Ident = LABEL;

            } else if (c == '(') {        // A Function?
               Ident = FUNCT;

            } else {                      // Nothing important after symbol.
               PushBack();                // Push it back for next time.
            }

            //-----------------------------------------------------------------
            // Here, we have either a FUNCT, LABEL, or a SYMBOL. Add symbol...
            //-----------------------------------------------------------------

            CurSymTab = SymAdd(CurSym, CurSymLen);     // Add symbol.
         }

         break;

      case NUMBER:
         if ( GetNumber() ) {          // Try to convert to a number.

            CurSymTab = SymAdd(CurSym, CurSymLen);    // Add symbol.
            CurSymTab->Literal->Integer  = CurInt;
            CurSymTab->Literal->Flag1   |= DATA_INT;

         } else {                      // Else, could not convert number.

            CurSymTab = SymAdd(CurSym, CurSymLen);    // Add symbol.
            Ident = STRING;            // Then just a string literal.
         }
         break;

      case POUND:                      // Special scanner commands.
         while (NextChar() == ' ');    // Squeeze out whitespace.
         GetName();                    // Get into CurSym.

         if (strcmp(CurSym, "line") == 0) {    // #line command?
            while (NextChar() == ' '); // Get first part of number.
            GetNumber();
            CurLine = CurInt - 1;
         }

         ScanNextLine();               // Throw away rest of this line.
         goto NextSymbol;
         break;

      default:                         // Not a valid character.
         CurError = SCAN_BADCHAR;
         break;
   }

   return Ident;
}



//-----------------------------------------------------------------------------
// GetName() - Will finish scanning for a symbolic name.
//-----------------------------------------------------------------------------

static void GetName( void )
{

   CurSymLen = 0;                      // Clear length.

   while( 1 ) {

      CurSym[CurSymLen++] = c;         // Save first char.
      NextChar();                      // Get next char.

      if (IdentType[c] != SYMBOL && IdentType[c] != NUMBER )
         break;

      if (CurSymLen > sizeof(CurSym)-1) {
         CurError = SCAN_STRINGLEN;
         return;
      }
   }

   PushBack();                         // Push back non-symbol.

   CurSym[CurSymLen] = '\0';           // NULL terminate the name.

}



//-----------------------------------------------------------------------------
// GetNumber() - Will finish scanning for a number...
//-----------------------------------------------------------------------------

static Bool GetNumber( void )
{
   Bool    IsHex    = False;
   Bool    IsOctal  = False;
   char    c1;


   CurSymLen = 0;                      // Clear length.

   if (c == '0') {                     // Check for octal/hex.
      c1 = c;
      NextChar();
      if (toupper(c) == 'X') {
         IsHex = True;                 // Hex number.
         CurSym[CurSymLen++] = c1;            // Save first char.
         CurSym[CurSymLen++] = toupper(c);    // Save second char.
         NextChar();                          // Get first char of number.
      } else {
         IsOctal = True;               // Octal number.
         PushBack();
         c = c1;
      }
   }

   while (1) {

      CurSym[CurSymLen++] = toupper(c);    // Save first/next char.
      NextChar();                          // Get next character.

      if (IdentType[c] != NUMBER &&
           !(IsHex && toupper(c) >= 'A' && toupper(c) <= 'F') )  {

         //if (IdentType[c] != SYMBOL)
         //   break;
         //CurError = SCAN_BADNUMBER;
         //return False;

         break;
      }

      if (CurSymLen > sizeof(CurSym)-1) {
         CurError = SCAN_STRINGLEN;
         return False;
      }
   }

   PushBack();                         // Push back non-symbol.

   CurSym[CurSymLen] = '\0';           // NULL terminate the name.


   // Now, try to convert string to a number...

   if (IsHex) {
      if (!CnvNumber(16))
         return False;

   } else if (IsOctal) {
      if (!CnvNumber(8))
         return False;

   } else {
      if (!CnvNumber(10))
         return False;
   }

   return True;
}




//-----------------------------------------------------------------------------
// CnvNumber() - Convert symbol to a number...
//-----------------------------------------------------------------------------

static Bool CnvNumber (int radix)
{
   char c;
   char *p;

   CurInt = 0;                         // Clear conversion work area.
   p = CurSym;
   if (radix == 16)
      p += 2;                          // If hex, then bump past 0x.
   while ( *p ) {

      c = toupper(*p);

      if (CurInt >= 429496730l) {      // Will it fit?
         CurError = SCAN_NUMBERTOOBIG;
         return False;
      }

      CurInt *= radix;

      if (radix == 16 && c >= 'A' && c <= 'F' )
         CurInt += c - 'A' + 10;
      else
         CurInt += c - '0';

      p++;
   }
   return True;
}



//-----------------------------------------------------------------------------
// GetLiteral() - Will finish scanning for a literal...
//-----------------------------------------------------------------------------

static void GetLiteral( int MatchIdent )
{
   BYTE        b;

   CurSymLen = 0;                      // Clear length.

   while (1) {
      while (NextChar() == LEX_EOL);   // No end of lines, please.
      if (c == LEX_EOF) {
         CurError = SCAN_UNMATCHED;
         return;
      }

      //-----------------------------------------------------------------------
      // Check for escape sequences...
      //-----------------------------------------------------------------------
      if (c == '\\') {
         NextChar();
         if ( !isdigit(c)) {
            switch (c) {
               case 'a':  c = 0x07; break;
               case 'b':  c = 0x08; break;
               case 'f':  c = 0x0c; break;
               case 'n':  c = 0x0a; break;
               case 'r':  c = 0x0d; break;
               case 't':  c = 0x09; break;
               case 'v':  c = 0x0b; break;
               case '\'': c = '\''; break;
               case '?':  c = '?';  break;
               case '"':  c = '\"'; break;
               case '\\': c = '\\'; break;
               case 'x':
               case 'X':
                  //-----------------------------------------------------------
                  // Convert hex escape sequence...
                  //-----------------------------------------------------------
                  b = 0;
                  while ( NextChar()) {
                     b <<= 4;
                     if (c >= 'A' && c <= 'F')
                        b += c - 'A' + 10;
                     else if (c >= 'a' && c <= 'f')
                        b += c - 'a' + 10;
                     else if (c >= 0 && c <= 9)
                        b += c - '0';
                     else
                        break;
                  }
                  c = b;
                  PushBack();
            }

         //-------------------------------------------------------------------
         // Convert an Octal number...
         //-------------------------------------------------------------------
         } else {
            b = 0;
            while ( NextChar() ) {
               b *= 8;
               if (c >= '0' && c <= '7')
                  b += c - '0';
               else
                  break;
            }
            c = b;
            PushBack();
         }

      } else {

         if( IdentType[c] == MatchIdent) {
            break;                        // Break. End of literal.
         }
      }

      if (CurSymLen > sizeof(CurSym)-1) {
         CurError = SCAN_STRINGLEN;
         return;
      }

      CurSym[CurSymLen++] = c;
   }

   CurSym[CurSymLen] = '\0';           // NULL terminate the literal.

}



//-----------------------------------------------------------------------------
// FindKeyWord() - Will try to match name in keyword table...
//-----------------------------------------------------------------------------

static int  FindKeyWord( void )
{
   KeyWordTab  *KW;                    // A pointer into keyword table.

   if ( (KW = CurKeyWordTab) == NULL)
      return 0;

   while (KW->Keyword != NULL) {
      if (strcmp(KW->Keyword, CurSym) == 0) {     // Is this it?
         if (KW->Next != NULL)
            CurKeyWordTab = KW->Next;
         return (KW->KeyID);
      }
      KW++;                            // Try next entry.
   }

   return 0;
}


//-----------------------------------------------------------------------------
// PushBack() - Will save the current character for next time, and
//              restore the last line and column numbers.
//-----------------------------------------------------------------------------

static void PushBack( void )
{
   SaveChar   = c;                     // Save current char for next time.
}



//-----------------------------------------------------------------------------
// NextChar() - Get next character from source file. Return LEX_EOL
//              at end of line. Return LEX_EOF and close file when
//              end-of-file is reached...
//-----------------------------------------------------------------------------

static int  NextChar( void )
{
   static  char    *CurLinePtr;
   int              Ch;
   char             Msg[81];


   if (SaveChar) {                     // If a char was pushed back...
      Ch         = SaveChar;           // Restore previous character.
      SaveChar   = 0;                  // Clear for next call to NextChar()
      return ( ( c = Ch) );            // Return previously saved char.
   }

   //-------------------------------------------------------------------------
   // If just did last position in file, then return EOL indicator...
   //-------------------------------------------------------------------------
   if (CurColumn == RecLen && CurColumn != 0) {
      CurColumn++;
      return ( (c = LEX_EOL) );
   }

   //-------------------------------------------------------------------------
   // No more current files?
   //-------------------------------------------------------------------------
   if (FileIndex < 0)
      return ( (c = LEX_EOL) );

   //-------------------------------------------------------------------------
   // If beyond end of line (already returned EOL indicator) or this
   // is the first time, then get next record from source file...
   //-------------------------------------------------------------------------
   if ((CurColumn++ > RecLen) || FirstTime) {

GetNextLine:

      FirstTime = False;
      fgets(Record, sizeof(Record), FileArray[FileIndex]);
      RecLen = strlen(Record);
      if (RecLen > (sizeof(Record) - 2)) {
         CurError = SCAN_SRCLONG;
         return(LEX_EOF);              // Error return.
      }
      Record[--RecLen] = '\0';         // Wipe out CR.

      if (feof(FileArray[FileIndex])) {
         if (FileIndex > 0) {
            fclose(FileArray[FileIndex--]);
            goto GetNextLine;
         }
         return ( (c = LEX_EOF));      // Return indicator.
      }

      CurLine++;                       // Bump up line number.

      if (CHECK_DEBUG()) {             // Display source line on log.
         strncpy(Msg, Record, RecLen);
         Msg[RecLen] = '\0';
         fprintf(ListFile, "%d: %s\n", CurLine, Msg);
      }

      CurLinePtr = Record;             // Start of record.
      CurColumn = 1;                   // Current column number.

      if (RecLen == 0)
         return ( (c = LEX_EOL) );
   }

   return ((c = *CurLinePtr++));       // Return current character.


}

