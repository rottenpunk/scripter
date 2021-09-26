//-----------------------------------------------------------------------------
//
//
//                      GENERALIZED IN-MEMORY TREE SERVICE
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
//            Module:  tree.c
//
//             Title:  Generalized in-memory multiway tree service
//
//       Description:  This file contains routines that make up the generalized
//                     in-memory tree service.
//
//            Author:  J.C. Overton
//
//              Date:  06/20/92 (originally)
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
#include <stdlib.h>
#include <string.h>

#include "typedefs.h"
#include "tree.h"

#define MEM_POOL 1               // Use memory pools.

#ifdef MEM_POOL
#include "mempool.h"
#endif


//-----------------------------------------------------------------------------
//  Internal static routines...
//-----------------------------------------------------------------------------


static Bool CreateNode(    TREE *,
                           NODE ** );

static Bool SearchNode(    NODE *,
                           void *,
                           unsigned int,
                           KEY ** );

static int  InsertNode(    TREE *,
                           NODE *,
                           void *,
                           unsigned int,
                           KEY **,
                           KEY ** );

static int  InsertKey(     TREE *,
                           NODE *,
                           KEY *,
                           KEY *,
                           KEY ** );

static void TraverseNode(  NODE *,
                           unsigned int   *,
                           TRAVFUNC,
                           void * );

static int  UpdateKeyData( TREE *,
                           KEY *,
                           void *,
                           unsigned int   );



//-----------------------------------------------------------------------------
// Memory pool function redefines...
//-----------------------------------------------------------------------------

#ifdef MEM_POOL

#define GMEMPOOL(n)       (MemPoolCreate(n))
#define GMEMSUB(n, len)   (MemPoolAlloc(n, len))
#define FMEMSUB(p)        (MemPoolFree(p))
#define FMEMPOOL(p)       (MemPoolDestroy(p))

#else

#define GMEMPOOL(n)       (n)
#define GMEMSUB(n, len)   (calloc(len, 1))
#define FMEMSUB(p)        (free(p))
#define FMEMPOOL(p)

#endif


//-----------------------------------------------------------------------------
//    Function: TreeCreate
//
//     Returns: TREE_OK if successful
//              TREE_NO_MEMORY if unable to allocate memory
//
//  Create a multiway tree of the specified order. A ptr to TREE
//  is passed back for use in all other Tree Manager API functions.
//-----------------------------------------------------------------------------

int   TreeCreate(
   unsigned int      TreeOrder,        // Max subtrees per node.
   unsigned int      TreePoolSize,     // Size of memory chunks.
   TREE            **RetTree )         // Ptr to returned tree anchor.
{
   TREE             *Tree;
   void             *MemPool;

   if (TreePoolSize < TREE_MIN_POOL_SIZE)
      TreePoolSize = TREE_MIN_POOL_SIZE;

   if ((MemPool = GMEMPOOL( TreePoolSize )) != NULL) {
      Tree = (TREE *) GMEMSUB(MemPool, sizeof( TREE ));
      Tree->TreeMemPool   = MemPool;
      Tree->TreeOrder     = TreeOrder;
      if ( Tree->TreeOrder % 2 == 0 )  // Ensure tree order is odd.
         Tree->TreeOrder++;
      Tree->TreeLevels    = 1;
      Tree->TreeNodes     = 0;
      Tree->TreeKeys      = 0;
      Tree->TreeInserts   = 0;
      Tree->TreeSplits    = 0;
      Tree->TreeDeletes   = 0;
      if ( CreateNode( Tree, &Tree->TreeRootNode )) {
         *RetTree = Tree;
         return( TREE_OK );
      }
   }
   *RetTree = NULL;
   return( TREE_NO_MEMORY );
}





//-----------------------------------------------------------------------------
//    Function: TreeLocate
//
//     Returns: TREE_OK if key found
//              TREE_KEYNOTFOUND  if key not found
//
//  Search a tree for the requested key argument string.
//-----------------------------------------------------------------------------

int  TreeLocate(
   TREE             *Tree,             // Ptr to TREE.
   void             *KeyArgPtr,        // Ptr to Key Argument.
   unsigned int      KeyArgLen,        // Len of Key Argument.
   void            **DataPtr,          // Ptr to returned Data ptr.
   unsigned int     *DataLen )         // Ptr to returned Data length.
{
   KEY              *Key;

   if ( SearchNode(Tree->TreeRootNode, KeyArgPtr, KeyArgLen, &Key)) {
      *DataPtr = Key->KeyDataPtr;
      *DataLen = Key->KeyDataLen;

      return( TREE_OK );
   }
  *DataPtr = NULL;
  *DataLen = 0;
   return( TREE_KEYNOTFOUND );
}



//-----------------------------------------------------------------------------
//    Function: TreeStore
//
//     Returns: TREE_OK if successful
//              TREE_NO_MEMORY  if insert failed
//
//  Store data in the tree with its associated key. If the KEY
//  already exists, it is updated to point to the new data. If the
//  existing data area is of sufficient length to contain the new
//  data, it is reused, otherwise it is freed.
//-----------------------------------------------------------------------------

int   TreeStore(
   TREE             *Tree,             // Ptr to TREE.
   void             *KeyArgPtr,        // Ptr to argument key.
   unsigned int      KeyArgLen,        // Len of argument key.
   void             *DataPtr,          // Ptr to related data.
   unsigned int      DataLen )         // Length of related data.
{
   NODE             *NewRootNode;
   KEY              *Key = NULL;
   KEY              *KeyPerc= NULL;
   int               rc;


   rc = InsertNode( Tree,
                    Tree->TreeRootNode,
                    KeyArgPtr,
                    KeyArgLen,
                    &Key,
                    &KeyPerc );

   if ( rc == TREE_OK ) {

      //-----------------------------------------------------------------------
      //    KEY inserted successfully:
      //-----------------------------------------------------------------------

      rc = UpdateKeyData( Tree, Key, DataPtr, DataLen );

      //-----------------------------------------------------------------------
      //   If the percolated KEY pointer is non-null, the tree has grown
      //   one level. We create a new root node and re-anchor the tree.
      //-----------------------------------------------------------------------

      if ( KeyPerc ) {
         if (! CreateNode( Tree, &NewRootNode ))
            return( TREE_NO_MEMORY );
         NewRootNode->NodeChildNode = Tree->TreeRootNode;
         Tree->TreeRootNode = NewRootNode;
         ++Tree->TreeLevels;
         rc = InsertKey( Tree, NewRootNode, NewRootNode->NodeKeyQ,
                           KeyPerc, &KeyPerc );
      }
   }
   return( rc );
}





//-----------------------------------------------------------------------------
//    Function: TreeDelete
//
//     Returns: TREE_OK  if successful
//              TREE_KEYNOTFOUND  if key not found
//
//  Delete a key and its related data from the TREE
//-----------------------------------------------------------------------------

int   TreeDelete(
   TREE              *Tree,            // Ptr to TREE.
   void              *KeyArgPtr,       // Ptr to argument key.
   unsigned int       KeyArgLen )      // Len of argument key.
{

   // Not presently supported.

   return( TREE_KEYNOTFOUND );
}




//-----------------------------------------------------------------------------
//    Function: TreeTraverse
//
//     Returns: void
//
//  Traverse the tree in key logical sequence. For each key, the
//  caller's exit function is called and passed the following
//  parameter list:
//                      unsigned int         Tree node level
//                      void *               Ptr to Key arg.
//                      unsigned int         Len of Key arg.
//                      void *               Ptr to stored data
//                      unsigned int         Length of stored data
//                      unsigned long        User parameter dword
//
//-----------------------------------------------------------------------------

void   TreeTraverse(
   TREE             *Tree,             // Ptr to TREE.
   TRAVFUNC          ExitFunc,         // Ptr to exit function.
   void             *ExitParm )        // User parameter.
{
   unsigned int      Level = 0;

   TraverseNode( Tree->TreeRootNode, &Level, ExitFunc, ExitParm );
}



//-----------------------------------------------------------------------------
//    Function: TreeQStats  (Query Tree Statistics)
//
//     Returns: TREE_OK
//
//  Return tree statistics information in the caller's TreeStats
//  structure.
//-----------------------------------------------------------------------------
int   TreeQStats(
   TREE            *Tree,              // Pointer to TREE.
   TREESTATS       *TreeStats )        // Pointer to Statistics block.
{

   TreeStats->TreeOrder      = Tree->TreeOrder;
   TreeStats->TreeLevels     = Tree->TreeLevels;
   TreeStats->TreeNodes      = Tree->TreeNodes;
   TreeStats->TreeKeys       = Tree->TreeKeys;
   TreeStats->TreeInserts    = Tree->TreeInserts;
   TreeStats->TreeSplits     = Tree->TreeSplits;
   TreeStats->TreeDeletes    = Tree->TreeDeletes;

   return( TREE_OK );
}




//-----------------------------------------------------------------------------
//    Function: TreeDestroy
//
//     Returns: TREE_OK
//
//  Remove all traces of a TREE from the environment.
//-----------------------------------------------------------------------------

int   TreeDestroy(
   TREE            *Tree )             // Ptr to TREE.
{
   FMEMPOOL(Tree->TreeMemPool);        // Free all memory used by tree.

   return( TREE_OK );
}





//-----------------------------------------------------------------------------
//    Function: SearchNode
//
//     Returns: True if KEY found
//              False if KEY not found
//
//  Recursive descent through the TREE for the purpose of locating
//  a key. A ptr to the located KEY or NULL is passed back.
//-----------------------------------------------------------------------------

Bool  SearchNode(
   NODE             *Node,             // Ptr to NODE to be searched.
   void             *KeyArgPtr,        // Ptr to search argument.
   unsigned int      KeyArgLen,        // Len of search argument.
   KEY             **KeyRet )          // Ptr to returned KEY ptr.
{
   NODE             *ChildNode;
   KEY              *Key;
   int               result;

   ChildNode = Node->NodeChildNode;

   //--------------------------------------------------------------------------
   //  Search the current NODE for the desired KEY.
   //--------------------------------------------------------------------------

   for ( Key = Node->NodeKeyQ;
         Key != (KEY *) Node;
         Key = Key->KeyKeyQ ) {

      result = memcmp( KeyArgPtr,
                       Key->KeyPtr,
                       KeyArgLen <= Key->KeyLen ?
                                    KeyArgLen : Key->KeyLen);
      if (result == 0)
         result = KeyArgLen - Key->KeyLen;

      if ( result >  0 ) {             // If argument > KEY value ...
         ChildNode = Key->KeyChildNode;// Save child sub-tree ptr.
      }
      else {
         if ( result <  0 )            // If argument < KEY value ...
            break;                     // Target KEY not in this node.

         else {                        // KEY found
            *KeyRet = Key;             // Return ptr to existing KEY.
            return( True );
         }
      }
   }

   //--------------------------------------------------------------------------
   //   Descend the tree recursively until we arrive at a leaf node
   //   (i.e. null child pointer ).
   //--------------------------------------------------------------------------

   if ( ChildNode )
      return( SearchNode( ChildNode, KeyArgPtr, KeyArgLen, KeyRet ));

   *KeyRet = NULL;
   return( False );
}








//-----------------------------------------------------------------------------
//    Function: InsertNode
//
//     Returns: TREE_OK or return( InsertKey(..))
//
//  Recursive descent through the TREE for the purpose of inserting
//  a key. Two pointers are passed back: (1) ptr to inserted (or
//  existing) KEY ; (2) ptr to KEY percolated by a split of the
//  NODE at the current recursion level.
//-----------------------------------------------------------------------------

static int InsertNode(
   TREE             *Tree,             // Ptr to TREE structure.
   NODE             *Node,             // Ptr to NODE to be searched.
   void             *KeyArgPtr,        // Ptr to argument key.
   unsigned int      KeyArgLen,        // Len of argument key.
   KEY             **KeyRet,           // Ptr to ptr to KEY struct.
   KEY             **KeyPerc )         // Ptr to ptr to split mid KEY.
{
   NODE             *ChildNode;
   KEY              *Key;
   KEY              *KeyPrev;
   unsigned int      KeyLen;
   int               result;

   ChildNode = Node->NodeChildNode;
   KeyPrev  = (KEY *) Node;

   //--------------------------------------------------------------------------
   //  Search the current NODE for the desired KEY.
   //--------------------------------------------------------------------------

   for ( Key = Node->NodeKeyQ;
         Key != (KEY *) Node;
         Key = Key->KeyKeyQ ) {

      result = memcmp( KeyArgPtr,
                       Key->KeyPtr,
                       KeyArgLen <= Key->KeyLen ?
                                    KeyArgLen : Key->KeyLen);
      if (result == 0)
         result = KeyArgLen - Key->KeyLen;

      if ( result >  0 ) {             // If argument > KEY value ...
         ChildNode = Key->KeyChildNode;// Save child sub-tree ptr.
         KeyPrev  = Key;               // Save KEY pointer.
      }
      else {
         if ( result <  0 )            // If argument < KEY value,
            break;                     // Target KEY not in this node.

         else {                        // KEY found.
            *KeyRet = Key;             // Return ptr to existing KEY.
            return( TREE_OK );
         }
      }
   }

   //--------------------------------------------------------------------------
   //   Descend the tree recursively until we arrive at a leaf node
   //   (i.e. null child pointer ).
   //--------------------------------------------------------------------------

   if ( ChildNode ) {

      //-----------------------------------------------------------------------
      //    Descend to next lower level NODE in the tree
      //-----------------------------------------------------------------------

      result = InsertNode( Tree,
                           ChildNode,
                           KeyArgPtr,
                           KeyArgLen,
                           KeyRet,
                           KeyPerc );

      if ( result == TREE_OK && *KeyPerc != NULL )
         result = InsertKey( Tree, Node, KeyPrev, *KeyPerc, KeyPerc );
   }

   //--------------------------------------------------------------------------
   //    Create a KEY structure to contain the key to be inserted.
   //--------------------------------------------------------------------------

   else {
      KeyLen = (( sizeof(KEY) + KeyArgLen ) * 2 + 1)/2;

      if ( (Key = (KEY *) GMEMSUB( Tree->TreeMemPool, KeyLen+1 )) != NULL) {
         Key->KeyLen = KeyArgLen;
         Key->KeyPtr = (void *) ((char *) Key + sizeof(KEY));
         memcpy( Key->KeyPtr, KeyArgPtr, KeyArgLen );
         ((char *)Key->KeyPtr)[KeyArgLen] = 0;    // Null terminate.
         ++Tree->TreeKeys;
         result = InsertKey( Tree, Node, KeyPrev, Key, KeyPerc );
         *KeyRet = Key;                           // Return ptr to new KEY.
      }
   }
   return( result );
}





//-----------------------------------------------------------------------------
//    Function: InsertKey
//
//     Returns: TREE_OK or TREE_NO_MEMORY
//
//  Insert a key in the specified NODE adjacent to the specified
//  KEY.  If the NODE KEY capacity is exceeded, divide the keys
//  evenly between the existing NODE and a newly created NODE which
//  becomes the child subtree of the middle KEY.
//
//-----------------------------------------------------------------------------

int   InsertKey(
   TREE             *Tree,             // Ptr to TREE structure.
   NODE             *Node,             // Ptr to target NODE.
   KEY              *KeyPrev,          // Ptr to queue prev NODE/KEY.
   KEY              *Key,              // Ptr to KEY to be inserted.
   KEY             **KeyPerc )         // Ptr to ptr to split mid KEY.
{
   NODE             *NewNode;
   KEY              *KeyHold;
   KEY              *KeyMid;
   unsigned int      Split;


   //--------------------------------------------------------------------------
   //   Link the new KEY occurrence into the NODE key list.
   //--------------------------------------------------------------------------

   KeyHold = KeyPrev->KeyKeyQ;
   KeyPrev->KeyKeyQ = Key;
   Key->KeyKeyQ = KeyHold;
   ++Tree->TreeInserts;
   if ( ++Node->NodeKeyCount < Tree->TreeOrder ) {
      *KeyPerc = NULL;                 // Nullify percolated KEY ptr.
      return( TREE_OK );
   }

   //--------------------------------------------------------------------------
   //  The NODE KEY limit has been reached.
   //    (1) Create a new node.
   //    (2) Divide the KEYs equally between the existing NODE
   //        and the new NODE, leaving the middle KEY free.
   //    (3) Make the new NODE the child of the middle KEY.
   //    (4) Percolate the middle KEY into the node of our parent KEY.
   //--------------------------------------------------------------------------

   if (! CreateNode( Tree, &NewNode ))
      return( TREE_NO_MEMORY );
   ++Tree->TreeSplits;
   Split = Node->NodeKeyCount / 2;
   if (! Node->NodeKeyCount % 2 ) --Split;

   //--------------------------------------------------------------------------
   //   Truncate the existing NODE at "Split" KEYS
   //--------------------------------------------------------------------------

   Node->NodeKeyCount = 0;

   for(  Key = Node->NodeKeyQ;         // for retained Old NODE KEYS.
         Split--;
         Key = Key->KeyKeyQ ) {

      ++Node->NodeKeyCount;
      KeyHold = Key;
   }

   KeyHold->KeyKeyQ = (KEY *) Node;    // break Key chain - q to NODE.

   //--------------------------------------------------------------------------
   //  Populate the new NODE with half of the keys from the old NODE
   //--------------------------------------------------------------------------

   KeyMid = Key;                       // Save middle key ptr.
   NewNode->NodeKeyQ=KeyMid->KeyKeyQ;  // Anchor top KEY in new NODE.
   NewNode->NodeChildNode=KeyMid->KeyChildNode;   // Transfer subtree.
   KeyMid->KeyChildNode = NewNode;     // NewNODE is child of MidKey.
   KeyMid->KeyKeyQ = NULL;             // Nullify middle KEY Q->.

   for(  Key = NewNode->NodeKeyQ;      // For remaining old NODE KEYS.
         Key != (KEY *) Node;
         Key = Key->KeyKeyQ ) {
      ++NewNode->NodeKeyCount;         // Incr new NODE key count.
      KeyHold = Key;
      }

   KeyHold->KeyKeyQ = (KEY *)NewNode;  // Set last KEY Q-> new NODE.

   //--------------------------------------------------------------------------
   //   Percolate the middle KEY (and its child node) to our parent.
   //--------------------------------------------------------------------------
   *KeyPerc = KeyMid;                  // Set percolated KEY ptr.
   return( TREE_OK );
}




//-----------------------------------------------------------------------------
//    Function: CreateNode
//
//     Returns: True if successful, False if unsuccessful
//
//  Allocate and initialize a NODE structure.
//-----------------------------------------------------------------------------

Bool  CreateNode(
   TREE             *Tree,             // Ptr to TREE structure.
   NODE            **RetNode )         // Ptr to ptr to new NODE.

{
   NODE             *Node;

   if ((Node = (NODE *) GMEMSUB(Tree->TreeMemPool, sizeof( NODE ))) == NULL ) {
      *RetNode = NULL;
      return ( False );
   }
   Node->NodeChildNode = NULL;
   Node->NodeKeyQ      = (KEY *) Node; // Initial null KEY queue.
   Node->NodeKeyCount  = 0;
   ++Tree->TreeNodes;
      *RetNode = Node;
   return( True );
}





//-----------------------------------------------------------------------------
//    Function: TraverseNode
//
//     Returns: void
//
//  Recursively traverse all subtrees of the current NODE
//  "IN-ORDER"; for each KEY, drive the caller's ExitFunc.
//-----------------------------------------------------------------------------

void  TraverseNode(
   NODE             *Node,             // Ptr to current NODE.
   unsigned int     *Level,            // Ptr to recursion level ctr.
   TRAVFUNC          ExitFunc,         // Ptr to exit function.
   void             *ExitParm )        // Exit function user param.
{
   KEY              *Key;

   ++*Level;                           // Increment level counter.

   if ( Node->NodeChildNode )
      TraverseNode( Node->NodeChildNode, Level, ExitFunc, ExitParm );

   for(  Key = Node->NodeKeyQ;
         Key != (KEY *) Node;
         Key = Key->KeyKeyQ ) {

      if (!(*ExitFunc)( Key->KeyPtr,
                        Key->KeyLen,
                        Key->KeyDataPtr,
                        Key->KeyDataLen,
                        ExitParm ))
         break;
      if ( Key->KeyChildNode )
         TraverseNode( Key->KeyChildNode, Level, ExitFunc, ExitParm );

   }
   --*Level;                           // Decrement level counter.
}




//-----------------------------------------------------------------------------
//    Function: UpdateKeyData
//
//     Returns: TREE_OK if successful
//              TREE_NOMEM if MemAlloc failed.
//
//  Replace the data associated with the specified KEY.  The
//  existing data area is reused is possible. Otherwise a new
//  area is allocated and the data is copied.
//-----------------------------------------------------------------------------
static int UpdateKeyData(
   TREE             *Tree,             // Ptr to TREE.
   KEY              *Key,              // Ptr to KEY.
   void             *DataPtr,          // Ptr to related data.
   unsigned int      DataLen ) {       // Length of related data.

   void             *Data;

   if ( DataLen != 0xffff ) {

      //-----------------------------------------------------------------------
      // If new data is not the same size then free and get another.
      //-----------------------------------------------------------------------

      if ( DataLen != 0) {
         if ( DataLen != Key->KeyDataLen ) {
            if ((Data = (void *) GMEMSUB(Tree->TreeMemPool, DataLen )) != NULL) {
               if ( Key->KeyDataLen )
                  FMEMSUB(Key->KeyDataPtr );
               Key->KeyDataPtr = Data;
               Key->KeyDataLen = DataLen;
            }
         }
         memcpy( Key->KeyDataPtr, DataPtr, DataLen );
      }

      //-----------------------------------------------------------------------
      // If DataLen = 0, then we are not to copy data. Just put DataPtr
      // into Key...
      //-----------------------------------------------------------------------

      else {                           // No Len, Just store DataPtr.
         if ( Key->KeyDataLen ) {      // If previous memory, free it.
            FMEMSUB(Key->KeyDataPtr );
            Key->KeyDataLen = 0;       // No previous data memory.
         }
         Key->KeyDataPtr = DataPtr;    // Copy data pointer.
      }
   }

   //--------------------------------------------------------------------------
   // Here, we just want to delete the key.
   //--------------------------------------------------------------------------

   else {
      if ( Key->KeyDataPtr ) {
         FMEMSUB(Key->KeyDataPtr );
         Key->KeyDataPtr = NULL;
         Key->KeyDataLen = 0;
      }
   }
   return( TREE_OK );
}
