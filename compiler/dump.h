//-----------------------------------------------------------------------------
//
//
//                       GENERALIZED DEBUG DUMP SERVICE
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
//            Module:  dump.h
//
//             Title:  Generalized debugging dump routine.
//
//       Description:  This file dumps a portion of memory to a file.
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


void Dump( FILE *outfile,
           char *Buffer,
           long Offset,
           int Length,
           int BreakEvery);
