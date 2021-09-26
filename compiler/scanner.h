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
//            Module:  scanner.h
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


#define MAX_INPUT_FILES 20             // Maximum open files.

//-----------------------------------------------------------------------------
// Error codes returned by various routines...
//-----------------------------------------------------------------------------

#define  SCAN_OK             0         // Not an error.
#define  SCAN_UNMATCHED      1         // Unmatched parenthesis or comments.
#define  SCAN_BADHEX         2         // Bad hex chars.
#define  SCAN_BADCHAR        3         // Bad ASCII character.
#define  SCAN_STRINGLEN      4         // String too long.
#define  SCAN_SRCLONG        5         // Source line too long.
#define  SCAN_BADNUMBER      6         // Bad number.
#define  SCAN_NUMBERTOOBIG   7         // Decimal number too big.
#define  SCAN_TOOMANYFILES   8         // Too many input files to handle.



//-----------------------------------------------------------------------------
// Keyword table structure...
//-----------------------------------------------------------------------------
typedef struct _KeyWordTab {
   char*              Keyword;
   int                KeyID;
   struct _KeyWordTab *Next;
} KeyWordTab;



//-----------------------------------------------------------------------------
// ID -- Identifier structure...
//-----------------------------------------------------------------------------
typedef struct Ident {
   BYTE      ID;                       // Identifier number.
   SYMTAB*   Symbol;                   // Pointer to symbol table entry.
   USHORT    Line;                     // Line number of symbol in source.
   USHORT    Column;                   // Starting col of symbol in source.
} ID;




//-----------------------------------------------------------------------------
// Identifiers...
// see also:  operator.h and keywords.h...
//-----------------------------------------------------------------------------

enum {

   //--------------------------------------------------------------------------
   // Miscellaneous symbolic identifiers...
   //--------------------------------------------------------------------------
   LABEL        = 250,                 // Label identifier.
   FUNCT        = 251,                 // Function identifier.
   STRING       = 252,                 // Literal string identifier.
   NUMBER       = 253,                 // Literal number identifier.
   SYMBOL       = 254,                 // Symbol identifier.
   EOM          = 255                  // End of source module.
};


//-----------------------------------------------------------------------------
// Initialize Lexical analyzer. Open source file...
//-----------------------------------------------------------------------------

int  ScanInit( void );                 // Initialize scanner.


//-----------------------------------------------------------------------------
// Terminate Lexical analyzer. Close source file (and any included files)...
//-----------------------------------------------------------------------------

int  ScanTerm( void );


//-----------------------------------------------------------------------------
// Set current file to scan. Pushes file onto file stack...
//-----------------------------------------------------------------------------

int  ScanFile( FILE* FIn);             // Set current file to scan.


//-----------------------------------------------------------------------------
// Set current line to scan. Pushes line onto stack. Remainder of current
// line is not parsed...
//-----------------------------------------------------------------------------

int  ScanLine(char* Line);             // Set current line to scan.


//-----------------------------------------------------------------------------
// Set keyword scan table...
//-----------------------------------------------------------------------------

KeyWordTab* ScanSetKeyTab( KeyWordTab* KeyTab );


//-----------------------------------------------------------------------------
// ScanGetSource() -- Return ptr and length of current line...
//-----------------------------------------------------------------------------

int  ScanGetSource( char **s, int  *Len);


//-----------------------------------------------------------------------------
// ScanNextLine() -- Advance to next line in source module...
//-----------------------------------------------------------------------------

int  ScanNextLine( void );


//-----------------------------------------------------------------------------
// Lexical analyzer routine...
//-----------------------------------------------------------------------------

int  Scan( ID  *Id);


//-----------------------------------------------------------------------------
// Scan next token and check it for a match...
//-----------------------------------------------------------------------------

Bool ScanExpect( int  Token);


//-----------------------------------------------------------------------------
// Scan next token and check it for a match...
//-----------------------------------------------------------------------------

int  ScanPeek( void );


//-----------------------------------------------------------------------------
// ScanPushBack() -- Push back an identifier for next time...
//-----------------------------------------------------------------------------

int   ScanPushBack( ID  *Id);


//-----------------------------------------------------------------------------
// ScanIsEOC() -- Check to see if we are at end of clause, that is,
//                the next identifier is a SEMIColon...
//-----------------------------------------------------------------------------

Bool  ScanIsEOC(void);

