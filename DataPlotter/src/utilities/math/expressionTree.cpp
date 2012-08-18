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

	if (!ParenthesesBalanced(expression))
		return _T("Imbalanced parentheses!");

	wxString errorString;
	errorString = ParseExpression(expression);

	if (!errorString.IsEmpty())
		return errorString;

	errorString = EvaluateExpression(solvedData);

	return errorString;
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
Dataset2D ExpressionTree::GetSetFromList(const unsigned int &i) const
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

//==========================================================================
// Class:			ExpressionTree
// Function:		ParenthesesBalanced
//
// Description:		Checks to see if the expression has balanced parentheses.
//
// Input Arguments:
//		expression	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if parentheses are balanced, false otherwise
//
//==========================================================================
bool ExpressionTree::ParenthesesBalanced(const wxString &expression) const
{
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
		return false;

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ParseExpression
//
// Description:		Parses the expression and produces a queue of Reverse
//					Polish Notation values and operations.  Implements the
//					shunting-yard algorithm as described by Wikipedia.
//
// Input Arguments:
//		expression	= const wxString& to be parsed
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing error descriptions or an empty string on success
//
//==========================================================================
wxString ExpressionTree::ParseExpression(const wxString &expression)
{
	std::stack<wxString> operatorStack;
	unsigned int i, advance;
	for (i = 0; i < expression.Len(); i++)
	{
		if (expression.Mid(i, 1).Trim().IsEmpty())
			continue;

		if (NextIsNumber(expression.Mid(i), &advance))
			outputQueue.push(expression.Mid(i, advance));
		else if (NextIsDataset(expression.Mid(i), &advance))
			outputQueue.push(expression.Mid(i, advance));
		else if (NextIsFunction(expression.Mid(i), &advance))
			operatorStack.push(expression.Mid(i, advance));
		else if (NextIsOperator(expression.Mid(i), &advance))
			ProcessOperator(operatorStack, expression.Mid(i, advance));
		else if (expression[i] == '(')
		{
			operatorStack.push(expression.Mid(i, 1));
			advance = 1;
		}
		else if (expression[i] == ')')
		{
			ProcessCloseParenthese(operatorStack);
			advance = 1;
		}
		else
			return _T("Unrecognized character:  '") + expression.Mid(i, 1) + _T("'.");

		i += advance - 1;
	}

	if (!EmptyStackToQueue(operatorStack))
		return _T("Imbalanced parentheses!");
	return wxEmptyString;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ProcessOperator
//
// Description:		Processes the next operator in the expression, adding it
//					to the appropriate stack.  This method enforces the order
//					of operations.
//
// Input Arguments:
//		operatorStack	= std::stack<wxString>&
//		s				= const wxString& representing the next operator
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::ProcessOperator(std::stack<wxString> &operatorStack, const wxString &s)
{
	// Handle operator precedence
	while (!operatorStack.empty())
	{
		if ((!NextIsOperator(operatorStack.top()) ||
			!OperatorShift(operatorStack.top(), s)) &&
			!NextIsFunction(operatorStack.top()))// Force functions to pop to maintain highest precedence
			break;
		PopStackToQueue(operatorStack);
	}
	operatorStack.push(s);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ProcessCloseParenthese
//
// Description:		Adjusts the stacks in response to encountering a close
//					parenthese.
//
// Input Arguments:
//		operatorStack	= std::stack<wxString>&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::ProcessCloseParenthese(std::stack<wxString> &operatorStack)
{
	while (!operatorStack.empty())
	{
		if (operatorStack.top().Cmp(_T("(")) == 0)
			break;
		PopStackToQueue(operatorStack);
	}

	if (operatorStack.empty())
	{
		assert(false);
		// Should never happen due to prior parenthese balance checks
		//return _T("Imbalanced parentheses!");
	}

	operatorStack.pop();
	if (!operatorStack.empty())
	{
		if (NextIsFunction(operatorStack.top()))
			PopStackToQueue(operatorStack);
	}
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateExpression
//
// Description:		Evaluates the expression in the queue using Reverse Polish
//					Notation.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		results	= Dataset2D&
//
// Return Value:
//		wxString containing a description of any errors, or wxEmptyString on success
//
//==========================================================================
wxString ExpressionTree::EvaluateExpression(Dataset2D &results)
{
	wxString next, errorString;

	std::stack<double> doubleStack;
	std::stack<Dataset2D> setStack;
	std::stack<bool> useDoubleStack;

	if (NextIsOperator(outputQueue.front()))// Special handling in case of "-3*..."
		PushToStack(0.0, doubleStack, useDoubleStack);

	while (!outputQueue.empty())
	{
		next = outputQueue.front();
		outputQueue.pop();

		if (!EvaluateNext(next, doubleStack, setStack, useDoubleStack, errorString))
			return errorString;
	}

	if (useDoubleStack.top())
		return _T("Expression evaluates to a number!");
	else
		results = setStack.top();

	return wxEmptyString;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		PopStackToQueue
//
// Description:		Removes the top entry of the stack and puts it in the queue.
//
// Input Arguments:
//		stack	= std::stack<wxString>& to be popped
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::PopStackToQueue(std::stack<wxString> &stack)
{
	outputQueue.push(stack.top());
	stack.pop();
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EmptyStackToQueue
//
// Description:		Empties the contents of the stack into the queue.
//
// Input Arguments:
//		stack	= std::stack<wxString>& to be emptied
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise (imbalance parentheses)
//
//==========================================================================
bool ExpressionTree::EmptyStackToQueue(std::stack<wxString> &stack)
{
	while (!stack.empty())
	{
		if (stack.top().Cmp(_T("(")) == 0)
			return false;
		PopStackToQueue(stack);
	}

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		NextIsNumber
//
// Description:		Determines if the next portion of the expression is a number.
//
// Input Arguments:
//		s		= const wxString& containing the expression
//
// Output Arguments:
//		stop	= unsigned int* (optional) indicating length of number
//
// Return Value:
//		bool, true if a number is next in the expression
//
//==========================================================================
bool ExpressionTree::NextIsNumber(const wxString &s, unsigned int *stop) const
{
	if (s.Len() == 0)
		return false;

	bool foundDecimal = s[0] == '.';
	if (foundDecimal ||
		(int(s[0]) >= int('0') && int(s[0]) <= int('9')))
	{
		unsigned int i;
		for (i = 1; i < s.Len(); i++)
		{
			if (s[i] == '.')
			{
				if (foundDecimal)
					return false;
				foundDecimal = true;
			}
			else if (int(s[i]) < int('0') || int(s[i]) > int('9'))
				break;
		}

		if (stop)
			*stop = i;
		return true;
	}

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		NextIsDataset
//
// Description:		Determines if the next portion of the expression is a dataset.
//
// Input Arguments:
//		s		= const wxString& containing the expression
//
// Output Arguments:
//		stop	= unsigned int* (optional) indicating length of dataset ID
//
// Return Value:
//		bool, true if a dataset is next in the expression
//
//==========================================================================
bool ExpressionTree::NextIsDataset(const wxString &s, unsigned int *stop) const
{
	if (s.Len() < 3)
		return false;

	if (s[0] == '[')
	{
		unsigned int close = s.Find(']');
		if (close == (unsigned int)wxNOT_FOUND)
			return false;

		unsigned int i;
		for (i = 1; i < close; i++)
		{
			if (int(s[i]) < '0' || int(s[i]) > '9')
				return false;
		}

		if (stop)
			*stop = close + 1;
		return true;
	}

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		NextIsFunction
//
// Description:		Determines if the next portion of the expression is a function.
//
// Input Arguments:
//		s		= const wxString& containing the expression
//
// Output Arguments:
//		stop	= unsigned int* (optional) indicating length of function
//
// Return Value:
//		bool, true if a function is next in the expression
//
//==========================================================================
bool ExpressionTree::NextIsFunction(const wxString &s, unsigned int *stop) const
{
	if (s.Len() < 3)
		return false;

	if (s.Mid(0, 3).CmpNoCase(_T("int")) == 0 ||
		s.Mid(0, 3).CmpNoCase(_T("ddt")) == 0 ||
		s.Mid(0, 3).CmpNoCase(_T("fft")) == 0 ||
		s.Mid(0, 3).CmpNoCase(_T("bit")) == 0)
	{
		if (stop)
			*stop = 3;
		return true;
	}
	else if (s.Mid(0, 8).CmpNoCase(_T("transfer")) == 0)
	{
		if (stop)
			*stop = 8;
		return true;
	}

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		NextIsOperator
//
// Description:		Determines if the next portion of the expression is an operator.
//
// Input Arguments:
//		s		= const wxString& containing the expression
//
// Output Arguments:
//		stop	= unsigned int* (optional) indicating length of operator
//
// Return Value:
//		bool, true if an operator is next in the expression
//
//==========================================================================
bool ExpressionTree::NextIsOperator(const wxString &s, unsigned int *stop) const
{
	if (s.Len() == 0)
		return false;

	if (s[0] == '+' ||// From least precedence
		s[0] == '-' ||
		s[0] == '*' ||
		s[0] == '/' ||
		s[0] == '%' ||
		s[0] == '^')// To most precedence
	{
		if (stop)
			*stop = 1;
		return true;
	}

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		OperatorShift
//
// Description:		Determines if the new operator requires a shift in
//					operator placement.
//
// Input Arguments:
//		stackString	= const wxString& containing the expression
//		newString	= const wxString& containing the expression
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if shifting needs to occur
//
//==========================================================================
bool ExpressionTree::OperatorShift(const wxString &stackString, const wxString &newString) const
{
	unsigned int stackPrecedence = GetPrecedence(stackString);
	unsigned int newPrecedence = GetPrecedence(newString);

	if (stackPrecedence == 0 || newPrecedence == 0)
		return false;

	if (IsLeftAssociative(newString[0]))
	{
		if (newPrecedence <= stackPrecedence)
			return true;
	}
	else if (newPrecedence < stackPrecedence)
		return true;

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		GetPrecedence
//
// Description:		Determines the precedence of the specified operator
//					(higher values are performed first)
//
// Input Arguments:
//		s	= const wxString& containing the operator
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int representing the precedence
//
//==========================================================================
unsigned int ExpressionTree::GetPrecedence(const wxString &s) const
{
	if (s.Len() != 1)
		return 0;

	if (s[0] == '+' ||
		s[0] == '-')
		return 2;
	else if (s[0] == '*' ||
		s[0] == '/' ||
		s[0] == '%')
		return 3;
	else if (s[0] == '^')
		return 4;

	return 0;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		IsLeftAssociative
//
// Description:		Determines if the specified operator is left or right
//					associative.
//
// Input Arguments:
//		c	= const wxChar&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if left associative
//
//==========================================================================
bool ExpressionTree::IsLeftAssociative(const wxChar &c) const
{
	switch (c)
	{
	case '^':
		return false;

	default:
		return true;
	}
}

//==========================================================================
// Class:			ExpressionTree
// Function:		PushToStack
//
// Description:		Pushes the specified value onto the stack.
//
// Input Arguments:
//		value			= const double&
//		doubleStack		= std::stack<double>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::PushToStack(const double &value, std::stack<double> &doubleStack,
	std::stack<bool> &useDoubleStack) const
{
	doubleStack.push(value);
	useDoubleStack.push(true);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		PushToStack
//
// Description:		Pushes the specified dataset onto the stack.
//
// Input Arguments:
//		dataset			= const Dataset2D&
//		setStack		= std::stack<Dataset2D>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::PushToStack(const Dataset2D &dataset, std::stack<Dataset2D> &setStack,
	std::stack<bool> &useDoubleStack) const
{
	setStack.push(dataset);
	useDoubleStack.push(false);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		PopFromStack
//
// Description:		Pops the next value from the top of the appropriate stack.
//
// Input Arguments:
//		doubleStack		= std::stack<double>&
//		setStack		= std::stack<Dataset2D>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		value			= double&
//		dataset			= Dataset2D&
//
// Return Value:
//		bool, true if a double was popped, false if a dataset was popped
//
//==========================================================================
bool ExpressionTree::PopFromStack(std::stack<double> &doubleStack, std::stack<Dataset2D> &setStack,
	std::stack<bool> &useDoubleStack, double &value, Dataset2D &dataset) const
{
	assert(!useDoubleStack.empty());

	bool useDouble = useDoubleStack.top();
	useDoubleStack.pop();

	if (useDouble)
	{
		assert(!doubleStack.empty());
		value = doubleStack.top();
		doubleStack.pop();
	}
	else
	{
		assert(!setStack.empty());
		dataset = setStack.top();
		setStack.pop();
	}

	return useDouble;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ApplyFunction
//
// Description:		Applies the specified function to the specified dataset.
//
// Input Arguments:
//		function	= const wxString& describing the function to apply
//		set			= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing the result of the function
//
//==========================================================================
Dataset2D ExpressionTree::ApplyFunction(const wxString &function,
	const Dataset2D &set) const
{
	if (function.CmpNoCase(_T("int")) == 0)
		return DiscreteIntegral::ComputeTimeHistory(set);
	else if (function.CmpNoCase(_T("ddt")) == 0)
		return DiscreteDerivative::ComputeTimeHistory(set);
	else if (function.CmpNoCase(_T("fft")) == 0)
		return FastFourierTransform::ComputeFFT(set).MultiplyXData(xAxisFactor);
	/*else if (function.CmpNoCase(_T("bit")) == 0)
		return PlotMath::ApplyBitMask(set, bit);
	else if (function.CmpNoCase(_T("transfer")) == 0)
		return FastFourierTransform::ComputeTransferFunction(set1, set2);*/

	assert(false);
	return set;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ApplyOperation
//
// Description:		Applies the specified operation to the specified operands.
//
// Input Arguments:
//		operation	= const wxString& describing the function to apply
//		first		= const Dataset2D&
//		second		= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing the result of the operation
//
//==========================================================================
Dataset2D ExpressionTree::ApplyOperation(const wxString &operation,
	const Dataset2D &first, const Dataset2D &second) const
{
	if (operation.Cmp(_T("+")) == 0)
		return second + first;
	else if (operation.Cmp(_T("-")) == 0)
		return second - first;
	else if (operation.Cmp(_T("*")) == 0)
		return second * first;
	else if (operation.Cmp(_T("/")) == 0)
		return second / first;

	assert(false);
	return first;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ApplyOperation
//
// Description:		Applies the specified operation to the specified operands.
//
// Input Arguments:
//		operation	= const wxString& describing the function to apply
//		first		= const Dataset2D&
//		second		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing the result of the operation
//
//==========================================================================
Dataset2D ExpressionTree::ApplyOperation(const wxString &operation,
	const Dataset2D &first, const double &second) const
{
	// These operations have some orders reversed in order to avoid undefined operations for doubles
	if (operation.Cmp(_T("+")) == 0)
		return first + second;
	else if (operation.Cmp(_T("-")) == 0)
		return first * -1.0 + second;
	else if (operation.Cmp(_T("*")) == 0)
		return first * second;

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ApplyOperation
//
// Description:		Applies the specified operation to the specified operands.
//
// Input Arguments:
//		operation	= const wxString& describing the function to apply
//		first		= const double&
//		second		= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing the result of the operation
//
//==========================================================================
Dataset2D ExpressionTree::ApplyOperation(const wxString &operation,
	const double &first, const Dataset2D &second) const
{
	if (operation.Cmp(_T("+")) == 0)
		return second + first;
	else if (operation.Cmp(_T("-")) == 0)
		return second - first;
	else if (operation.Cmp(_T("*")) == 0)
		return second * first;
	else if (operation.Cmp(_T("/")) == 0)
		return second / first;
	else if (operation.Cmp(_T("%")) == 0)
		return second % first;
	else if (operation.Cmp(_T("^")) == 0)
		return second.ToPower(first);

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ApplyOperation
//
// Description:		Applies the specified operation to the specified operands.
//
// Input Arguments:
//		operation	= const wxString& describing the function to apply
//		first		= const double&
//		second		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double containing the result of the operation
//
//==========================================================================
double ExpressionTree::ApplyOperation(const wxString &operation,
	const double &first, const double &second) const
{
	if (operation.Cmp(_T("+")) == 0)
		return second + first;
	else if (operation.Cmp(_T("-")) == 0)
		return second - first;
	else if (operation.Cmp(_T("*")) == 0)
		return second * first;
	else if (operation.Cmp(_T("/")) == 0)
		return second / first;
	else if (operation.Cmp(_T("%")) == 0)
		return PlotMath::Modulo(second, first);
	else if (operation.Cmp(_T("^")) == 0)
		return pow(second, first);

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateFunction
//
// Description:		Evaluates the function specified.
//
// Input Arguments:
//		function		= const wxString& describing the function to apply
//		doubleStack		= std::stack<double>&
//		setStack		= std::stack<Dataset2D>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString		= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateFunction(const wxString &function, std::stack<double> &doubleStack,
	std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	double value;
	Dataset2D dataset;

	if (useDoubleStack.empty())
	{
		errorString = _T("Attempting to apply function without argument!");
		return false;
	}
	else if (PopFromStack(doubleStack, setStack, useDoubleStack, value, dataset))
	{
		errorString = _T("Attempting to apply function to value (requires dataset).");
		return false;
	}

	// FIXME:  Handle multiple args here

	PushToStack(ApplyFunction(function, dataset), setStack, useDoubleStack);

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateOperator
//
// Description:		Evaluates the operator specified.
//
// Input Arguments:
//		operator		= const wxString& describing the function to apply
//		doubleStack		= std::stack<double>&
//		setStack		= std::stack<Dataset2D>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString		= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateOperator(const wxString &operation, std::stack<double> &doubleStack,
	std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	double value1, value2;
	Dataset2D dataset1, dataset2;

	if (useDoubleStack.size() < 2)
	{
		errorString = _T("Attempting to apply operator without two operands!");
		return false;
	}
	else if (PopFromStack(doubleStack, setStack, useDoubleStack, value1, dataset1))
	{
		if (PopFromStack(doubleStack, setStack, useDoubleStack, value2, dataset2))
			PushToStack(ApplyOperation(operation, value1, value2), doubleStack, useDoubleStack);
		else
			PushToStack(ApplyOperation(operation, value1, dataset2), setStack, useDoubleStack);
	}
	else if (PopFromStack(doubleStack, setStack, useDoubleStack, value2, dataset2))
	{
		if (!SetOperatorValid(operation, true))
		{
			errorString = wxString::Format("The number %s dataset operation is invalid.", operation.c_str());
			return false;
		}
		PushToStack(ApplyOperation(operation, dataset1, value2), setStack, useDoubleStack);
	}
	else
	{
		if (!SetOperatorValid(operation, false))
		{
			errorString = wxString::Format("The dataset %s dataset operation is invalid.", operation.c_str());
			return false;
		}
		PushToStack(ApplyOperation(operation, dataset1, dataset2), setStack, useDoubleStack);
	}
	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateNumber
//
// Description:		Evaluates the number specified.
//
// Input Arguments:
//		number			= const wxString& describing the function to apply
//		doubleStack		= std::stack<double>&
//		setStack		= std::stack<Dataset2D>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString		= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateNumber(const wxString &number, std::stack<double> &doubleStack,
	std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	double value;

	if (!number.ToDouble(&value))
	{
		errorString = _T("Could not convert ") + number + _T(" to a number.");
		return false;
	}

	PushToStack(value, doubleStack, useDoubleStack);

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateDataset
//
// Description:		Evaluates the dataset specified.
//
// Input Arguments:
//		dataset			= const wxString& describing the function to apply
//		setStack		= std::stack<Dataset2D>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString		= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateDataset(const wxString &dataset, std::stack<Dataset2D> &setStack,
	std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	unsigned long set;
	if (!dataset.Mid(1, dataset.Len() - 2).ToULong(&set))
	{
		errorString = _T("Could not convert '") + dataset + _T("' to set ID.");
		return false;
	}
	else if (set > (unsigned int)list.GetCount())
	{
		errorString = wxString::Format("Set ID %lu is not a valid set ID", set);
		return false;
	}

	PushToStack(GetSetFromList(set), setStack, useDoubleStack);

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		SetOperatorValid
//
// Description:		Verifies if the specified operator is valid for dataset operations.
//
// Input Arguments:
//		operation			= const wxString&
//		leftOperandIsDouble	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for valid operation, false otherwise
//
//==========================================================================
bool ExpressionTree::SetOperatorValid(const wxString &operation, const bool &leftOperandIsDouble) const
{
	if (operation.Cmp(_T("+")) == 0)
		return true;
	else if (operation.Cmp(_T("-")) == 0)
		return true;
	else if (operation.Cmp(_T("*")) == 0)
		return true;

	if (!leftOperandIsDouble &&
		operation.Cmp(_T("/")) == 0)
			return true;

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateNext
//
// Description:		Determines how to evaluate the specified term and takes
//					appropriate action.
//
// Input Arguments:
//		operation			= const wxString&
//		leftOperandIsDouble	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for valid operation, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateNext(const wxString &next, std::stack<double> &doubleStack,
		std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	if (NextIsFunction(next))
		return EvaluateFunction(next, doubleStack, setStack, useDoubleStack, errorString);
	else if(NextIsOperator(next))
		return EvaluateOperator(next, doubleStack, setStack, useDoubleStack, errorString);
	else if (NextIsDataset(next))
		return EvaluateDataset(next, setStack, useDoubleStack, errorString);

	return EvaluateNumber(next, doubleStack, useDoubleStack, errorString);
}
