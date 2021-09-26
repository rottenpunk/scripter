//-----------------------------------------------------------------------------
//
//
//                      GENERALIZED MEMORY POOL MANAGER
//
//        (C) Copyright John C. Overton, Inc. 1998. All Rights Reserved.
//               Advanced Communication Development Tools, Inc
//
//        No part of this file may be duplicated, revised, translated,
//        localized or modified in any manner or compiled, linked or
//        uploaded or downloaded to or from any computer system without
//        the prior written consent of Advanced Communication Development
//        Tools, Inc, or John C. Overton.
//
//
//            Module:  mempool.h
//
//             Title:  Generalized memory pool manager
//
//       Description:  This file contains general definitions for the
//                     generalized memory pool manager.
//
//            Author:  J.C. Overton
//
//              Date:  04/06/98
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


//-----------------------------------------------------------------------------
// MemPoolCreate() -- Create a new memory pool. Return anchor to new pool...
//-----------------------------------------------------------------------------
void*  MemPoolCreate( USHORT SegmentSize );

//-----------------------------------------------------------------------------
// MemPoolDestroy() -- Destroy memory pool and all allocated memory...
//-----------------------------------------------------------------------------
void  MemPoolDestroy( void* AnchorPtr);

//-----------------------------------------------------------------------------
// MemPoolAlloc() -- Allocate a new piece of memory from pool...
//-----------------------------------------------------------------------------
void* MemPoolAlloc(void* AnchorPtr, USHORT Size);

//-----------------------------------------------------------------------------
// MemPoolFree() -- Free a piece of memory that was allocated from pool...
//-----------------------------------------------------------------------------
Bool  MemPoolFree(void* p);            // Returns FALSE if error.
