/*===================================================================================
                                    DataPlotter
                         Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  objectList.h
// Created:  1/20/2009
// Author:  K. Loux
// Description:  This is a template class for lists of objects that can be accessed by
//				 index, but the order is not important.
// History:
//	11/7/2011	- Corrected camelCase, K. Loux.

#ifndef _OBJECT_LIST_H_
#define _OBJECT_LIST_H_

// Standard C++ headers
#include <cstdlib>
#include <assert.h>

template <class T>
class ObjectList
{
public:
	// Constructor
	ObjectList();

	// Destructor
	virtual ~ObjectList();

	// Private data accessors
	int Add(T *toAdd);
	virtual void Remove(const int &index);
	inline int GetCount(void) const { return count; };
	T *GetObject(const int &index) const;

	// Removes all objects from the list
	virtual void Clear(void);

	// Re-organizes the data in the list
	void ReorderObjects(const int *newOrder);

	// Operators
	T *operator [](const int &index) const;

protected:
	// The number of objects in this list
	int count;

	// The data in the list
	T **objectList;
};

//==========================================================================
// Class:			ObjectList
// Function:		ObjectList
//
// Description:		Constructor for the ObjectList class.
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
ObjectList<T>::ObjectList()
{
	// Initialize the count to zero and the pointers to NULL
	count = 0;
	objectList = NULL;
}

//==========================================================================
// Class:			ObjectList
// Function:		~ObjectList
//
// Description:		Destructor for the ObjectList class.
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
ObjectList<T>::~ObjectList()
{
	// Remove all of the items in this list
	Clear();
}

//==========================================================================
// Class:			ObjectList
// Function:		Add
//
// Description:		Adds objects to the list.  Performs the necessary memory
//					allocating and transferring routines.
//
// Input Arguments:
//		toAdd	= const T*, pointing to the object to add
//
// Output Arguments:
//		None
//
// Return Value:
//		integer specifying the index of the newly added item
//
//==========================================================================
template <class T>
int ObjectList<T>::Add(T *toAdd)
{
	// Check to see if we already have an object open
	if (count == 0)
		// We don't have an object open, so let's create a pointer to an object
		objectList = new T*[1];
	else
	{
		// We do have (at least) an object open, so we'll have to store the existing objects in a temporary
		// list while we re-dimension the real list.
		T **tempList = new T*[count];
		int i;
		for (i = 0; i < count; i++)
			tempList[i] = objectList[i];

		// Re-dimension ObjectList
		delete [] objectList;
		objectList = new T*[count + 1];

		// Re-assign the first Count items in ObjectList to the original pointers
		for (i = 0; i < count; i++)
			objectList[i] = tempList[i];

		delete [] tempList;
		tempList = NULL;
	}

	// Add the new address to the Count'th element of ObjectList
	objectList[count] = toAdd;

	// Increment the number of objects in the list
	count++;

	// Return Count - 1 so we can use an indexing system that starts at zero
	return count - 1;
}

//==========================================================================
// Class:			ObjectList
// Function:		Remove
//
// Description:		Removes the object at the specified index from the list
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
void ObjectList<T>::Remove(const int &index)
{
	// Check to see if we have more than one object in the list
	if (count == 1)
	{
		// Delete the list entry for the object
		delete [] objectList;
		objectList = NULL;
	}
	else
	{
		// Store the array in a temporary list
		T **tempList = new T*[count];
		int i;
		for (i = 0; i < count; i++)
			tempList[i] = objectList[i];

		// Re-dimension the list to be one pointer smaller
		delete [] objectList;
		objectList = new T*[count - 1];

		// For all of the pointers below the one we want to remove, it's a direct copy
		// from the temporary list.
		for (i = 0; i < index; i++)
			objectList[i] = tempList[i];

		// For the rest of the objects, we have to re-number them as we add them
		for (i = index; i < count - 1; i++)
			// Add the object from the temporary list to our "permanent" list
			objectList[i] = tempList[i + 1];

		delete [] tempList;
		tempList = NULL;
	}

	// Decrement the number of objects in the list
	count--;
}

//==========================================================================
// Class:			ObjectList
// Function:		GetObject
//
// Description:		Returns a pointer to the object with the specified index.
//
// Input Arguments:
//		index	= const int& specifying which object we want to retrieve
//
// Output Arguments:
//		None
//
// Return Value:
//		T* pointing to the object at the specified index
//
//==========================================================================
template <class T>
T *ObjectList<T>::GetObject(const int &index) const
{
	// Make sure the index is valid
	assert(index >= 0 && index < count);

	return objectList[index];
}

//==========================================================================
// Class:			ObjectList
// Function:		operator[]
//
// Description:		Returns a pointer to the object with the specified index.
//
// Input Arguments:
//		index	= const int& specifying which object we want to retrieve
//
// Output Arguments:
//		None
//
// Return Value:
//		T* pointing to the object at the specified index
//
//==========================================================================
template <class T>
T *ObjectList<T>::operator[](const int &index) const
{
	// Make sure the index is valid
	assert(index >= 0 && index < count);

	return objectList[index];
}

//==========================================================================
// Class:			ObjectList
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
void ObjectList<T>::Clear(void)
{
	// Delete the list and set the count to zero
	delete [] objectList;
	objectList = NULL;
	count = 0;
}

//==========================================================================
// Class:			ObjectList
// Function:		ReorderObjects
//
// Description:		Re-organizes all of the objects in the list according to
//					the specified order.
//
// Input Arguments:
//		newOrder	= const int* pointing to an array (MUST contain exactly
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
void ObjectList<T>::ReorderObjects(const int *newOrder)
{
	// Create a duplicate list
	T **swapList = new T*[count];
	int i;
	for (i = 0; i < count; i++)
		swapList[i] = objectList[i];

	// Perform the swap
	for (i = 0; i < count; i++)
		objectList[i] = swapList[newOrder[i]];

	// Clean up memory
	delete [] swapList;
	swapList = NULL;
}

#endif// _OBJECT_LIST_H_