/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  expressionTree.h
// Created:  5/6/2011
// Author:  K. Loux
// Description:  Handles user-specified mathematical operations on datasets.
// History:

#ifndef _EXPRESSION_TREE_H_
#define _EXPRESSION_TREE_H_

// Standard C++ headers
#include <queue>
#include <stack>

// Local headers
#include "utilities/managedList.h"
#include "utilities/dataset2D.h"

// wxWidgets forward declarations
class wxString;

class ExpressionTree
{
public:
	// Constructor
	ExpressionTree(const ManagedList<const Dataset2D> &_list);

	// Main solver method
	wxString Solve(wxString expression, Dataset2D &solvedData, const double &_xAxisFactor);

private:
	const ManagedList<const Dataset2D> &list;

	double xAxisFactor;

	std::queue<wxString> outputQueue;

	wxString ParseExpression(const wxString &expression);
	wxString EvaluateExpression(Dataset2D &results);

	void ProcessOperator(std::stack<wxString> &operatorStack, const wxString &s);
	void ProcessCloseParenthese(std::stack<wxString> &operatorStack);

	Dataset2D GetSetFromList(const unsigned int &i) const;

	bool NextIsNumber(const wxString &s, unsigned int *stop = NULL) const;
	bool NextIsDataset(const wxString &s, unsigned int *stop = NULL) const;
	bool NextIsFunction(const wxString &s, unsigned int *stop = NULL) const;
	bool NextIsOperator(const wxString &s, unsigned int *stop = NULL) const;

	bool IsLeftAssociative(const wxChar &c) const;
	bool OperatorShift(const wxString &stackString, const wxString &newString) const;

	void PopStackToQueue(std::stack<wxString> &stack);
	bool EmptyStackToQueue(std::stack<wxString> &stack);
	unsigned int GetPrecedence(const wxString &s) const;

	void PushToStack(const double &value, std::stack<double> &doubleStack,
		std::stack<bool> &useDoubleStack) const;
	void PushToStack(const Dataset2D &dataset, std::stack<Dataset2D> &setStack,
		std::stack<bool> &useDoubleStack) const;
	bool PopFromStack(std::stack<double> &doubleStack, std::stack<Dataset2D> &setStack,
		std::stack<bool> &useDoubleStack, double &value, Dataset2D &dataset) const;

	Dataset2D ApplyFunction(const wxString &function, const Dataset2D &set) const;
	Dataset2D ApplyOperation(const wxString &operation, const Dataset2D &first, const Dataset2D &second) const;
	Dataset2D ApplyOperation(const wxString &operation, const Dataset2D &first, const double &second) const;
	Dataset2D ApplyOperation(const wxString &operation, const double &first, const Dataset2D &second) const;
	double ApplyOperation(const wxString &operation, const double &first, const double &second) const;

	bool EvaluateNext(const wxString &next, std::stack<double> &doubleStack,
		std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateFunction(const wxString &function, std::stack<double> &doubleStack,
		std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateOperator(const wxString &operation, std::stack<double> &doubleStack,
		std::stack<Dataset2D> &setStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateNumber(const wxString &number, std::stack<double> &doubleStack,
		std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateDataset(const wxString &dataset, std::stack<Dataset2D> &setStack,
		std::stack<bool> &useDoubleStack, wxString &errorString) const;

	bool SetOperatorValid(const wxString &operation, const bool &leftOperandIsDouble) const;
	bool ParenthesesBalanced(const wxString &expression) const;
	bool BeginningMatchesNoCase(const wxString &s, const wxString &target, unsigned int *length = NULL) const;
};

#endif// _EXPRESSION_TREE_H_