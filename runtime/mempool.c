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
//            Module:  mempool.c
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
#include <stdio.h>
#include "executor.h"

//-----------------------------------------------------------------------------
// Various definitions...
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Related structures...
//-----------------------------------------------------------------------------
typedef struct MemElement {            // A piece of allocated storage.
   struct MemElement*      Next;       // Next free or allocated element.
   struct MemElement*      Prev;       // Previous free or allocated element.
   struct MemSegment*      Segment;    // Ptr to segment that element is in.
   USHORT                  Size;       // Size of this element.
   USHORT                  Id;         // Identifier to verify memory.
} MEM_ELEMENT;

#define MEM_ELEMENT_ID_ALLOC  0x4d41   // Memory ident for allocated element.
#define MEM_ELEMENT_ID_FREE   0x4d46   // Memory ident for freed element.

typedef struct MemSegment {            // Memory segment.
   struct MemSegment*      Next;       // Pointer to next memory segment.
   struct MemSegment*      Prev;       // Pointer to previos memory segment.
   struct MemPoolAnchor*   Anchor;     // Pointer to anchor structure.
   USHORT                  Size;       // Size of this segment.
   USHORT                  Residual;   // Remaining unallocated size.
   BYTE*                   FreeSpace;  // Space remaining in segment.
} MEM_SEGMENT;

typedef struct MemPoolAnchor {
   struct MemPoolAnchor*   Next;       // Pointer to next anchor on chain.
   struct MemPoolAnchor*   Prev;       // Pointer to next anchor on chain.
   struct MemSegment*      SegFirst;   // First segment in this pool.
   struct MemElement*      Allocated;  // Allocated storage chain.
   struct MemElement*      Free;       // Freed storage chain.
   USHORT                  SegSize;    // Size to use to allocate segments.
} MEM_POOL_ANCHOR;


//-----------------------------------------------------------------------------
// Global memory definitions...
//-----------------------------------------------------------------------------
MEM_POOL_ANCHOR* MemoryPoolAnchor = NULL;


//-----------------------------------------------------------------------------
// MemPoolCreate() -- Create a new memory pool. Return anchor to new pool...
//-----------------------------------------------------------------------------
void*  MemPoolCreate( USHORT SegmentSize)
{
   MEM_POOL_ANCHOR* Anchor;

   //--------------------------------------------------------------------------
   // Alocate an anchor block for this pool, then chain in with rest of
   // anchor blocks...
   //--------------------------------------------------------------------------
   Anchor = (MEM_POOL_ANCHOR*) GMEM(sizeof(MEM_POOL_ANCHOR));
   Anchor->Next = MemoryPoolAnchor;        // Chain this anchor block in.
   Anchor->SegSize = SegmentSize;          // Save segment size.
   if (MemoryPoolAnchor)                   // If there's some on chain already,
      MemoryPoolAnchor->Prev = Anchor;     // Then chain back to new one.
   MemoryPoolAnchor = Anchor;              // New one is now new head of chain.
   return (void*) Anchor;                  // Return with new pool anchor.
}


//-----------------------------------------------------------------------------
// MemPoolDestroy() -- Destroy memory pool and all allocated memory...
//-----------------------------------------------------------------------------
void  MemPoolDestroy( void* AnchorPtr)
{
   MEM_POOL_ANCHOR* Anchor = AnchorPtr;
   MEM_SEGMENT*     Segment;
   MEM_SEGMENT*     NextSegment;

   Segment = Anchor->SegFirst;
   while (Segment) {
      NextSegment = Segment->Next;
      FMEM(Segment);
      Segment = NextSegment;
   }
   FMEM(Anchor);
}


//-----------------------------------------------------------------------------
// MemPoolDump() -- Dump all segments of all memory pools...
//-----------------------------------------------------------------------------
void  MemPoolDump( void )
{
   MEM_POOL_ANCHOR* Anchor = MemoryPoolAnchor;
   MEM_SEGMENT*     Segment;
   //FILE*            DumpFile;

   //if ((DumpFile = fopen("pool.lst", "w")) == NULL) {
   //   fprintf(stderr, "Can not write to pool.lst to dump memory pools!\n");
   //   return;
   //}
   while (Anchor) {
      printf("\nMemory Pool...\n");
      Dump((char*) Anchor, 0, sizeof(MEM_POOL_ANCHOR), 0);
      printf("\n");
      Segment = Anchor->SegFirst;
      while (Segment) {
         Dump((char*) Segment, 0, Segment->Size, 0);
         printf("\n");
         Segment = Segment->Next;
      }
      Anchor = Anchor->Next;           // Go to next memory pool.
   }
   printf("\n");
   // fclose(DumpFile);
}


//-----------------------------------------------------------------------------
// MemPoolAlloc() -- Allocate a new piece of memory from pool...
//-----------------------------------------------------------------------------
void* MemPoolAlloc(void* AnchorPtr, USHORT Size)
{
   MEM_POOL_ANCHOR* Anchor = AnchorPtr;
   MEM_SEGMENT*     Segment;
   MEM_ELEMENT*     Element;
   USHORT           i;
   BYTE*            p;


   //--------------------------------------------------------------------------
   // First, go thru free chain to see if there's a free one...
   //--------------------------------------------------------------------------
   Element = Anchor->Free;             // First free in chain.
   while(Element) {
      if (Element->Size == Size) {
         // Unchain from free chain...
         if (Element->Next)
            Element->Next->Prev = Element->Prev;
         if (Element->Prev)
            Element->Prev->Next = Element->Next;
         else
            Anchor->Free = Element->Next;
         goto ChainInElement;
      }
      Element = Element->Next;
   }

   //--------------------------------------------------------------------------
   // If we're here, then we need to try to allocate new element. See if room
   // in one of the segments...
   //--------------------------------------------------------------------------
   Segment = Anchor->SegFirst;

TryNewSegment:
   i = Size + sizeof(MEM_ELEMENT);     // Total size to allocate.
   while(Segment) {
      if (Segment->Residual >= i) {    // If there's room in this segment...
         Element = (MEM_ELEMENT*) Segment->FreeSpace;   // Allocate a chunk.
         Segment->FreeSpace += i;      // Adjust free pointer.
         Segment->Residual  -= i;      // Adjust remaining free space counter.
         Element->Size = Size;         // Save size of newly alloc'd element.
         Element->Segment = Segment;   // Point back to segment element is in.
         goto ChainInElement;          // Go chain it into allocated chain.
      }
      Segment = Segment->Next;         // Try next segment.
   }

   //--------------------------------------------------------------------------
   // No space on free chain, no space in any segments, so we now need to
   // allocate a new segment...
   //--------------------------------------------------------------------------
   if (Size > Anchor->SegSize - sizeof(MEM_SEGMENT) - sizeof(MEM_ELEMENT))
      i = Size + sizeof(MEM_SEGMENT) + sizeof(MEM_ELEMENT);
   else
      i = Anchor->SegSize;
   Segment = (MEM_SEGMENT*) GMEM(i);     // Allocate new segment.
   if (Segment == NULL)                  // If we couldn't allocate new segment,
      return NULL;                       // then return with nothing.
   Segment->Next = Anchor->SegFirst;     // Chain new segment into pool anchor.
   if (Anchor->SegFirst != NULL)         // If there was a prev head of chain,
      Anchor->SegFirst->Prev = Segment;  // Then link backward ptr to new one.
   Anchor->SegFirst = Segment;           // New one is now first in chain.
   Segment->Size = i;                    // Save size of newly alloc'd segment.
   Segment->Residual = i - sizeof(MEM_SEGMENT);  // Size of allocatable space.
   Segment->FreeSpace = ((BYTE*) Segment) +      // Where free space starts.
                        sizeof(MEM_SEGMENT);     // After segment control block.
   goto TryNewSegment;                   // Go try to allocate from new segment.

   //--------------------------------------------------------------------------
   // Memory element allocated, chain into allocate chain, then return it
   // to caller...
   //--------------------------------------------------------------------------
ChainInElement:
   // Chain onto head of allocate chain...
   Element->Next = Anchor->Allocated;
   if (Anchor->Allocated)
      Anchor->Allocated->Prev = Element;
   Element->Prev = NULL;
   Anchor->Allocated = Element;
   Element->Id = MEM_ELEMENT_ID_ALLOC;   // Mark as allocated storage.

   p = ((BYTE*)Element) + sizeof(MEM_ELEMENT);  // Point to memory to return.
   memset(p, 0, Size);                          // Clear memory.
   return (void*) p;
}



//-----------------------------------------------------------------------------
// MemPoolFree() -- Free a piece of memory that was allocated from pool...
//-----------------------------------------------------------------------------
Bool MemPoolFree(void* p)
{
   MEM_ELEMENT*     Element;
   MEM_POOL_ANCHOR* Anchor;

   Element = (MEM_ELEMENT*)(((BYTE*) p) -         // Back up to header.
             sizeof(MEM_ELEMENT));
   if (Element->Id != MEM_ELEMENT_ID_ALLOC)       // Check id for allocated mem.
      return False;                               // It's bad! Return error.

   Element->Id = MEM_ELEMENT_ID_FREE;             // Now, mark memory as freed.
   Anchor = Element->Segment->Anchor;             // Point to pool anchor.

   //--------------------------------------------------------------------------
   // Unchain element from alocate chain...
   //--------------------------------------------------------------------------
   if (Element->Next)
      Element->Next->Prev = Element->Prev;
   if (Element->Prev)
      Element->Prev->Next = Element->Next;

   //--------------------------------------------------------------------------
   // Now, put onto free chain...
   //--------------------------------------------------------------------------

   Element->Next = Anchor->Free;
   if (Anchor->Free != NULL)
      Anchor->Free->Prev = Element;
   Anchor->Free = Element;

   return True;                        // Return indicating no error.
}
