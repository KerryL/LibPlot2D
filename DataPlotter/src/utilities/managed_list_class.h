/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  managed_list_class.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  This is a template class for linked lists that handle automatic deletion
//				 of the items in the list.
// History:

#ifndef _MANAGED_LIST_CLASS_H_
#define _MANAGED_LIST_CLASS_H_

// Local headers
#include "object_list_class.h"

// Standard C++ headers
#include <cstdlib>

// Disable warning C4505 for this file
// This warning occurs because some types (every T= is a different type) do not
// use both of the overloaded methods in this object
#ifdef WIN32
#pragma warning (disable:4505)
#endif

template <class T>
class MANAGED_LIST : public OBJECT_LIST<T>
{
public:
	// Overloaded methods
	void Remove(const int &Index);
	void Clear(void);
};

//==========================================================================
// Class:			MANAGED_LIST
// Function:		Remove
//
// Description:		Removes the object at the specified index from the list.
//
// Input Argurments:
//		Index	= const int& specifying the object to remove
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
template <class T>
void MANAGED_LIST<T>::Remove(const int &Index)
{
	// Check to see if we have more than one object in the list
	if (OBJECT_LIST<T>::Count == 1)
	{
		delete OBJECT_LIST<T>::ObjectList[0];
		OBJECT_LIST<T>::ObjectList[0] = NULL;

		// Delete the list entry for the object
		delete [] OBJECT_LIST<T>::ObjectList;
		OBJECT_LIST<T>::ObjectList = NULL;
	}
	else
	{
		// Store the array in a temporary list
		T **TempList = new T *[OBJECT_LIST<T>::Count];
		int i;
		for (i = 0; i < OBJECT_LIST<T>::Count; i++)
			TempList[i] = OBJECT_LIST<T>::ObjectList[i];

		// Re-dimension the list to be one pointer smaller
		delete [] OBJECT_LIST<T>::ObjectList;
		OBJECT_LIST<T>::ObjectList = new T *[OBJECT_LIST<T>::Count - 1];

		// For all of the pointers below the one we want to remove, it's a direct copy
		// from the temporary list.
		for (i = 0; i < Index; i++)
			OBJECT_LIST<T>::ObjectList[i] = TempList[i];

		// Delete the object with the specified index
		delete TempList[Index];
		TempList[Index] = NULL;

		// For the rest of the objects, we have to re-number them as we add them
		for (i = Index; i < OBJECT_LIST<T>::Count - 1; i++)
			// Add the object from the temporary list to our "permanent" list
			OBJECT_LIST<T>::ObjectList[i] = TempList[i + 1];

		delete [] TempList;
		TempList = NULL;
	}

	// Decrement the number of objects in the list
	OBJECT_LIST<T>::Count--;

	return;
}

//==========================================================================
// Class:			MANAGED_LIST
// Function:		Clear
//
// Description:		Removes all items in the list.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
template <class T>
void MANAGED_LIST<T>::Clear(void)
{
	// Delete all of the items in the list
	int i;
	for (i = 0; i < OBJECT_LIST<T>::Count; i++)
	{
		delete OBJECT_LIST<T>::ObjectList[i];
		OBJECT_LIST<T>::ObjectList[i] = NULL;
	}

	// Delete the list and set the count to zero
	delete [] OBJECT_LIST<T>::ObjectList;
	OBJECT_LIST<T>::ObjectList = NULL;
	OBJECT_LIST<T>::Count = 0;

	return;
}

#endif// _MANAGED_LIST_CLASS_H_