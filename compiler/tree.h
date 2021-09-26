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

#define TREE_MIN_POOL_SIZE 1024

//-----------------------------------------------------------------------------
//           Function return values...
//-----------------------------------------------------------------------------

#define  TREE_OK             0         // Success.
#define  TREE_KEYNOTFOUND    1         // Key not found.
#define  TREE_NO_MEMORY      2         // Insufficient memory.
#define  TREE_INFOSIZERR     3         // KeyInfo size != new KeyInfo

//-----------------------------------------------------------------------------
//                 KEY     Key Element Control Block
//-----------------------------------------------------------------------------

typedef struct _KEY {
   struct _KEY      *KeyKeyQ;          // Ptr to higher key in node.
   struct _NODE     *KeyChildNode;     // Ptr to child subtree.
   void             *KeyDataPtr;       // Address of data.
   unsigned int      KeyDataLen;       // Length of data.
   void             *KeyPtr;           // Address of key.
   unsigned int      KeyLen;           // Length of key.
} KEY;


//-----------------------------------------------------------------------------
//                  NODE     n-way Tree Node Control Block
//-----------------------------------------------------------------------------

typedef struct _NODE {
   struct _KEY      *NodeKeyQ;         // Ptr to key element list.
   struct _NODE     *NodeChildNode;    // Ptr to left node sibling.
   unsigned int      NodeKeyCount;     // Current key count.
} NODE;


//-----------------------------------------------------------------------------
//                 TREE     (Tree Anchor Block)
//-----------------------------------------------------------------------------

typedef struct _TREE {
   void             *TreeMemPool;      // Memory pool handle.
   struct _NODE     *TreeRootNode;     // Pointer to root node.
   unsigned int      TreeOrder;        // Number keys causing split.
   unsigned int      TreeLevels;       // Number levels in tree.
   unsigned long     TreeNodes;        // Number nodes in tree.
   unsigned long     TreeKeys;         // Number keys in tree.
   unsigned long     TreeInserts;      // Number InsertKey calls.
   unsigned long     TreeSplits;       // Number TreeSplit calls.
   unsigned long     TreeDeletes;      // Number DeleteKey calls.
} TREE;


//-----------------------------------------------------------------------------
//                 TREESTATS
//-----------------------------------------------------------------------------

typedef struct _TREESTATS {
   unsigned int      TreeOrder;        // Number keys causing split.
   unsigned int      TreeLevels;       // Number levels in tree.
   unsigned long     TreeNodes;        // Number nodes in tree.
   unsigned long     TreeKeys;         // Number keys in tree.
   unsigned long     TreeInserts;      // Number InsertKey calls.
   unsigned long     TreeSplits;       // Number TreeSplit calls.
   unsigned long     TreeDeletes;      // Number DeleteKey calls.
} TREESTATS;


//-----------------------------------------------------------------------------
//    Function: your_TreeTraverse_exit_function
//
//     Returns: TRUE    to continue traversal
//              FALSE   to terminate traversal
//
//  A function pointer of type TRAVFUNC is passed to TreeTraverse,
//  which drives that function for each key in the tree.
//-----------------------------------------------------------------------------
typedef Bool (*TRAVFUNC) (
                    void *,            // Ptr to Key string.
                    unsigned int,      // Len of Key string.
                    void *,            // Ptr to stored data.
                    unsigned int,      // Len of stored data.
                    void * );          // User parameter dword.


//-----------------------------------------------------------------------------
//             F U N C T I O N   D E C L A R A T I O N S
//-----------------------------------------------------------------------------



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
   unsigned int      TreePoolSize,     // #Bytes of Tree memory pool.
   TREE            **RetTree );        // Ptr to returned tree anchor



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
   unsigned int     *DataLen );        // Ptr to returned Data len.




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
   void             *KeyArgPtr,        // Ptr to Argument key.
   unsigned int      KeyArgLen,        // Length to argument key.
   void             *DataPtr,          // Ptr to related data (|NULL)
   unsigned int      DataLen );        // Length of related data.




//-----------------------------------------------------------------------------
//    Function: TreeDelete
//
//     Returns: TREE_OK  if successful
//              TREE_KEYNOTFOUND  if key not found
//
//  Delete a key and its related data from the TREE
//-----------------------------------------------------------------------------

int   TreeDelete(
   TREE             *Tree,             // Ptr to TREE.
   void             *KeyArgPtr,        // Ptr to argument key.
   unsigned int      KeyArgLen );      // Len of argument key.



//-----------------------------------------------------------------------------
//    Function: TreeTraverse
//
//     Returns: void
//
//  Traverse the tree in key logical sequence. For each key, the
//  caller's exit function is called and passed the following
//  parameter list:
//                      void *               Ptr to Key string
//                      unsigned int         Length of Key string
//                      void *               Ptr to stored data
//                      unsigned int         Length of stored data
//                      void *               User parameter dword
//
//-----------------------------------------------------------------------------

void   TreeTraverse(
   TREE             *Tree,             // Ptr to TREE.
   TRAVFUNC          ExitFunc,         // Ptr to exit function.
   void             *ExitParm );       // User parameter.


//-----------------------------------------------------------------------------
//    Function: TreeQStats  (Query Tree Statistics)
//
//     Returns: TREE_OK
//
//  Return tree statistics information in the caller's TreeStats
//  structure.
//-----------------------------------------------------------------------------

int   TreeQStats(
   TREE             *Tree,             // Pointer to TREE.
   TREESTATS        *TreeStats );      // Pointer to Statistics block.


//-----------------------------------------------------------------------------
//    Function: TreeDestroy
//
//     Returns: TREE_OK
//
//  Remove all traces of a TREE from the environment.
//-----------------------------------------------------------------------------

int   TreeDestroy(
   TREE             *Tree );           // Ptr to TREE.
