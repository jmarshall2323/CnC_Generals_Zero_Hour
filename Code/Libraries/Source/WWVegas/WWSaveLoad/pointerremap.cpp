/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : WWSaveLoad                                                   *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/wwsaveload/pointerremap.cpp                  $*
 *                                                                                             *
 *                       Author:: Greg Hjelstrom                                               *
 *                                                                                             *
 *                     $Modtime:: 5/09/01 11:36a                                              $*
 *                                                                                             *
 *                    $Revision:: 9                                                           $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


#include "pointerremap.h"
#include "refcount.h"
#include "wwdebug.h"


const int POINTER_TABLES_GROWTH_STEP = 4096;


PointerRemapClass::PointerRemapClass(void)
{
	PointerPairTable.reserve(POINTER_TABLES_GROWTH_STEP);
	PointerRequestTable.reserve(POINTER_TABLES_GROWTH_STEP);
	RefCountRequestTable.reserve(POINTER_TABLES_GROWTH_STEP);
}

PointerRemapClass::~PointerRemapClass(void)
{
}

void PointerRemapClass::Reset(void)
{
	PointerPairTable.clear();
	PointerRequestTable.clear();
	RefCountRequestTable.clear();
}

void PointerRemapClass::Process(void)
{
	if ( PointerPairTable.size() > 0 ) {
		qsort(&PointerPairTable[0], PointerPairTable.size(), sizeof(PointerPairTable[0]), ptr_pair_compare_function);
	}

	if ( PointerRequestTable.size() > 0 ) {
		WWASSERT( PointerPairTable.size() > 0 );
		qsort(&PointerRequestTable[0],PointerRequestTable.size(), sizeof(PointerRequestTable[0]), ptr_request_compare_function);
		Process_Request_Table(PointerRequestTable,false);
	}

	// remap the ref-counted pointers
	if ( RefCountRequestTable.size() > 0 ) {
		WWASSERT( PointerPairTable.size() > 0 );
		qsort(&RefCountRequestTable[0],RefCountRequestTable.size(), sizeof(RefCountRequestTable[0]), ptr_request_compare_function);
		Process_Request_Table(RefCountRequestTable,true);
	}
}

void PointerRemapClass::Process_Request_Table(std::vector<PtrRemapStruct>& request_table, bool refcount)
{
	// Remap the pointers
	int pair_index = 0;

	for (const auto& request : request_table) {

		const void* pointer_to_remap = *(request.PointerToRemap);
		const int pre_search_index = pair_index;

		// Find the pair which contains the pointer we are looking for as its "old" pointer
		while (	(pair_index < PointerPairTable.size()) &&
					(PointerPairTable[pair_index].OldPointer < pointer_to_remap)  ) 
		{
			pair_index++;
		}
	
		if ((pair_index < PointerPairTable.size()) && (PointerPairTable[pair_index].OldPointer == pointer_to_remap)) {

			// we found the match, plug in the new pointer and add a ref if needed.
			*request.PointerToRemap = PointerPairTable[pair_index].NewPointer;

			if (refcount) {
				RefCountClass * refptr = (RefCountClass *)(*request.PointerToRemap);
				refptr->Add_Ref();
			}

		} else {
			
			// Failed to re-map the pointer.
			// warn the user, set pointer to NULL, reset index to the pre_search_index.
			// If this happens, things could be going very wrong.  (find out why its happening!)
			pair_index = pre_search_index;
			*request.PointerToRemap = NULL;
#ifdef WWDEBUG			
			const char * file = request.File;
			int line = request.Line;
			WWDEBUG_SAY(("Warning! Failed to re-map pointer! old_ptr = 0x%X  file = %s  line = %d\r\n",(unsigned int)pointer_to_remap,file,line));
			WWASSERT( 0 );
#endif
		}
	}
}

void PointerRemapClass::Register_Pointer (void *old_pointer, void *new_pointer)
{
	PointerPairTable.push_back(PtrPairStruct(old_pointer,new_pointer));
}

#ifdef WWDEBUG
void PointerRemapClass::Request_Pointer_Remap(void **pointer_to_convert,const char * file,int line)
{
	PtrRemapStruct remap;
	remap.PointerToRemap = pointer_to_convert;
	remap.File = file;
	remap.Line = line;
	PointerRequestTable.push_back(remap);
}

void PointerRemapClass::Request_Ref_Counted_Pointer_Remap (RefCountClass **pointer_to_convert,const char * file, int line)
{
	PtrRemapStruct remap;
	remap.PointerToRemap = (void**)pointer_to_convert;
	remap.File = file;
	remap.Line = line;
	RefCountRequestTable.push_back(remap);
}

#else

void PointerRemapClass::Request_Pointer_Remap (void **pointer_to_convert)
{
	PtrRemapStruct remap;
	remap.PointerToRemap = pointer_to_convert;
	PointerRequestTable.push_back(remap);
}

void PointerRemapClass::Request_Ref_Counted_Pointer_Remap (RefCountClass **pointer_to_convert)
{
	PtrRemapStruct remap;
	remap.PointerToRemap = (void**)pointer_to_convert;
	RefCountRequestTable.push_back(remap);
}

#endif

/*
** sort compare function for pointer pair structures
** sorts by the old pointer value
*/
int __cdecl PointerRemapClass::ptr_pair_compare_function(void const * ptr1, void const * ptr2)
{
	void * old1 = ((PointerRemapClass::PtrPairStruct const *)ptr1)->OldPointer;
	void * old2 = ((PointerRemapClass::PtrPairStruct const *)ptr2)->OldPointer;

	if (old1 == old2) {
		return(0);
	}
	if (old1 < old2) {
		return(-1);
	}
	return(1);
}

/*
** sort compare function for pointer remap structures
** sorts by the old pointer value
*/
int __cdecl PointerRemapClass::ptr_request_compare_function(void const * ptr1, void const * ptr2)
{
	PtrRemapStruct * remap1 = (PtrRemapStruct *)ptr1;
	PtrRemapStruct * remap2 = (PtrRemapStruct *)ptr2;
	
	void * old1 = *(remap1->PointerToRemap);
	void * old2 = *(remap2->PointerToRemap);

	if (old1 == old2) {
		return(0);
	}
	if (old1 < old2) {
		return(-1);
	}
	return(1);
}


