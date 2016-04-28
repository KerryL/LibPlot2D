/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  managedList.h
// Created:  1/29/2009
// Author:  K. Loux
// Description:  This is a template class for lists that handle automatic deletion
//				 of the items in the list.
// History:
//	11/7/2011	- Corrected camelCase, K. Loux.
//	8/10/2012	- Removed dependence on ObjectList in favor of std::vector, K. Loux.

#ifndef _MANAGED_LIST_H_
#define _MANAGED_LIST_H_

// Standard C++ headers
#include <cstdlib>
#include <vector>
#include <assert.h>

// Disable warning C4505 for this file
// This warning occurs because some types (every T= is a different type) do not
// use all of the overloaded methods in this object
#ifdef __WXMSW__
#pragma warning (disable:4505)
#endif

template <class T>
class ManagedList
{
public:
	virtual ~ManagedList();

	// Private data accessors
	unsigned int Add(T *toAdd);
	virtual void Remove(const unsigned int &index);
	inline unsigned int GetCount(void) const { return list.size(); };

	// Removes all objects from the list
	virtual void Clear(void);

	// Re-organizes the data in the list
	void ReorderObjects(const std::vector<unsigned int> &order);

	T *operator [](const unsigned int &index) const;

	typename std::vector<T*>::iterator begin() { return list.begin(); }
	typename std::vector<T*>::const_iterator begin() const { return list.begin(); }

	typename std::vector<T*>::iterator end() { return list.end(); }
	typename std::vector<T*>::const_iterator end() const { return list.end(); }

	typename std::vector<T*>::reverse_iterator rbegin() { return list.rbegin(); }
	typename std::vector<T*>::const_reverse_iterator rbegin() const { return list.rbegin(); }

	typename std::vector<T*>::reverse_iterator rend() { return list.rend(); }
	typename std::vector<T*>::const_reverse_iterator rend() const { return list.rend(); }

private:
	std::vector<T*> list;
};

//==========================================================================
// Class:			ManagedList
// Function:		~ManagedList
//
// Description:		Destructor for the ManagedList class.
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
ManagedList<T>::~ManagedList()
{
	Clear();
}

//==========================================================================
// Class:			ManagedList
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
//		int specifying the index of the newly added item
//
//==========================================================================
template <class T>
unsigned int ManagedList<T>::Add(T *toAdd)
{
	list.push_back(toAdd);
	return list.size() - 1;
}

//==========================================================================
// Class:			ManagedList
// Function:		Remove
//
// Description:		Removes the object at the specified index from the list
//
// Input Arguments:
//		index	= const unsigned int& specifying the object to remove
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
template <class T>
void ManagedList<T>::Remove(const unsigned int &index)
{
	assert(index < list.size());

	delete list[index];
	list.erase(list.begin() + index);
}

//==========================================================================
// Class:			ManagedList
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
T *ManagedList<T>::operator[](const unsigned int &index) const
{
	// Make sure the index is valid
	assert(index < list.size());

	return list[index];
}

//==========================================================================
// Class:			ManagedList
// Function:		ReorderObjects
//
// Description:		Re-organizes all of the objects in the list according to
//					the specified order.
//
// Input Arguments:
//		order	= conststd::vector<unsigned int>& specifying the new
//				  order.  If the list were {3, 2, 1}, then the three element
//				  list would be reversed.  If it were {2, 1, 3} then only the
//				  first two elements would be swapped.
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
template <class T>
void ManagedList<T>::ReorderObjects(const std::vector<unsigned int> &order)
{
	assert(order.size() == list.size());

	std::vector<T*> swap = list;

	unsigned int i;
	for (i = 0; i < list.size(); i++)
		list[i] = swap[order[i]];
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
	unsigned int i;
	for (i = 0; i < list.size(); i++)
		delete list[i];

	list.clear();
}

#endif// _MANAGED_LIST_H_
