/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

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
//		_list		= const ManagedList<const Dataset2D>* reference to the
//					  other datasets which may be required to complete the calculation
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ExpressionTree::ExpressionTree(const ManagedList<const Dataset2D> *_list) : list(_list)
{
}

//==========================================================================
// Class:			ExpressionTree
// Function:		Constant Declarations
//
// Description:		Constant declarations for ExpressionTree class.
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
const unsigned int ExpressionTree::printfPrecision = 15;

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
// Function:		Solve
//
// Description:		Main solving method for the tree.
//
// Input Arguments:
//		expression			= wxString containing the expression to parse
//		solvedExpression	=
//
// Output Arguments:
//		solvedData		= Dataset2D& containing the evaluated data
//
// Return Value:
//		std::string, empty for success, error string if unsuccessful
//
//==========================================================================
std::string ExpressionTree::Solve(std::string expression, std::string &solvedExpression)
{
	if (!ParenthesesBalanced(expression))
		return "Imbalanced parentheses!";

	std::string errorString;
	errorString = ParseExpression(expression).c_str();

	if (!errorString.empty())
		return errorString;

	errorString = EvaluateExpression(solvedExpression);

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
	assert(list);

	// If user is requesting time, we need to assign the x values to the y values
	if (i == 0)
	{
		Dataset2D set(*(*list)[0]);
		unsigned int j;
		for (j = 0; j < set.GetNumberOfPoints(); j++)
			set.GetYPointer()[j] = set.GetXData(j);

		return set;
	}

	return *(*list)[i - 1];
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
	bool lastWasOperator(true);
	wxString errorString;

	for (i = 0; i < expression.Len(); i++)
	{
		if (expression.Mid(i, 1).Trim().IsEmpty())
			continue;

		errorString = ParseNext(expression.Mid(i), lastWasOperator, advance, operatorStack);
		if (!errorString.IsEmpty())
			return errorString;
		i += advance - 1;
	}

	if (!EmptyStackToQueue(operatorStack))
		errorString = _T("Imbalanced parentheses!");

	return errorString;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ParseNext
//
// Description:		Parses the expression and processes the next item.
//
// Input Arguments:
//		expression	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing any errors
//
//==========================================================================
wxString ExpressionTree::ParseNext(const wxString &expression, bool &lastWasOperator,
	unsigned int &advance, std::stack<wxString> &operatorStack)
{
	bool thisWasOperator(false);
	if (NextIsNumber(expression, &advance, lastWasOperator))
		outputQueue.push(expression.Mid(0, advance));
	else if (NextIsDataset(expression, &advance, lastWasOperator))
		outputQueue.push(expression.Mid(0, advance));
	else if (NextIsS(expression, &advance))
		outputQueue.push(expression.Mid(0, advance));
	else if (NextIsFunction(expression, &advance))
	{
		operatorStack.push(expression.Mid(0, advance));
		thisWasOperator = true;
	}
	else if (NextIsOperator(expression, &advance))
	{
		ProcessOperator(operatorStack, expression.Mid(0, advance));
		thisWasOperator = true;
	}
	else if (expression[0] == '(')
	{
		if (!lastWasOperator)
			operatorStack.push(_T("*"));
		operatorStack.push(expression[0]);
		advance = 1;
		thisWasOperator = true;
	}
	else if (expression[0] == ')')
	{
		ProcessCloseParenthese(operatorStack);
		advance = 1;
	}
	else
		return _T("Unrecognized character:  '") + expression.Mid(0, 1) + _T("'.");
	lastWasOperator = thisWasOperator;
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

	while (!outputQueue.empty())
	{
		next = outputQueue.front();
		outputQueue.pop();

		if (!EvaluateNext(next, doubleStack, setStack, useDoubleStack, errorString))
			return errorString;
	}

	if (useDoubleStack.size() > 1)
		return _T("Not enough operators!");

	if (useDoubleStack.top())
		return _T("Expression evaluates to a number!");
	else
		results = setStack.top();

	return wxEmptyString;
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
//		results	= std::string&
//
// Return Value:
//		std::string containing a description of any errors, or empty string on success
//
//==========================================================================
std::string ExpressionTree::EvaluateExpression(std::string &results)
{
	wxString next, errorString;

	std::stack<double> doubleStack;
	std::stack<wxString> stringStack;
	std::stack<bool> useDoubleStack;

	while (!outputQueue.empty())
	{
		next = outputQueue.front();
		outputQueue.pop();

		if (!EvaluateNext(next, doubleStack, stringStack, useDoubleStack, errorString))
			return std::string(errorString.mb_str());
	}

	if (useDoubleStack.size() > 1)
		return "Not enough operators!";

	if (useDoubleStack.top())
		results = wxString::Format("%0.*f",
			PlotMath::GetPrecision(doubleStack.top(), printfPrecision),
			doubleStack.top()).mb_str();
	else
		results = stringStack.top();

	return "";
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
//					Some cleverness is required to tell the difference between
//					a minus sign and a negative sign (minus sign would return false).
//
// Input Arguments:
//		s				= const wxString& containing the expression
//		lastWasOperator	= const bool& indicating whether or not the last thing
//						  on the stack is an operator
//
// Output Arguments:
//		stop	= unsigned int* (optional) indicating length of number
//
// Return Value:
//		bool, true if a number is next in the expression
//
//==========================================================================
bool ExpressionTree::NextIsNumber(const wxString &s, unsigned int *stop, const bool &lastWasOperator)
{
	if (s.Len() == 0)
		return false;

	bool foundDecimal = s[0] == '.';
	if (foundDecimal ||
		(int(s[0]) >= int('0') && int(s[0]) <= int('9')) ||
		(s[0] == '-' && lastWasOperator && NextIsNumber(s.Mid(1), NULL, false)))
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
// Function:		NextIsS
//
// Description:		Determines if the next portion of the expression is
//					complex frequency (s) or discrete time (z).
//
// Input Arguments:
//		s		= const wxString& containing the expression
//
// Output Arguments:
//		stop	= unsigned int* (optional) indicating length
//
// Return Value:
//		bool, true if a dataset is next in the expression
//
//==========================================================================
bool ExpressionTree::NextIsS(const wxString &s, unsigned int *stop)
{
	if (s[0] == 's' || s[0] == 'z')
	{
		if (stop)
			*stop = 1;
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
//		s				= const wxString& containing the expression
//		lastWasOperator	= const bool& indicating whether or not the last
//						  item pushed to the stack was an operator or not
//
// Output Arguments:
//		stop	= unsigned int* (optional) indicating length of dataset ID
//
// Return Value:
//		bool, true if a dataset is next in the expression
//
//==========================================================================
bool ExpressionTree::NextIsDataset(const wxString &s, unsigned int *stop, const bool &lastWasOperator)
{
	if (s.Len() < 3)
		return false;

	if (s[0] == '[' ||
		(s[0] == '-' && lastWasOperator && NextIsDataset(s.Mid(1), NULL, false)))
	{
		unsigned int close = s.Find(']');
		if (close == (unsigned int)wxNOT_FOUND)
			return false;

		unsigned int i;
		for (i = 1 + (unsigned int)lastWasOperator; i < close; i++)
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
bool ExpressionTree::NextIsFunction(const wxString &s, unsigned int *stop)
{
	// List these in order of longest to shortest
	if (BeginningMatchesNoCase(s, _T("log10"), stop))
		return true;
	else if (BeginningMatchesNoCase(s, _T("int"), stop))
		return true;
	else if (BeginningMatchesNoCase(s, _T("ddt"), stop))
		return true;
	else if (BeginningMatchesNoCase(s, _T("fft"), stop))
		return true;
	else if (BeginningMatchesNoCase(s, _T("frf"), stop))
		return true;
	else if (BeginningMatchesNoCase(s, _T("log"), stop))
		return true;
	else if (BeginningMatchesNoCase(s, _T("exp"), stop))
		return true;
	else if (BeginningMatchesNoCase(s, _T("abs"), stop))
		return true;
	else if (BeginningMatchesNoCase(s, _T("bit"), stop))
		return true;

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
bool ExpressionTree::NextIsOperator(const wxString &s, unsigned int *stop)
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
// Function:		PushToStack
//
// Description:		Pushes the specified dataset onto the stack.
//
// Input Arguments:
//		s				= const wxString&
//		stringStack		= std::stack<wxString>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::PushToStack(const wxString &s, std::stack<wxString> &stringStack,
	std::stack<bool> &useDoubleStack) const
{
	stringStack.push(s);
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
// Function:		PopFromStack
//
// Description:		Pops the next value from the top of the appropriate stack.
//
// Input Arguments:
//		doubleStack		= std::stack<double>&
//		stringStack		= std::stack<wxString>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		string			= wxString&
//		value			= double&
//
// Return Value:
//		bool, true if a double was popped, false otherwise
//
//==========================================================================
bool ExpressionTree::PopFromStack(std::stack<double> &doubleStack, std::stack<wxString> &stringStack,
	std::stack<bool> &useDoubleStack, wxString& string, double &value) const
{
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
		assert(!stringStack.empty());
		string = stringStack.top();
		stringStack.pop();
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
	else if (function.CmpNoCase(_T("log")) == 0)
		return set.DoLog();
	else if (function.CmpNoCase(_T("log10")) == 0)
		return set.DoLog10();
	else if (function.CmpNoCase(_T("exp")) == 0)
		return set.DoExp();
	else if (function.CmpNoCase(_T("abs")) == 0)
		return set.DoAbs();
	/*else if (function.CmpNoCase(_T("bit")) == 0)
		return PlotMath::ApplyBitMask(set, bit);
	else if (function.CmpNoCase(_T("frf")) == 0)
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
		return Dataset2D::DoUnsyncrhonizedAdd(second, first);
	else if (operation.Cmp(_T("-")) == 0)
		return Dataset2D::DoUnsyncrhonizedSubtract(second, first);
	else if (operation.Cmp(_T("*")) == 0)
		return Dataset2D::DoUnsyncrhonizedMultiply(second, first);
	else if (operation.Cmp(_T("/")) == 0)
		return Dataset2D::DoUnsyncrhonizedDivide(second, first);
	else if (operation.Cmp(_T("^")) == 0)
		return Dataset2D::DoUnsyncrhonizedExponentiation(second, first);

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
	else if (operation.Cmp(_T("^")) == 0)
		return first.ApplyPower(second);

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
		return fmod(second, first);
	else if (operation.Cmp(_T("^")) == 0)
		return pow(second, first);

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
//		first		= const wxString&
//		second		= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the result of the operation
//
//==========================================================================
wxString ExpressionTree::ApplyOperation(const wxString &operation,
	const wxString &first, const wxString &second) const
{
	/*if (operation.Cmp(_T("+")) == 0)
		return second + first;
	else if (operation.Cmp(_T("-")) == 0)
		return second - first;
	else */if (operation.Cmp(_T("*")) == 0)
		return StringMultiply(first, second);
	return second + operation + first;

/*	assert(false);
	return "";*/
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ApplyOperation
//
// Description:		Applies the specified operation to the specified operands.
//
// Input Arguments:
//		operation	= const wxString& describing the function to apply
//		first		= const wxString&
//		second		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the result of the operation
//
//==========================================================================
wxString ExpressionTree::ApplyOperation(const wxString &operation,
	const wxString &first, const double &second) const
{
	if (operation.Cmp(_T("+")) == 0)
		return StringAdd(first, second);
	else if (operation.Cmp(_T("-")) == 0)
		return StringSubtract(first, second);
	else if (operation.Cmp(_T("*")) == 0)
		return StringMultiply(first, second);

	assert(false);
	return "";
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
//		second		= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the result of the operation
//
//==========================================================================
wxString ExpressionTree::ApplyOperation(const wxString &operation,
	const double &first, const wxString &second) const
{
	if (operation.Cmp(_T("+")) == 0)
		return StringAdd(first, second);
	else if (operation.Cmp(_T("-")) == 0)
		return StringSubtract(first, second);
	else if (operation.Cmp(_T("*")) == 0)
		return StringMultiply(first, second);
	else if (operation.Cmp(_T("/")) == 0)
		return StringDivide(first, second);
	else if (operation.Cmp(_T("^")) == 0)
		return StringPower(first, second);

	assert(false);
	return "";
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

	// TODO:  Handle multiple args here

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
		return EvaluateUnaryOperator(operation, doubleStack, setStack, useDoubleStack, errorString);
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
// Function:		EvaluateUnaryOperator
//
// Description:		Evaluates the operator specified.  The only unary operator
//					we recognize is minus (negation).
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
bool ExpressionTree::EvaluateUnaryOperator(const wxString &operation, std::stack<double> &doubleStack,
	std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	if (operation.Cmp(_T("-")) != 0)
	{
		errorString = _T("Attempting to apply operator without two operands!");
		return false;
	}

	double value;
	Dataset2D dataset;
	if (PopFromStack(doubleStack, setStack, useDoubleStack, value, dataset))
		PushToStack(ApplyOperation(_T("*"), -1.0, value), doubleStack, useDoubleStack);
	else
		PushToStack(ApplyOperation(_T("*"), -1.0, dataset), setStack, useDoubleStack);

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
//		stringStack		= std::stack<wxString>&
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
	std::stack<wxString> &stringStack, std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	double value1, value2;
	wxString string1, string2;

	if (useDoubleStack.size() < 2)
		return EvaluateUnaryOperator(operation, doubleStack, stringStack, useDoubleStack, errorString);
	else if (PopFromStack(doubleStack, stringStack, useDoubleStack, string1, value1))
	{
		if (PopFromStack(doubleStack, stringStack, useDoubleStack, string2, value2))
			PushToStack(ApplyOperation(operation, value1, value2), doubleStack, useDoubleStack);
		else
			PushToStack(ApplyOperation(operation, value1, string2), stringStack, useDoubleStack);
	}
	else if (PopFromStack(doubleStack, stringStack, useDoubleStack, string2, value2))
		PushToStack(ApplyOperation(operation, string1, value2), stringStack, useDoubleStack);
	else
		PushToStack(ApplyOperation(operation, string1, string2), stringStack, useDoubleStack);

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateUnaryOperator
//
// Description:		Evaluates the operator specified.  The only unary operator
//					we recognize is minus (negation).
//
// Input Arguments:
//		operator		= const wxString& describing the function to apply
//		doubleStack		= std::stack<double>&
//		stringStack		= std::stack<wxString>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString		= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateUnaryOperator(const wxString &operation, std::stack<double> &doubleStack,
	std::stack<wxString> &stringStack, std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	if (operation.Cmp(_T("-")) != 0)
	{
		errorString = _T("Attempting to apply operator without two operands!");
		return false;
	}

	double value;
	wxString string;
	if (PopFromStack(doubleStack, stringStack, useDoubleStack, string, value))
		PushToStack(ApplyOperation(_T("*"), -1.0, value), doubleStack, useDoubleStack);
	else
		PushToStack(ApplyOperation(_T("*"), -1.0, string), stringStack, useDoubleStack);

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
	if (!list)
	{
		errorString = _T("No datasets available!");
		return false;
	}

	bool unaryMinus = dataset[0] == '-';

	unsigned long set;
	if (!dataset.Mid(1 + (int)unaryMinus, dataset.Len() - 2 - (int)unaryMinus).ToULong(&set))
	{
		errorString = _T("Could not convert '") + dataset + _T("' to set ID.");
		return false;
	}
	else if (set > (unsigned int)(*list).GetCount())
	{
		errorString = wxString::Format("Set ID %lu is not a valid set ID.", set);
		return false;
	}

	if (unaryMinus)
		PushToStack(GetSetFromList(set) * -1.0, setStack, useDoubleStack);
	else
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
	else if (operation.Cmp(_T("^")) == 0)
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
//		next			= const wxString&
//		doubleStack		= std::stack<double>&
//		setStack		= std::stack<Dataset2D>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString		= wxString&
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
	else if (NextIsNumber(next))
		return EvaluateNumber(next, doubleStack, useDoubleStack, errorString);
	else if (NextIsDataset(next))
		return EvaluateDataset(next, setStack, useDoubleStack, errorString);
	else if(NextIsOperator(next))
		return EvaluateOperator(next, doubleStack, setStack, useDoubleStack, errorString);
	else
		errorString = _T("Unable to evaluate '") + next + _T("'.");

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
//		next			= const wxString&
//		doubleStack		= std::stack<double>&
//		stringStack		= std::stack<wxString>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString		= wxString&
//
// Return Value:
//		bool, true for valid operation, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateNext(const wxString &next, std::stack<double> &doubleStack,
		std::stack<wxString> &stringStack, std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	if (NextIsNumber(next))
		return EvaluateNumber(next, doubleStack, useDoubleStack, errorString);
	else if(NextIsOperator(next))
		return EvaluateOperator(next, doubleStack, stringStack, useDoubleStack, errorString);
	else if (NextIsS(next))
	{
		PushToStack(next, stringStack, useDoubleStack);
		return true;
	}
	else
		errorString = _T("Unable to evaluate '") + next + _T("'.");

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		BeginningMatchesNoCase
//
// Description:		Determines if the target string matches the beginning of
//					string s.  Populates the length argument if provided and
//					if a match is found.
//
// Input Arguments:
//		s		= const wxString&
//		target	= const wxString&
//
// Output Arguments:
//		length	= unsigned int* (optional)
//
// Return Value:
//		bool, true for match
//
//==========================================================================
bool ExpressionTree::BeginningMatchesNoCase(const wxString &s, const wxString &target,
	unsigned int *length)
{
	if (s.Len() < target.Len())
		return false;

	if (s.Mid(0, target.Len()).CmpNoCase(target) != 0)
		return false;

	if (length)
		*length = target.Len();

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		StringAdd
//
// Description:		Performs arithmatic on the arguments, returns a string.
//
// Input Arguments:
//		first	= const wxString&
//		second	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString ExpressionTree::StringAdd(const wxString &first, const double &second) const
{
	return wxString::Format("%0.*f+%s",
		PlotMath::GetPrecision(second, printfPrecision), second, first.c_str());
}

//==========================================================================
// Class:			ExpressionTree
// Function:		StringAdd
//
// Description:		Performs arithmatic on the arguments, returns a string.
//
// Input Arguments:
//		first	= const double&
//		second	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString ExpressionTree::StringAdd(const double &first, const wxString &second) const
{
	return wxString::Format("%s+%0.*f", second.c_str(),
		PlotMath::GetPrecision(first, printfPrecision), first);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		StringSubtract
//
// Description:		Performs arithmatic on the arguments, returns a string.
//
// Input Arguments:
//		first	= const wxString&
//		second	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString ExpressionTree::StringSubtract(const wxString &first, const double &second) const
{
	return wxString::Format("%0.*f-%s",
		PlotMath::GetPrecision(second, printfPrecision), second, first.c_str());
}

//==========================================================================
// Class:			ExpressionTree
// Function:		StringSubtract
//
// Description:		Performs arithmatic on the arguments, returns a string.
//
// Input Arguments:
//		first	= const double&
//		second	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString ExpressionTree::StringSubtract(const double &first, const wxString &second) const
{
	return wxString::Format("%s-%0.*f", second.c_str(),
		PlotMath::GetPrecision(first, printfPrecision), first);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		StringMultiply
//
// Description:		Performs arithmatic on the arguments, returns a string.
//
// Input Arguments:
//		first	= const wxString&
//		second	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString ExpressionTree::StringMultiply(const wxString &first, const double &second) const
{
	std::vector<std::pair<int, double> > terms(FindPowersAndCoefficients(BreakApartTerms(first)));
	unsigned int i;
	wxString expression;
	for (i = 0; i < terms.size(); i++)
		AddToExpressionString(expression, terms[i].second * second, terms[i].first);

	return expression;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		StringMultiply
//
// Description:		Performs arithmatic on the arguments, returns a string.
//
// Input Arguments:
//		first	= const wxString&
//		second	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString ExpressionTree::StringMultiply(const wxString &first, const wxString &second) const
{
	std::vector<std::pair<int, double> > firstTerms(FindPowersAndCoefficients(BreakApartTerms(first)));
	std::vector<std::pair<int, double> > secondTerms(FindPowersAndCoefficients(BreakApartTerms(second)));
	std::vector<std::pair<int, double> > terms;
	unsigned int i, j;
	for (i = 0; i < firstTerms.size(); i++)
	{
		for (j = 0; j < secondTerms.size(); j++)
			terms.push_back(std::pair<int, double>(firstTerms[i].first + secondTerms[j].first, firstTerms[i].second * secondTerms[j].second));
	}

	wxString expression;
	for (i = 0; i < terms.size(); i++)
		AddToExpressionString(expression, terms[i].second, terms[i].first);

	return expression;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		StringMultiply
//
// Description:		Performs arithmatic on the arguments, returns a string.
//
// Input Arguments:
//		first	= const double&
//		second	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString ExpressionTree::StringMultiply(const double &first, const wxString &second) const
{
	return StringMultiply(second, first);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		StringDivide
//
// Description:		Performs arithmatic on the arguments, returns a string.
//
// Input Arguments:
//		first	= const double&
//		second	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString ExpressionTree::StringDivide(const double &first, const wxString &second) const
{
	return StringMultiply(second, 1.0 / first);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		StringPower
//
// Description:		Performs arithmatic on the arguments, returns a string.
//					For positive powers, expand and do the multiplication.
//					For negative powers (i.e. z-domain stuff), add them to the
//					string.  Assumes exponent is an integer.
//
// Input Arguments:
//		first	= const double&
//		second	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString ExpressionTree::StringPower(const double &first, const wxString &second) const
{
	if (first < 0.0)
		return wxString::Format("%s^%i", second.c_str(), (int)first);

	wxString result(second);
	unsigned int i;
	for (i = 1; i < (unsigned int)first; i++)
		result = StringMultiply(result, second);

	return result;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		BreakApartTerms
//
// Description:		Breaks apart all the terms in the string expression.  Be
//					wary of negative signs preceded by another operator!
//
// Input Arguments:
//		s	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString
//
//==========================================================================
wxArrayString ExpressionTree::BreakApartTerms(const wxString &s)
{
	wxArrayString terms;
	unsigned int start(0), end(0);
	while (end != (unsigned int)wxNOT_FOUND)
	{
		end = FindEndOfNextTerm(s, start);

		if (start > 0 && s.Mid(start - 1, 1).Cmp(_T("-")) == 0)
		{
			if (end != (unsigned int)wxNOT_FOUND)
				terms.Add(s.Mid(start - 1, end + 1));
			else
				terms.Add(s.Mid(start - 1));
		}
		else
			terms.Add(s.Mid(start, end));

		start += end + 1;
	}

	return terms;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		FindEndOfNextTerm
//
// Description:		Finds the end of the next term in the string.
//
// Input Arguments:
//		s		= const wxString&
//		start	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
unsigned int ExpressionTree::FindEndOfNextTerm(const wxString &s, const unsigned int &start)
{
	unsigned int end, plusEnd, minusEnd;

	plusEnd = s.Mid(start).Find('+');
	minusEnd = s.Mid(start).Find('-');

	if (minusEnd < plusEnd && start + minusEnd > 0 && NextIsOperator(s[start + minusEnd - 1]))
	{
		unsigned int nextMinus = s.Mid(start + minusEnd + 1).Find('-');
		if (nextMinus != (unsigned int)wxNOT_FOUND)
			minusEnd += nextMinus + 1;
		else
			minusEnd = nextMinus;
	}
	end = std::min(plusEnd, minusEnd);

	if (end != (unsigned int)wxNOT_FOUND && NextIsOperator(s.Mid(start + end - 1)))
	{
		plusEnd = s.Mid(start + end).Find('+');
		minusEnd = s.Mid(start + end).Find('-');
		end += std::min(plusEnd, minusEnd);
	}

	return end;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		FindPowersAndCoefficients
//
// Description:		Breaks a (previously separated) set of terms into a coefficient
//					and a power of the algebraic variable.
//
// Input Arguments:
//		terms	= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		std::vector<std::pair<int, double> >
//
//==========================================================================
std::vector<std::pair<int, double> > ExpressionTree::FindPowersAndCoefficients(const wxArrayString &terms)
{
	std::vector<std::pair<int, double> > processedTerms;
	unsigned int i, start, end;
	int count;
	double temp, coefficient;
	for (i = 0; i < terms.Count(); i++)
	{
		count = 0;
		start = 0;
		end = 0;
		coefficient = 1.0;
		while (end != (unsigned int)wxNOT_FOUND)
		{
			end = terms[i].Mid(start).Find('*');
			if (terms[i].Mid(start, end).ToDouble(&temp))
				coefficient = temp;
			else
			{
				if (terms[i][0] == '-' && coefficient == 1.0)
				{
					coefficient = -1.0;
					start++;
					if (end != (unsigned int)wxNOT_FOUND)
						end++;
				}

				count += GetTermPower(terms[i].Mid(start), start, end);
			}
			start += end + 1;
		}

		processedTerms.push_back(std::pair<int, double>(count, coefficient));
	}

	return processedTerms;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		GetTermPower
//
// Description:		Returns the value of the power for the specified term
//					(power of s or z).
//
// Input Arguments:
//		s		= const wxString&
//
// Output Arguments:
//		start	= unsigned int&
//		end		= unsigned int&
//
// Return Value:
//		int
//
//==========================================================================
int ExpressionTree::GetTermPower(const wxString &s, unsigned int &start, unsigned int &end)
{
	long power;
	if (s[0] == 's' || s[0] == 'z')
	{
		power = s.Find('^');
		if (power == wxNOT_FOUND)
			return 1;
		else
		{
			start += power + 1;
			end = s.Find('*');
			if (s.Mid(power + 1, end).ToLong(&power))
				return power;
		}
	}

	return 0;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		AddToExpressionString
//
// Description:		Adds the next term to the string.  Handles signed terms,
//					cleans up for terms with coefficient == 1.0, etc.
//
// Input Arguments:
//		coefficient	= const double&
//		power		= const int&
//
// Output Arguments:
//		expression	= wxString&
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::AddToExpressionString(wxString &expression,
	const double &coefficient, const int &power) const
{
	if (coefficient == 1.0 && power != 0)
	{
		if (!expression.IsEmpty())
			expression.Append(_T("+"));
		if (power == 1)
				expression.Append(_T("s"));
		else
			expression.Append(wxString::Format("s^%i", power));
	}
	else if (expression.IsEmpty())
	{
		if (power == 0)
			expression.Printf("%0.*f", PlotMath::GetPrecision(coefficient, printfPrecision), coefficient);
		else if (power == 1)
			expression.Printf("%0.*f*s", PlotMath::GetPrecision(coefficient, printfPrecision), coefficient);
		else
			expression.Printf("%0.*f*s^%i", PlotMath::GetPrecision(coefficient, printfPrecision), coefficient, power);
	}
	else
	{
		if (power == 0)
			expression.Append(wxString::Format("%+0.*f",
				PlotMath::GetPrecision(coefficient, printfPrecision), coefficient));
		else if (power == 1)
			expression.Append(wxString::Format("%+0.*f*s",
				PlotMath::GetPrecision(coefficient, printfPrecision), coefficient));
		else
			expression.Append(wxString::Format("%+0.*f*s^%i",
				PlotMath::GetPrecision(coefficient, printfPrecision), coefficient, power));
	}
}
