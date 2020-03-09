/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  managedList.h
// Date:  1/29/2009
// Auth:  K. Loux
// Desc:  This is a template class for lists that handle automatic deletion of
//        the items in the list.

#ifndef MANAGED_LIST_H_
#define MANAGED_LIST_H_

// Standard C++ headers
#include <cstdlib>
#include <vector>
#include <memory>
#include <cassert>

namespace LibPlot2D
{

// Disable warning C4505 for this file
// This warning occurs because some types (every T is a different type) do not
// use all of the overloaded methods in this object
#ifdef __WXMSW__
#pragma warning (disable:4505)
#endif

/// Wrapper around std::vector for storing std::unique_ptr objects.
template <class T>
class ManagedList
{
public:
	/// Adds the specified object to the list.
	///
	/// \param toAdd Object to add to the list.
	///
	/// \returns Index of the newly added item.
	unsigned int Add(std::unique_ptr<T> toAdd);

	/// Removes the object with the specified index from the list.
	///
	/// \param index Index of the object to remove.
	void Remove(const unsigned int &index);

	/// Gets the number of objects in the list.
	/// \returns The number of objects in the list.
	inline typename std::vector<std::unique_ptr<T>>::size_type GetCount() const { return mList.size(); }

	/// Removes all objects from the list.
	void Clear();

	/// Re-organizes the data in the list according to the specified order.
	///
	/// \param order Vector specifying the new order of the elements.  The
	///              values specified in this vector are the current indices of
	///              the elements, but listed in a new order.
	void ReorderObjects(const std::vector<unsigned int> &order);

	/// Element access operator.
	///
	/// \param index Index of the element to access.
	///
	/// \returns Reference to the desired element.
	const std::unique_ptr<T>& operator[](const unsigned int &index) const;

	/// @{
	
	/// Gets the last element in the vector.
	/// \returns Reference to the last element.
	const std::unique_ptr<T>& Back() const { return mList.back(); }

	/// Gets the last element in the vector.
	/// \returns Reference to the last element.
	std::unique_ptr<T>& Back() { return mList.back(); }

	/// @}

	/// \name Iterator accessors
	/// @{

	typename std::vector<std::unique_ptr<T>>::iterator begin() { return mList.begin(); }
	typename std::vector<std::unique_ptr<T>>::const_iterator begin() const { return mList.begin(); }

	typename std::vector<std::unique_ptr<T>>::iterator end() { return mList.end(); }
	typename std::vector<std::unique_ptr<T>>::const_iterator end() const { return mList.end(); }

	typename std::vector<std::unique_ptr<T>>::reverse_iterator rbegin() { return mList.rbegin(); }
	typename std::vector<std::unique_ptr<T>>::const_reverse_iterator rbegin() const { return mList.rbegin(); }

	typename std::vector<std::unique_ptr<T>>::reverse_iterator rend() { return mList.rend(); }
	typename std::vector<std::unique_ptr<T>>::const_reverse_iterator rend() const { return mList.rend(); }

	/// @}

private:
	std::vector<std::unique_ptr<T>> mList;
};

//=============================================================================
// Class:			ManagedList
// Function:		Add
//
// Description:		Adds objects to the list.  Performs the necessary memory
//					allocating and transferring routines.
//
// Input Arguments:
//		toAdd	= std::unique_ptr<T>, pointing to the object to add
//
// Output Arguments:
//		None
//
// Return Value:
//		int specifying the index of the newly added item
//
//=============================================================================
template <class T>
unsigned int ManagedList<T>::Add(std::unique_ptr<T> toAdd)
{
	mList.push_back(std::move(toAdd));
	return mList.size() - 1;
}

//=============================================================================
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
//=============================================================================
template <class T>
void ManagedList<T>::Remove(const unsigned int &index)
{
	assert(index < mList.size());
	mList.erase(mList.begin() + index);
}

//=============================================================================
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
//		const std::unique_ptr<T>& pointing to the object at the specified index
//
//=============================================================================
template <class T>
const std::unique_ptr<T>& ManagedList<T>::operator[](const unsigned int &index) const
{
	// Make sure the index is valid
	assert(index < mList.size());

	return mList[index];
}

//=============================================================================
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
//=============================================================================
template <class T>
void ManagedList<T>::ReorderObjects(const std::vector<unsigned int> &order)
{
	assert(order.size() == mList.size());

	std::vector<T*> swap = mList;

	unsigned int i;
	for (i = 0; i < mList.size(); ++i)
		mList[i] = swap[order[i]];
}

//=============================================================================
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
//=============================================================================
template <class T>
void ManagedList<T>::Clear()
{
	mList.clear();
}

}// namespace LibPlot2D

#endif// MANAGED_LIST_H_
