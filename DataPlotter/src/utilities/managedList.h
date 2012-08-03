/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  managedList.h
// Created:  1/29/2009
// Author:  K. Loux
// Description:  This is a template class for linked lists that handle automatic deletion
//				 of the items in the list.
// History:
//	11/7/2011	- Corrected camelCase, K. Loux.

#ifndef _MANAGED_LIST_H_
#define _MANAGED_LIST_H_

// Local headers
#include "objectList.h"

// Standard C++ headers
#include <cstdlib>

// Disable warning C4505 for this file
// This warning occurs because some types (every T= is a different type) do not
// use all of the overloaded methods in this object
#ifdef __WXMSW__
#pragma warning (disable:4505)
#endif

template <class T>
class ManagedList : public ObjectList<T>
{
public:
	// Overloaded methods
	void Remove(const int &Index);
	void Clear(void);
};

//==========================================================================
// Class:			ManagedList
// Function:		Remove
//
// Description:		Removes the object at the specified index from the list.
//
// Input Arguments:
//		index	= const int& specifying the object to remove
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
template <class T>
void ManagedList<T>::Remove(const int &index)
{
	// Check to see if we have more than one object in the list
	if (ObjectList<T>::count == 1)
	{
		delete ObjectList<T>::objectList[0];
		ObjectList<T>::objectList[0] = NULL;

		// Delete the list entry for the object
		delete [] ObjectList<T>::objectList;
		ObjectList<T>::objectList = NULL;
	}
	else
	{
		// Store the array in a temporary list
		T **tempList = new T *[ObjectList<T>::count];
		int i;
		for (i = 0; i < ObjectList<T>::count; i++)
			tempList[i] = ObjectList<T>::objectList[i];

		// Re-dimension the list to be one pointer smaller
		delete [] ObjectList<T>::objectList;
		ObjectList<T>::objectList = new T *[ObjectList<T>::count - 1];

		// For all of the pointers below the one we want to remove, it's a direct copy
		// from the temporary list.
		for (i = 0; i < index; i++)
			ObjectList<T>::objectList[i] = tempList[i];

		// Delete the object with the specified index
		delete tempList[index];
		tempList[index] = NULL;

		// For the rest of the objects, we have to re-number them as we add them
		for (i = index; i < ObjectList<T>::count - 1; i++)
			// Add the object from the temporary list to our "permanent" list
			ObjectList<T>::objectList[i] = tempList[i + 1];

		delete [] tempList;
		tempList = NULL;
	}

	// Decrement the number of objects in the list
	ObjectList<T>::count--;
}

//==========================================================================
// Class:			ManagedList
// Function:		Clear
//
// Description:		Removes all items in the list.
//
// Input Arguments:
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
void ManagedList<T>::Clear(void)
{
	// Delete all of the items in the list
	int i;
	for (i = 0; i < ObjectList<T>::count; i++)
	{
		delete ObjectList<T>::objectList[i];
		ObjectList<T>::objectList[i] = NULL;
	}

	// Delete the list and set the count to zero
	delete [] ObjectList<T>::objectList;
	ObjectList<T>::objectList = NULL;
	ObjectList<T>::count = 0;
}

#endif// _MANAGED_LIST_H_