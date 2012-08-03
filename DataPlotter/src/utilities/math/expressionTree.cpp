/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  expressionTree.cpp
// Created:  5/6/2011
// Author:  K. Loux
// Description:  Handles user-specified mathematical operations on datasets.
// History:

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/math/expressionTree.h"
#include "utilities/signals/derivative.h"
#include "utilities/signals/integral.h"
#include "utilities/signals/fft.h"
#include "utilities/math/plotMath.h"

//==========================================================================
// Class:			ExpressionTree
// Function:		ExpressionTree
//
// Description:		Constructor for ExpressionTree class.
//
// Input Arguments:
//		_list		= const ManagedList<const Dataset2D>& reference to the
//					  other datasets which may be required to complete the calculation
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ExpressionTree::ExpressionTree(const ManagedList<const Dataset2D> &_list) : list(_list)
{
}

//==========================================================================
// Class:			ExpressionTree
// Function:		Solve
//
// Description:		Main solving method for the tree.
//
// Input Arguments:
//		expression		= wxString containing the expression to parse
//		_xAxisFactor	= const double& specifying the factor required to convert
//						  X-axis data into seconds (for FFT or filtering operations)
//
// Output Arguments:
//		solvedData		= Dataset2D& containing the evaluated data
//
// Return Value:
//		wxString, empty for success, error string if unsuccessful
//
//==========================================================================
wxString ExpressionTree::Solve(wxString expression, Dataset2D &solvedData, const double &_xAxisFactor)
{
	xAxisFactor = _xAxisFactor;
	wxString errorString;
	expression = Parenthesize(expression, errorString);

	if (!errorString.IsEmpty())
		return errorString;

	Node topNode = EvaluateNextNode(expression, errorString);
	solvedData = topNode.set;

	return errorString;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		Parenthesize
//
// Description:		Adds parentheses to the string so it is fully parenthesized,
//					and still the original order of operations holds.  Also
//					removes spaces from the string.
//
// Input Arguments:
//		expression	= wxString containing the expression as entered by the user
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing string with parentheses added
//
//==========================================================================
wxString ExpressionTree::Parenthesize(wxString expression, wxString &errorString) const
{
	// Without parentheses, everthing is evaluated left-to-right, so we need (as a minimum)
	// to add parentheses around multiplication and division

	// FIXME:  I think parenthesizing can fix the double operator bug (see next function down)
	// FIXME:  Also, need to add parentheses for things like int[1] -> should be int([1])

	// Check for a parentheses imbalance
	unsigned int leftCount(0), rightCount(0);
	int location = expression.find(_T("("));
	while (location != wxNOT_FOUND)
	{
		leftCount++;
		location = expression.find(_T("("), location + 1);
	}
	location = expression.find(_T(")"));
	while (location != wxNOT_FOUND)
	{
		rightCount++;
		location = expression.find(_T(")"), location + 1);
	}
	if (leftCount != rightCount)
	{
		errorString = _T("Imbalanced parentheses!");
		return expression;
	}

	// Parse the expression and look for *, / and % operators, then parse in both directions
	wxString targetOperator = _T("*");
	int nextMorD = expression.Find(targetOperator);
	int nextCloseParenthese, nextOpenParenthese, nextPlus, nextMinus, insertAt;

	while (nextMorD != wxNOT_FOUND)
	{
		// Find next previous important characters
		nextCloseParenthese = expression.find_last_of(_T(")"), nextMorD);
		nextOpenParenthese = expression.find_last_of(_T("("), nextMorD);
		nextPlus = expression.find_last_of(_T("+"), nextMorD);
		nextMinus = expression.find_last_of(_T("-"), nextMorD);

		// Handle cases differently, depending on what the first important character is
		if (nextCloseParenthese > nextPlus &&
			nextCloseParenthese > nextMinus &&
			nextCloseParenthese > nextOpenParenthese)
		{
			insertAt = expression.find_last_of(_T("("), nextCloseParenthese);
			if (insertAt == wxNOT_FOUND)
			{
				errorString = _T("Imbalanced parentheses!");
				return expression;
			}
		}
		else if (nextPlus > nextMinus &&
			nextPlus > nextOpenParenthese)
			insertAt = nextPlus;
		else if (nextMinus > nextOpenParenthese)
			insertAt = nextMinus;
		else
		{
			// Nothing was found (all are == wxNOT_FOUND) OR
			// The first character found was a "("
			// Take no further action this loop
			insertAt = -2;// -2 because -1 == wxNOT_FOUND
		}

		if (insertAt >= 0)
		{
			// Insert an open parenthese
			expression = expression.Mid(0, insertAt + 1) + _("(") + expression.Mid(insertAt + 1);

			// Find next previous important characters
			nextOpenParenthese = expression.find_first_of(_T("("), nextMorD);
			nextPlus = expression.find_first_of(_T("+"), nextMorD);
			nextMinus = expression.find_first_of(_T("-"), nextMorD);

			// Handle cases differently, depending on what the first important character is
			if ((nextOpenParenthese < nextPlus || nextPlus == wxNOT_FOUND) &&
				(nextOpenParenthese < nextMinus || nextMinus == wxNOT_FOUND) &&
				nextOpenParenthese != wxNOT_FOUND)
			{
				insertAt = expression.find_first_of(_T(")"), nextOpenParenthese);
				if (insertAt == wxNOT_FOUND)
				{
					errorString = _T("Imbalanced parentheses!");
					return expression;
				}
			}
			else if ((nextPlus < nextMinus || nextMinus == wxNOT_FOUND) &&
				nextPlus != wxNOT_FOUND)
				insertAt = nextPlus;
			else if (nextMinus != wxNOT_FOUND)
				insertAt = nextMinus;
			else
			{
				// Nothing was found (all are == wxNOT_FOUND)
				insertAt = expression.Len();
			}

			// Insert a close parenthese
			expression = expression.Mid(0, insertAt) + _(")") + expression.Mid(insertAt);
		}

		if (insertAt == -2)
			insertAt = nextMorD - 1;

		// Find the next multiplcation or division
		if (insertAt + 2 < (int)expression.Len())
			nextMorD = expression.find(targetOperator, insertAt + 2);
		else
			nextMorD = wxNOT_FOUND;

		// If we can't find any more multiplications, look for divisions
		if (nextMorD == wxNOT_FOUND && targetOperator.Cmp(_T("*")) == 0)
		{
			targetOperator = _T("/");
			nextMorD = expression.find(targetOperator);
		}
		// Can't find any more divisions, look for modulo
		else if (nextMorD == wxNOT_FOUND && targetOperator.Cmp(_T("/")) == 0)
		{
			targetOperator = _T("%");
			nextMorD = expression.find(targetOperator);
		}
	}

	return expression;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateNextNode
//
// Description:		Evaluates single node.  Removes children from the node
//					in the process.
//
// Input Arguments:
//		expression	= wxString& containing the remaining portion of the expression
//
// Output Arguments:
//		errorString	= wxString& containing a description of any errors encountered
//
// Return Value:
//		Node evaluated
//
//==========================================================================
ExpressionTree::Node ExpressionTree::EvaluateNextNode(wxString &expression, wxString &errorString)
{
	// FIXME:  Double operators are not handled properly and do not generate errors (example: 5*int([1]) evaluates as int([1]) )
	Node node;
	wxString nextOperator;
	bool firstChar = true;
	while (expression.Len() > 0)
	{
		if (expression.at(0) == ')')
		{
			// Exit the loop
			expression = expression.Mid(1);
			break;
		}
		else if (expression.at(0) == '(')
		{
			// Make recursive call
			expression = expression.Mid(1);
			Node newNode = EvaluateNextNode(expression, errorString);

			// Don't continue if we have an error
			if (!errorString.IsEmpty())
				return node;

			if (nextOperator.IsEmpty())
				// FIXME:  If there is no operator, we should generate an error (example: 3(3+[2]) should generate an error, but doesn't)
				node = newNode;
			else
			{
				// Apply the operator
				if (nextOperator.CmpNoCase(_T("ddt")) == 0)
					node.set = DiscreteDerivative::ComputeTimeHistory(newNode.set);
				else if (nextOperator.CmpNoCase(_T("int")) == 0)
					node.set = DiscreteIntegral::ComputeTimeHistory(newNode.set);
				else if (nextOperator.CmpNoCase(_T("fft")) == 0)
				{
					node.set = FastFourierTransform::Compute(newNode.set);

					// Scale the X-axis as required
					node.set.MultiplyXData(xAxisFactor);
				}
				else if (node.set.GetNumberOfPoints() > 0 && newNode.set.GetNumberOfPoints() > 0)
				{
					// set and set
					if (nextOperator.compare(_T("+")) == 0)
						node.set += newNode.set;
					else if (nextOperator.compare(_T("-")) == 0)
						node.set -= newNode.set;
					else if (nextOperator.compare(_T("*")) == 0)
						node.set *= newNode.set;
					else if (nextOperator.compare(_T("/")) == 0)
						node.set /= newNode.set;
					else
						assert(false);
				}
				else if (node.set.GetNumberOfPoints())
				{
					// set and double
					if (nextOperator.compare(_T("+")) == 0)
						node.set += newNode.dValue;
					else if (nextOperator.compare(_T("-")) == 0)
						node.set -= newNode.dValue;
					else if (nextOperator.compare(_T("*")) == 0)
						node.set *= newNode.dValue;
					else if (nextOperator.compare(_T("/")) == 0)
						node.set /= newNode.dValue;
					else if (nextOperator.compare(_T("%")) == 0)
						node.set = node.set % newNode.dValue;
					else
						assert(false);
				}
				else if (newNode.set.GetNumberOfPoints() > 0)
				{
					node.set = newNode.set;

					// double and set
					if (nextOperator.compare(_T("+")) == 0)
						node.set += newNode.dValue;
					else if (nextOperator.compare(_T("-")) == 0)
					{
						// Special handling because this is double - set, not set - double
						node.set *= -1.0;
						node.set += newNode.set;
					}
					else if (nextOperator.compare(_T("*")) == 0)
						node.set *= newNode.dValue;
					else if (nextOperator.compare(_T("/")) == 0)
					{
						// Can't hande this case
						errorString = _T("Cannot divide a number by a dataset!");
						return node;
					}
					else
						assert(false);
				}
				else
				{
					// double and double
					if (nextOperator.compare(_T("+")) == 0)
						node.dValue += newNode.dValue;
					else if (nextOperator.compare(_T("-")) == 0)
						node.dValue -= newNode.dValue;
					else if (nextOperator.compare(_T("*")) == 0)
						node.dValue *= newNode.dValue;
					else if (nextOperator.compare(_T("/")) == 0)
						node.dValue /= newNode.dValue;
					else if (nextOperator.compare(_T("%")) == 0)
						node.dValue = PlotMath::Modulo(node.dValue, newNode.dValue);
					else
						assert(false);
				}

				// Clear the operator
				nextOperator.Empty();
			}
		}
		else
		{
			// Evaluate
			// Are we at a number?
			if (expression.Mid(0, 1).IsNumber() &&
				expression.Mid(0, 1).compare(_T("+")) != 0 &&// Not a + sign
				(expression.Mid(0, 1).compare(_T("-")) != 0 ||// Not a - sign
				(expression.Mid(0, 1).compare(_T("-")) == 0 &&
				(!nextOperator.IsEmpty() || firstChar))))// Is a - sign, but to identify a negative number
			{
				unsigned int i;
				for (i = 1; i < expression.Len(); i++)
				{
					if ((!expression.Mid(i, 1).IsNumber() &&
						expression.Mid(i, 1).compare(_T(".")) != 0) ||
						expression.Mid(i, 1).compare(_T("-")) == 0 ||
						expression.Mid(i, 1).compare(_T("+")) == 0)
					{
						break;
					}
				}

				// This could be reached if user inputed -[x] (invert dataset) - handle this here
				if (expression.at(i) == '[')
				{
					int end = expression.find(_T("]"), i);
					if (end == wxNOT_FOUND)
					{
						errorString = _T("Missing ']'!");
						return node;
					}

					Dataset2D newSet;
					long set;
					if (expression.Mid(i + 1, end - i - 1).ToLong(&set))
					{
						if (set >= 0 && set <= list.GetCount())
							newSet = GetSetFromList(set) * -1.0;
						else
						{
							errorString.Printf("Dataset ID %li is invalid!", set);
							return node;
						}
					}
					else
					{
						errorString = _T("Unrecognized dataset ID:  ") + expression.Mid(i + 1, end - i - 1);
						return node;
					}

					// Handle the math
					if (node.set.GetNumberOfPoints() == 0)
					{
						node.set = newSet;

						// handle operating on a double and a set
						if (nextOperator.compare(_T("+")) == 0)
							node.set += node.dValue;
						else if (nextOperator.compare(_T("-")) == 0)
						{
							// Special handling because this is double - set, not set - double
							node.set *= -1.0;
							node.set += node.dValue;
						}
						else if (nextOperator.compare(_T("*")) == 0)
							node.set *= node.dValue;
						else// "/"
						{
							// Can't hande this case
							errorString = _T("Cannot divide a number by a dataset!");
							return node;
						}
					}
					else
					{
						// handle operating on two sets
						if (nextOperator.compare(_T("+")) == 0)
							node.set += newSet;
						else if (nextOperator.compare(_T("-")) == 0)
							node.set -= newSet;
						else if (nextOperator.compare(_T("*")) == 0)
							node.set *= newSet;
						else if (nextOperator.compare(_T("/")) == 0)
							node.set /= newSet;
						else
							assert(false);
					}

					nextOperator.Empty();

					// Shorten the expression
					expression = expression.Mid(end + 1);

					continue;
				}

				if (nextOperator.IsEmpty())
					expression.Mid(0, i).ToDouble(&node.dValue);
				else
				{
					double value;
					expression.Mid(0, i).ToDouble(&value);

					// Are operating on two doubles, or one double and one set?
					if (node.set.GetNumberOfPoints() == 0)
					{
						// handle operating on two doubles
						if (nextOperator.compare(_T("+")) == 0)
							node.dValue += value;
						else if (nextOperator.compare(_T("-")) == 0)
							node.dValue -= value;
						else if (nextOperator.compare(_T("*")) == 0)
							node.dValue *= value;
						else if (nextOperator.compare(_T("/")) == 0)
							node.dValue /= value;
						else if (nextOperator.compare(_T("%")) == 0)
							node.dValue = PlotMath::Modulo(node.dValue, value);
						else
							assert(false);
					}
					else
					{
						// handle operating on a set and a double
						if (nextOperator.compare(_T("+")) == 0)
							node.set += value;
						else if (nextOperator.compare(_T("-")) == 0)
							node.set -= value;
						else if (nextOperator.compare(_T("*")) == 0)
							node.set *= value;
						else if (nextOperator.compare(_T("/")) == 0)
							node.set /= value;
						else if (nextOperator.compare(_T("%")) == 0)
							node.set = node.set % value;
						else
							assert(false);
					}

					nextOperator.Empty();
				}

				// Shorten the string
				expression = expression.Mid(i);
			}
			// At a dataset identifier
			else if (expression.Mid(0, 1).CmpNoCase(_T("[")) == 0)
			{
				long i;
				Dataset2D newSet;
				int end = expression.Find(_T("]"));
				if (end == wxNOT_FOUND)
				{
					errorString = _T("Missing ']'!");
					return node;
				}

				if (expression.Mid(1, end - 1).ToLong(&i))
				{
					if (i >= 0 && i <= list.GetCount())
						newSet = GetSetFromList(i);
					else
					{
						errorString.Printf("Dataset ID %li is invalid!", i);
						return node;
					}
				}
				else
				{
					wxString t=expression.Mid(1, end-1);
					errorString = _T("Unrecognized dataset ID:  ") + expression.Mid(1, end - 1);
					return node;
				}
				expression = expression.Mid(end + 1);

				if (nextOperator.IsEmpty())
					node.set = newSet;
				else
				{
					if (node.set.GetNumberOfPoints() == 0)
					{
						node.set = newSet;

						// handle operating on a double and a set
						if (nextOperator.compare(_T("+")) == 0)
							node.set += node.dValue;
						else if (nextOperator.compare(_T("-")) == 0)
						{
							// Special handling because this is double - set, not set - double
							node.set *= -1.0;
							node.set += node.dValue;
						}
						else if (nextOperator.compare(_T("*")) == 0)
							node.set *= node.dValue;
						else if (nextOperator.compare(_T("/")) == 0)
						{
							// Can't hande this case
							errorString = _T("Cannot divide a number by a dataset!");
							return node;
						}
						else if (nextOperator.compare(_T("%")) == 0)
						{
							// Can't hande this case
							errorString = _T("Cannot perform modulo with RHS dataset!");
							return node;
						}
					}
					else
					{
						// handle operating on two sets
						if (nextOperator.compare(_T("+")) == 0)
							node.set += newSet;
						else if (nextOperator.compare(_T("-")) == 0)
							node.set -= newSet;
						else if (nextOperator.compare(_T("*")) == 0)
							node.set *= newSet;
						else if (nextOperator.compare(_T("/")) == 0)
							node.set /= newSet;
						else
							assert(false);
					}

					nextOperator.Empty();
				}
			}
			else// Must be at an operator
			{
				// Store the operator so we know what to do next
				if (expression.Mid(0,3).CmpNoCase(_T("int")) == 0)
				{
					nextOperator = _T("int");
					expression = expression.Mid(3);
				}
				else if (expression.Mid(0,3).CmpNoCase(_T("ddt")) == 0)
				{
					nextOperator = _T("ddt");
					expression = expression.Mid(3);
				}
				else if (expression.Mid(0,3).CmpNoCase(_T("fft")) == 0)
				{
					nextOperator = _T("fft");
					expression = expression.Mid(3);
				}
				else if (expression.Mid(0,1).compare(_T("+")) == 0 ||
					expression.Mid(0,1).compare(_T("-")) == 0 ||
					expression.Mid(0,1).compare(_T("*")) == 0 ||
					expression.Mid(0,1).compare(_T("/")) == 0 ||
					expression.Mid(0,1).compare(_T("%")) == 0)
				{
					nextOperator = expression.at(0);
					expression = expression.Mid(1);
				}
				else
				{
					errorString = _T("Unrecognized character:  ") + expression.Mid(0, 1);
					return node;
				}
			}
		}

		firstChar = false;
	}

	return node;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		GetSetFromList
//
// Description:		Retrieves the proper set from the list.  Handles i=0
//					where 0 indicates time series instead of data.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		Datased2D requested
//
//==========================================================================
Dataset2D ExpressionTree::GetSetFromList(const unsigned int &i)
{
	// If user is requesting time, we need to assign the x values to the y values
	if (i == 0)
	{
		Dataset2D set(*list[0]);
		unsigned int j;
		for (j = 0; j < set.GetNumberOfPoints(); j++)
			set.GetYPointer()[j] = set.GetXData(j);

		return set;
	}
	
	return *list[i - 1];
}