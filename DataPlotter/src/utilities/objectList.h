/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  object_list_class.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  This is a template class for lists of objects that can be accessed by
//				 index, but the order is not important.
// History:

#ifndef _OBJECT_LIST_CLASS_H_
#define _OBJECT_LIST_CLASS_H_

// Standard C++ headers
#include <cstdlib>
#include <assert.h>

template <class T>
class OBJECT_LIST
{
public:
	// Constructor
	OBJECT_LIST();

	// Destructor
	virtual ~OBJECT_LIST();

	// Private data accessors
	int Add(T *ToAdd);
	virtual void Remove(const int &Index);
	inline int GetCount(void) const { return Count; };
	T *GetObject(const int &Index) const;

	// Removes all objects from the list
	virtual void Clear(void);

	// Re-organizes the data in the list
	void ReorderObjects(const int *NewOrder);

	// Operators
	T *operator [](const int &Index) const;

protected:
	// The number of objects in this list
	int Count;

	// The data in the list
	T **ObjectList;
};

//==========================================================================
// Class:			OBJECT_LIST
// Function:		OBJECT_LIST
//
// Description:		Constructor for the OBJECT_LIST class.
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
OBJECT_LIST<T>::OBJECT_LIST()
{
	// Initialize the count to zero and the pointers to NULL
	Count = 0;
	ObjectList = NULL;
}

//==========================================================================
// Class:			OBJECT_LIST
// Function:		~OBJECT_LIST
//
// Description:		Destructor for the OBJECT_LIST class.
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
OBJECT_LIST<T>::~OBJECT_LIST()
{
	// Remove all of the items in this list
	Clear();
}

//==========================================================================
// Class:			OBJECT_LIST
// Function:		Add
//
// Description:		Adds objects to the list.  Performs the necessary memory
//					allocating and transferring routines.
//
// Input Arguments:
//		ToAdd	= const T*, pointing to the object to add
//
// Output Arguments:
//		None
//
// Return Value:
//		integer specifying the index of the newly added item
//
//==========================================================================
template <class T>
int OBJECT_LIST<T>::Add(T *ToAdd)
{
	// Check to see if we already have an object open
	if (Count == 0)
		// We don't have an object open, so let's create a pointer to an object
		ObjectList = new T*[1];
	else
	{
		// We do have (at least) an object open, so we'll have to store the existing objects in a temporary
		// list while we re-dimension the real list.
		T **TempList = new T*[Count];
		int i;
		for (i = 0; i < Count; i++)
			TempList[i] = ObjectList[i];

		// Re-dimension ObjectList
		delete [] ObjectList;
		ObjectList = new T*[Count + 1];

		// Re-assign the first Count items in ObjectList to the original pointers
		for (i = 0; i < Count; i++)
			ObjectList[i] = TempList[i];

		delete [] TempList;
		TempList = NULL;
	}

	// Add the new address to the Count'th element of ObjectList
	ObjectList[Count] = ToAdd;

	// Increment the number of objects in the list
	Count++;

	// Return Count - 1 so we can use an indexing system that starts at zero
	return Count - 1;
}

//==========================================================================
// Class:			OBJECT_LIST
// Function:		Remove
//
// Description:		Removes the object at the specified index from the list
//
// Input Arguments:
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
void OBJECT_LIST<T>::Remove(const int &Index)
{
	// Check to see if we have more than one object in the list
	if (Count == 1)
	{
		// Delete the list entry for the object
		delete [] ObjectList;
		ObjectList = NULL;
	}
	else
	{
		// Store the array in a temporary list
		T **TempList = new T*[Count];
		int i;
		for (i = 0; i < Count; i++)
			TempList[i] = ObjectList[i];

		// Re-dimension the list to be one pointer smaller
		delete [] ObjectList;
		ObjectList = new T*[Count - 1];

		// For all of the pointers below the one we want to remove, it's a direct copy
		// from the temporary list.
		for (i = 0; i < Index; i++)
			ObjectList[i] = TempList[i];

		// For the rest of the objects, we have to re-number them as we add them
		for (i = Index; i < Count - 1; i++)
			// Add the object from the temporary list to our "permanent" list
			ObjectList[i] = TempList[i + 1];

		delete [] TempList;
		TempList = NULL;
	}

	// Decrement the number of objects in the list
	Count--;

	return;
}

//==========================================================================
// Class:			OBJECT_LIST
// Function:		GetObject
//
// Description:		Returns a pointer to the object with the specified index.
//
// Input Arguments:
//		Index	= const int& specifying which object we want to retrieve
//
// Output Arguments:
//		None
//
// Return Value:
//		T* pointing to the object at the specified index
//
//==========================================================================
template <class T>
T *OBJECT_LIST<T>::GetObject(const int &Index) const
{
	// Make sure the index is valid
	assert(Index >= 0 && Index < Count);

	return ObjectList[Index];
}

//==========================================================================
// Class:			OBJECT_LIST
// Function:		operator[]
//
// Description:		Returns a pointer to the object with the specified index.
//
// Input Arguments:
//		Index	= const int& specifying which object we want to retrieve
//
// Output Arguments:
//		None
//
// Return Value:
//		T* pointing to the object at the specified index
//
//==========================================================================
template <class T>
T *OBJECT_LIST<T>::operator[](const int &Index) const
{
	// Make sure the index is valid
	assert(Index >= 0 && Index < Count);

	return ObjectList[Index];
}

//==========================================================================
// Class:			OBJECT_LIST
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
void OBJECT_LIST<T>::Clear(void)
{
	// Delete the list and set the count to zero
	delete [] ObjectList;
	ObjectList = NULL;
	Count = 0;

	return;
}

//==========================================================================
// Class:			OBJECT_LIST
// Function:		ReorderObjects
//
// Description:		Re-organizes all of the objects in the list according to
//					the specified order.
//
// Input Arguments:
//		NewOrder	= const int* pointing to an array (MUST contain exactly
//					  the number of elements of this list) specifying the new
//					  order.  If the list were {3, 2, 1}, then the three element
//					  list would be reversed.  If it were {2, 1, 3} then only the
//					  first two elements would be swapped.
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
template <class T>
void OBJECT_LIST<T>::ReorderObjects(const int *NewOrder)
{
	// Create a duplicate list
	T **SwapList = new T*[Count];
	int i;
	for (i = 0; i < Count; i++)
		SwapList[i] = ObjectList[i];

	// Perform the swap
	for (i = 0; i < Count; i++)
		ObjectList[i] = SwapList[NewOrder[i]];

	// Clean up memory
	delete [] SwapList;
	SwapList = NULL;

	return;
}

#endif// _OBJECT_LIST_CLASS_H_