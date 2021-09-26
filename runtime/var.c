//-----------------------------------------------------------------------------
//
//
//                GENERALIZED SCRIPTING LANGUAGE EXECUTOR
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
//            Module:  variable.c
//
//             Title:  Generalized scripting language execultor
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
#include "tree.h"



#define GMEM(l)  (calloc(1, l))
#define FMEM(p)  (free(p))



//-----------------------------------------------------------------------------
// LocateVariable() return indicators...
//-----------------------------------------------------------------------------

#define LOCATE_NOTFOUND   0
#define LOCATE_LOCAL      1
#define LOCATE_MODULE     2
#define LOCATE_EXTERNAL   3
#define LOCATE_SYSTEM     4

//-----------------------------------------------------------------------------
// Instance of a local level variable name space...
//-----------------------------------------------------------------------------

typedef struct _VarLocal {
   struct _VarLocal*    Next;
   TREE*                VarTree;
} VARLOCAL;

//-----------------------------------------------------------------------------
// Instance of a module level variable name space...
//-----------------------------------------------------------------------------

typedef struct _VarModule {
   struct _VarModule*   Next;
   struct _VarLocal*    Local;
   TREE*                VarTree;
} VARMODULE;


//-----------------------------------------------------------------------------
// Instance of a extern level variable name space...
//-----------------------------------------------------------------------------

typedef struct _VarExtern {
   struct _VarExtern*   Next;   
   TREE*                ExtTree;
} VAREXTERN;

//-----------------------------------------------------------------------------
// Instance of variable name space...
//-----------------------------------------------------------------------------

typedef struct _VarAnchor {   
   struct _VarModule*   Module;   
   struct _VarExtern*   VarExt;
} VARANCHOR;

//-----------------------------------------------------------------------------
// Global variables...
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Local static functions...
//-----------------------------------------------------------------------------

int   LocateVariable(  void* Handle, char* Name, void** Value, TREE** ExtTree);
void  VarFreeTreeVars( TREE* Tree );
Bool  SortVars(        void           *KeyPtr,
                       unsigned short  KeyLen,
                       void           *DataPtr,
                       unsigned short  DataLen,
                       void           *Parm);


//-----------------------------------------------------------------------------
// Local static variables...
//-----------------------------------------------------------------------------

static void*        NullPtr = NULL;



//-----------------------------------------------------------------------------
// VarInit() -- Initialize instance of variable name space...
//-----------------------------------------------------------------------------
int VarInit(        void** Handle )
{
   *Handle = (void*) GMEM(sizeof(VARANCHOR));  // Allocate anchor.
   if (*Handle == NULL)
      return VAR_NOMEMORY;
   return VAR_OK;
}



//-----------------------------------------------------------------------------
// VarTerm() -- Terminate instance of variable name space...
//-----------------------------------------------------------------------------
int VarTerm(        void* Handle )
{
   VARANCHOR*   Va;

   Va = (VARANCHOR*) Handle;

   while (Va->Module)                  // While there's module levels...
      VarModuleExit(Handle);           // Kill them.

   while (Va->VarExt) {
	   VarExternExit(Handle);
   }

   FMEM(Handle);                       // Free Anchor control block.
   return VAR_OK;
}



//-----------------------------------------------------------------------------
// VarExtern() -- Define external variable at the module var name space level...
//-----------------------------------------------------------------------------
int VarExtern(      void* Handle, char* Name, void* Value )
{
   VARANCHOR*   Va;
   VAREXTERN*   Ve;
   int          rc;

   Va = (VARANCHOR*) Handle;
   if ((Ve = Va->VarExt) == NULL)
      return VAR_NOEXTERNLEVEL;

   if (Ve->ExtTree == NULL) {
      if (( rc = TreeCreate(5, 0, &Ve->ExtTree)) != TREE_OK)
         return VAR_TREECREATE;
   }     

   if (( rc = TreeStore( Ve->ExtTree,   // Store variable.
                         Name,
                         strlen(Name),
                         &Value,
                         sizeof(Value))) != TREE_OK) {
      return VAR_STOREERROR;            // Tree store error.
   }   
   return VAR_OK;
}



//-----------------------------------------------------------------------------
// VarAssign() -- Assign a value to a variable...
//-----------------------------------------------------------------------------
int VarAssign(      void* Handle, char* Name, void*  Value)
{
   VARANCHOR*   Va;
   VARMODULE*   Vm;
   VARLOCAL*    Vl;
   int          FindCode;
   int          rc;
   void*        LocateValue;
   TREE*		ExtTree;  //extern tree where variable is found

   Va = (VARANCHOR*) Handle;

   if ((Vm = Va->Module) == NULL)
      return VAR_NOMODULELEVEL;
         
   
   FindCode = LocateVariable( Handle, Name, &LocateValue, &ExtTree);

#ifdef DEBUG
   printf("VarAssign: %s -- %p Locate code: %d -- New Value: %p\n",
          Name,
          LocateValue,
          FindCode,
          Value);
#endif

   switch (FindCode) {
      case LOCATE_LOCAL:                      // Local variable with same name.
		  if ((Vl = Vm->Local) == NULL)
			return VAR_NOLOCALLEVEL;   
		  if ( LocateValue != NULL && *((DATA**)LocateValue) != NULL)
            DataFree( *((DATA**)LocateValue));// Free previous local value.
         if (( rc = TreeStore( Vl->VarTree,   // Store variable.
                               Name,
                               strlen(Name),
                               &Value,
                               sizeof(Value))) != TREE_OK) {
            return VAR_STOREERROR;            // Tree store error.
         }
         break;

      case LOCATE_NOTFOUND:
         if (Vm->VarTree == NULL) {
            if (( rc = TreeCreate(5, 0, &Vm->VarTree)) != TREE_OK)
               return VAR_TREECREATE;
         }     // Fall thru to module level variable store...

      case LOCATE_MODULE:
         if (FindCode == LOCATE_MODULE)       // If Module level variable...
            if ( LocateValue != NULL && *((DATA**)LocateValue) != NULL)
               DataFree( *((DATA**)LocateValue));// Free previous value.
         if (( rc = TreeStore( Vm->VarTree,   // Store variable.
                               Name,
                               strlen(Name),
                               &Value,
                               sizeof(Value))) != TREE_OK) {
            return VAR_STOREERROR;            // Tree store error.
         }
         break;

      case LOCATE_EXTERNAL:
         if ( LocateValue != NULL && *((DATA**)LocateValue) != NULL)
            DataFree(*((DATA**)LocateValue));        // Free previous value.
         if (( rc = TreeStore( ExtTree,   // Store global variable.
                               Name,
                               strlen(Name),
                               &Value,
                               sizeof(Value))) != TREE_OK) {
            return VAR_STOREERROR;            // Tree store error.
         }
         break;

      case LOCATE_SYSTEM:              // Not yet implemented.
         break;
   }
   return VAR_OK;
}



//-----------------------------------------------------------------------------
// VarFind() -- Return value of a variable...
//-----------------------------------------------------------------------------
int VarFind(        void* Handle, char* Name, void** Value)
{
   int    FindCode;
   void** FoundValue;
   TREE* ExtTree;

   FindCode = LocateVariable( Handle, Name, &((void*)FoundValue), &ExtTree);

#ifdef DEBUG
   printf("VarFind: %s -- %p Locate code: %d\n", Name, *FoundValue, FindCode);
#endif

   if (FindCode == LOCATE_NOTFOUND)
      return VAR_NOTFOUND;
   //if (FindCode == SYSTEM)
   //

   *Value = *FoundValue;
   return VAR_OK;
}

//-----------------------------------------------------------------------------
// VarExternEnter() -- Initialize a new extern level variable name space...
//-----------------------------------------------------------------------------
int VarExternEnter( void* Handle )
{
   VARANCHOR*   Va;
   VAREXTERN*   Ve;

   Va = (VARANCHOR*) Handle;

   Ve = Va->VarExt;                    // Save previous extern level.

   if ((Va->VarExt = GMEM(sizeof(VAREXTERN))) == NULL)
      return VAR_NOMEMORY;

   Va->VarExt->Next = Ve;              // Chain previous extern level.

   return VAR_OK;
}



//-----------------------------------------------------------------------------
// VarExternExit() -- Free up level of extern level variable name space...
//-----------------------------------------------------------------------------
int VarExternExit(  void* Handle )
{
   VARANCHOR*   Va;
   VAREXTERN*   Ve;

   Va = (VARANCHOR*) Handle;

   if ((Ve = Va->VarExt) == NULL)      // Get current module level.
      return VAR_NOEXTERNLEVEL;
   
   Va->VarExt = Ve->Next;              // Unchain from previous module levels.
   if (Ve->ExtTree) {                  // If there's a module level var tree...
      VarFreeTreeVars(Ve->ExtTree);    // Free all variable values in var tree.
      TreeDestroy(Ve->ExtTree);        // Kill module level variable tree.
   }
   FMEM(Ve);                           // Free module level structure.

   return VAR_OK;
}


//-----------------------------------------------------------------------------
// VarModuleEnter() -- Initialize a new module level variable name space...
//-----------------------------------------------------------------------------
int VarModuleEnter( void* Handle )
{
   VARANCHOR*   Va;
   VARMODULE*   Vm;

   Va = (VARANCHOR*) Handle;

   Vm = Va->Module;                    // Save previous module level.

   if ((Va->Module = GMEM(sizeof(VARMODULE))) == NULL)
      return VAR_NOMEMORY;

   Va->Module->Next = Vm;              // Chain previous module level.

   return VAR_OK;
}



//-----------------------------------------------------------------------------
// VarModuleExit() -- Free up level of module level variable name space...
//-----------------------------------------------------------------------------
int VarModuleExit(  void* Handle )
{
   VARANCHOR*   Va;
   VARMODULE*   Vm;

   Va = (VARANCHOR*) Handle;

   if ((Vm = Va->Module) == NULL)      // Get current module level.
      return VAR_NOMODULELEVEL;

   while ((Vm->Local) != NULL)         // Kill all local levels.
      VarLocalExit(Handle);

   Va->Module = Vm->Next;              // Unchain from previous module levels.
   if (Vm->VarTree) {                  // If there's a module level var tree...
      VarFreeTreeVars(Vm->VarTree);    // Free all variable values in var tree.
      TreeDestroy(Vm->VarTree);        // Kill module level variable tree.
   }
   FMEM(Vm);                           // Free module level structure.

   return VAR_OK;
}



//-----------------------------------------------------------------------------
// VarLocalEnter() -- Initialize another level of local variables...
//-----------------------------------------------------------------------------
int VarLocalEnter(  void* Handle )
{
   VARANCHOR*   Va;
   VARMODULE*   Vm;
   VARLOCAL*    Vl;

   Va = (VARANCHOR*) Handle;

   if ((Vm = Va->Module) == NULL)
      return VAR_NOMODULELEVEL;

   Vl = Vm->Local;                                    // Save current level.
   if ((Vm->Local = GMEM(sizeof(VARLOCAL))) == NULL)  // Get new level.
      return VAR_NOMEMORY;                            // Memory error.
   Vm->Local->Next = Vl;               // Chain previous level to new level.

   return VAR_OK;
}



//-----------------------------------------------------------------------------
// VarLocalExit() -- Free up local variables for this level...
//-----------------------------------------------------------------------------
int VarLocalExit(   void* Handle )
{
   VARANCHOR*   Va;
   VARMODULE*   Vm;
   VARLOCAL*    Vl;

   Va = (VARANCHOR*) Handle;

   if ((Vm = Va->Module) == NULL)
      return VAR_NOMODULELEVEL;
   if ((Vl = Vm->Local) == NULL)
      return VAR_NOLOCALLEVEL;

   if (Vl->VarTree) {                  // Is there a local var tree?
      VarFreeTreeVars(Vl->VarTree);    // Free all variable values in var tree.
      TreeDestroy(Vl->VarTree);        // Delete local variable tree.
   }
   Vm->Local = Vl->Next;               // Unchain any from previous level.   
   FMEM(Vl);                           // Free local level.
   return VAR_OK;
}



//-----------------------------------------------------------------------------
// Varlocal() -- Define a local variable...
//-----------------------------------------------------------------------------
int VarLocal(       void* Handle, char* Name )
{
   VARANCHOR*   Va;
   VARMODULE*   Vm;
   VARLOCAL*    Vl;
   int          rc;

   Va = (VARANCHOR*) Handle;

   if ((Vm = Va->Module) == NULL)
      return VAR_NOMODULELEVEL;
   if ((Vl = Vm->Local) == NULL)
      return VAR_NOLOCALLEVEL;

   //-----------------------------------------------------------------------
   // If there's no local variable tree, then create one...
   //-----------------------------------------------------------------------
   if (Vl->VarTree == NULL) {

      if (( rc = TreeCreate(5, 0, &Vl->VarTree)) != TREE_OK)
         return VAR_TREECREATE;
   }

   //-----------------------------------------------------------------------
   // Now, add variable to local tree...
   //-----------------------------------------------------------------------

   if (( rc = TreeStore( Vl->VarTree,
                         Name,
                         strlen(Name),
                         &NullPtr,
                         0)) != TREE_OK) {
      return VAR_STOREERROR;
   }
   return VAR_OK;

}



//-----------------------------------------------------------------------------
// LocateVariable() -- Find variable in one of the name spaces...
//-----------------------------------------------------------------------------

int LocateVariable( void* Handle, char* Name, void** Value, TREE** ExtTree)
{
   VARANCHOR*   Va;
   VARMODULE*   Vm;
   VARLOCAL*    Vl;
   VAREXTERN*   Ve;
   unsigned int Length;
   int          rc = TREE_KEYNOTFOUND;

   Va = (VARANCHOR*) Handle;

   if ((Vm = Va->Module) != NULL) {

      if ((Vl = Vm->Local) != NULL) {

         //--------------------------------------------------------------------
         // If there's a local variable tree, then check if variable is there.
         //--------------------------------------------------------------------
         if (Vl->VarTree != NULL) {

            if (( rc = TreeLocate( Vl->VarTree,
                                   Name,
                                   strlen(Name),
                                   Value,
                                   &Length)) == TREE_OK) {
               return LOCATE_LOCAL;
            }
         }
      }
   }	  

   //-----------------------------------------------------------------------
   // If there's an external variable tree, then check if variable is there.
   //-----------------------------------------------------------------------
   if ((Ve = Va->VarExt) != NULL) {       
      while (Ve != NULL) {	     	  
         if (( rc = TreeLocate( Ve->ExtTree,
                             Name,
                             strlen(Name),
                             Value,
                             &Length)) == TREE_OK) {
		    ExtTree = &Ve->ExtTree;
            return LOCATE_EXTERNAL;
		 }
		 Ve = Ve->Next;
	  }
   }

   //-----------------------------------------------------------------------
   // If there's a module level tree, then check if variable is there.
   //-----------------------------------------------------------------------
   if ((Vm = Va->Module) != NULL) {
      if (Vm->VarTree != NULL) {

         if (( rc = TreeLocate( Vm->VarTree,
                                Name,
                                strlen(Name),
                                Value,
                                &Length)) == TREE_OK) {

            return LOCATE_MODULE;
		 }
	  }   
   }
   return LOCATE_NOTFOUND;
}


//-----------------------------------------------------------------------------
// FreeTreeVar() -- Called to free a Variable value in the tree...
//-----------------------------------------------------------------------------

void   VarFreeTreeVars(TREE* Tree )
{
   TreeTraverse(Tree, (TRAVFUNC) SortVars, (void*) NULL);
}



//-----------------------------------------------------------------------------
// SortVars() -- Called by tree services with each entry in tree...
//-----------------------------------------------------------------------------

static Bool SortVars(
   void           *KeyPtr,
   unsigned short  KeyLen,
   void           *DataPtr,
   unsigned short  DataLen,
   void           *Parm)
{
   DATA**          Data1;

   Data1  = (DATA**) DataPtr;
#ifdef DEBUG
   printf("Freeing variable: %s\n", KeyPtr);
#endif
   if (*Data1 != NULL)
      DataFree( *Data1 );              // Free data value.
   return True;
}

DATA * InitString(char *Variable, DATA *Result) {
	int Length = strlen(Variable);
	
	/*--------------------------------------------------------------------------
	// Get DATA string to read into...
	//--------------------------------------------------------------------------*/
	Result = (DATA*) GMEM(sizeof(DATA) +
                        (Length) );	
    Result->Flag1        = DATA_STRING;         /* Indicate it's a string.*/
    Result->StringLength = Length;  /* Copy length of string.*/      
	strcpy(Result->String, Variable);
	return Result;
}