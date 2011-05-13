/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  expression_tree_class.h
// Created:  5/6/2011
// Author:  K. Loux
// Description:  Handles user-specified mathematical operations on datasets.
// History:

#ifndef _EXPRESSION_TREE_CLASS_H_
#define _EXPRESSION_TREE_CLASS_H_

// Local headers
#include "utilities/managed_list_class.h"
#include "utilities/dataset2D.h"

// wxWidgets forward declarations
class wxString;

class ExpressionTree
{
public:
	// Constructor
	ExpressionTree(const MANAGED_LIST<const Dataset2D> &_list);

	// Main solver method
	wxString Solve(wxString expression, Dataset2D &solvedData);

private:
	const MANAGED_LIST<const Dataset2D> &list;

	struct Node
	{
		//wxString value;
		Dataset2D set;
		double dValue;
		//int child1, child2;
	};

	wxString Parenthesize(wxString expression, wxString &errorString) const;
	Node EvaluateNextNode(wxString &expression, wxString &errorString);
	Dataset2D GetSetFromList(const unsigned int &i);
};

#endif// _EXPRESSION_TREE_CLASS_H_