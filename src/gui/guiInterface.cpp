/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  guiInterface.cpp
// Date:  12/9/2016
// Auth:  K. Loux
// Desc:  Manages interactions between plot grid and plot renderings, handles
//        context menus, etc.

// Local headers
#include "lp2d/gui/guiInterface.h"
#include "lp2d/parser/dataFile.h"
#include "lp2d/utilities/arrayStringCompare.h"
#include "lp2d/utilities/math/expressionTree.h"

// wxWidgets headers
#include <wx/wx.h>
#include <wx/file.h>

// Standard C++ headers
#include <map>

namespace LibPlot2D
{

//==========================================================================
// Class:			GuiInterface
// Function:		LoadFiles
//
// Description:		Method for loading a multiple files.
//
// Input Arguments:
//		fileList	= const wxArrayString&
//		parent		= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		true for files successfully loaded, false otherwise
//
//==========================================================================
bool GuiInterface::LoadFiles(const wxArrayString &fileList, wxWindow* parent)
{
	unsigned int i, j;
	std::vector<bool> loaded(fileList.size());
	std::vector<DataFile*> files(fileList.size());
	typedef std::map<wxArrayString, DataFile::SelectionData,
		ArrayStringCompare> SelectionMap;
	SelectionMap selectionInfoMap;
	SelectionMap::const_iterator it;
	DataFile::SelectionData selectionInfo;
	bool atLeastOneFileLoaded(false);
	for (i = 0; i < fileList.Count(); i++)
	{
		files[i] = GetDataFile(fileList[i]);
		files[i]->Initialize();
		it = selectionInfoMap.find(files[i]->GetAllDescriptions());
		if (it == selectionInfoMap.end())
		{
			if (files[i]->DescriptionsMatch(lastDescriptions))
				selectionInfo = lastSelectionInfo;
			else
				selectionInfo.selections.Clear();

			if (plotList.GetCount() == 0 && files[i]->GetAllDescriptions().Count() == 2)// 2 descriptions because X column counts as one
			{
				// No existing data (no need for "Remove Existing Curves?") and
				// only one curve in file - no need to ask anything of user.
				selectionInfo.selections.Clear();// In case lastSelectionInfo was used to initialize selectionInfo
				selectionInfo.selections.Add(0);
				selectionInfoMap[files[i]->GetAllDescriptions()] = selectionInfo;
			}
			else
			{
				files[i]->GetSelectionsFromUser(selectionInfo, parent);
				if (selectionInfo.selections.Count() < 1)
				{
					for (j = 0; j <= i; j++)
						delete files[j];
					return false;
				}
				selectionInfoMap[files[i]->GetAllDescriptions()] = selectionInfo;
			}
		}
		else
			selectionInfo = it->second;

		loaded[i] = selectionInfo.selections.Count() > 0 && files[i]->Load(selectionInfo);
		atLeastOneFileLoaded = atLeastOneFileLoaded || loaded[i];
	}

	if (!atLeastOneFileLoaded)
	{
		for (i = 0; i < files.size(); i++)
			delete files[i];
		return false;
	}

	if (selectionInfo.removeExisting)
		ClearAllCurves();

	wxString curveName;
	for (i = 0; i < fileList.Count(); i++)
	{
		if (!loaded[i])
			continue;

		for (j = 0; j < files[i]->GetDataCount(); j++)
		{
			if (fileList.Count() > 1)
				curveName = files[i]->GetDescription(j + 1) + _T(" : ") + ExtractFileNameFromPath(fileList[i]);
			else
				curveName = files[i]->GetDescription(j + 1);
			AddCurve(files[i]->GetDataset(j), curveName);
		}
	}

	if (fileList.Count() > 1)
		SetTitle(_T("Multiple Files - ") + DataPlotterApp::dataPlotterTitle);
	else
		SetTitle(ExtractFileNameFromPath(fileList[0]) + _T( "- ") + DataPlotterApp::dataPlotterTitle);
	genericXAxisLabel = files[0]->GetDescription(0);
	SetXDataLabel(genericXAxisLabel);
	plotArea->SaveCurrentZoom();

	// Because fileList is a reference (and may refer to lastFilesLoaded), we need to check for self-assignment
	if (&fileList != &lastFilesLoaded)
		lastFilesLoaded = fileList;
	lastSelectionInfo = selectionInfo;
	lastDescriptions = files[files.size() - 1]->GetAllDescriptions();

	for (i = 0; i < files.size(); i++)
		delete files[i];

	return true;
}

//==========================================================================
// Class:			GuiInterface
// Function:		LoadText
//
// Description:		Public method for loading a single object from text.
//					This writes the text to a temporary file, then tries to
//					open it using normal methods.
//
// Input Arguments:
//		textData	= const wxString&
//		parent		= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool GuiInterface::LoadText(const wxString &textData, wxWindow* parent)
{
	wxString tempFileName(GenerateTemporaryFileName());
	while (wxFile::Exists(tempFileName))
		tempFileName = GenerateTemporaryFileName();

	std::ofstream tempFile(tempFileName.mb_str(), std::ios::out);
	if (!tempFile.good() || !tempFile.is_open())
	{
		tempFile.close();
		if (remove(tempFileName.mb_str()) != 0)
			wxMessageBox(_T("Error deleting temporary file '") + tempFileName + _T("'."),
			_T("Could Not Delete File"), wxICON_ERROR, parent);
		return false;
	}

	tempFile << textData;
	tempFile.close();

	bool fileLoaded = LoadFiles(wxArrayString(1, &tempFileName));
	if (remove(tempFileName.mb_str()) != 0)
		wxMessageBox(_T("Error deleting temporary file '") + tempFileName + _T("'."),
		_T("Could Not Delete File"), wxICON_ERROR, this);

	if (fileLoaded)
		SetTitle(_T("Clipboard Data - ") + DataPlotterApp::dataPlotterTitle);

	return fileLoaded;
}

//==========================================================================
// Class:			GuiInterface
// Function:		ReloadData
//
// Description:		Reloads the data from the last set of files loaded.
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
void GuiInterface::ReloadData()
{
	if (lastFilesLoaded.IsEmpty())
		return;

	LoadFiles(lastFilesLoaded);
}

//==========================================================================
// Class:			GuiInterface
// Function:		GenerateTemporaryFileName
//
// Description:		Generates a random string of characters to use as a file
//					name (always ends with .tmp).
//
// Input Arguments:
//		length	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString GuiInterface::GenerateTemporaryFileName(const unsigned int &length) const
{
	wxString name;
	unsigned int i;
	for (i = 0; i < length; i++)
		name.Append((char)((rand() % 52) + 65));

	// Remove illegal characters
#ifdef __WXMSW__
	name.Replace(_T("?"), _T(""));
	name.Replace(_T("\""), _T(""));
	name.Replace(_T("\\"), _T(""));
	name.Replace(_T("/"), _T(""));
	name.Replace(_T(":"), _T(""));
	name.Replace(_T("*"), _T(""));
	name.Replace(_T("|"), _T(""));
	name.Replace(_T("<"), _T(""));
	name.Replace(_T(">"), _T(""));
#else
	name.Replace(_T("/"), _T(""));
#endif

	name.Append(_T(".tmp"));

	return name;
}

//==========================================================================
// Class:			GuiInterface
// Function:		ClearAllCurves
//
// Description:		Removes all curves from the plot.
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
void GuiInterface::ClearAllCurves()
{
	while (plotList.GetCount() > 0)
		RemoveCurve(0);
}

//==========================================================================
// Class:			GuiInterface
// Function:		AddCurve
//
// Description:		Adds a new dataset to the plot, created by operating on
//					existing datasets.
//
// Input Arguments:
//		mathString	= wxString describing the desired math operations
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::AddCurve(wxString mathString)
{
	// String will be empty if the user cancelled
	if (mathString.IsEmpty())
		return;

	// Parse string and determine what the new dataset should look like
	ExpressionTree expression(&plotList);
	Dataset2D *mathChannel = new Dataset2D;

	double xAxisFactor;
	GetXAxisScalingFactor(xAxisFactor);// No warning here:  it's only an issue for FFTs and filters; warning are generated then

	wxString errors = expression.Solve(mathString, *mathChannel, xAxisFactor);

	if (!errors.IsEmpty())
	{
		wxMessageBox(_T("Could not solve expression:\n\n") + errors, _T("Error Solving Expression"), wxICON_ERROR, this);
		delete mathChannel;

		DisplayMathChannelDialog(mathString);
		return;
	}

	AddCurve(mathChannel, mathString.Upper());
}

//==========================================================================
// Class:			GuiInterface
// Function:		AddCurve
//
// Description:		Adds an existing dataset to the plot.
//
// Input Arguments:
//		data	= Dataset2D* to add
//		name	= wxString specifying the label for the curve
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::AddCurve(Dataset2D *data, wxString name)
{
	plotList.Add(data);

	optionsGrid->BeginBatch();
	if (optionsGrid->GetNumberRows() == 0)
		AddTimeRowToGrid();
	unsigned int index = AddDataRowToGrid(name);
	optionsGrid->EndBatch();

	optionsGrid->Scroll(-1, optionsGrid->GetNumberRows());

	plotArea->AddCurve(*data);
	UpdateCurveProperties(index - 1, GetNextColor(index), true, false);

	UpdateCurveQuality();
	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			GuiInterface
// Function:		RemoveCurve
//
// Description:		Removes a curve from the plot.
//
// Input Arguments:
//		i	= const unsigned int& specifying curve to remove
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::RemoveCurve(const unsigned int &i)
{
	optionsGrid->DeleteRows(i + 1);

	if (optionsGrid->GetNumberRows() == 1)
		optionsGrid->DeleteRows();

	optionsGrid->AutoSizeColumns();

	plotArea->RemoveCurve(i);
	plotList.Remove(i);

	UpdateCurveQuality();
	UpdateLegend();
}

//==========================================================================
// Class:			GuiInterface
// Function:		UpdateCursorValues
//
// Description:		Updates the values for the cursors and their differences
//					in the options grid.
//
// Input Arguments:
//		leftVisible		= const bool& indicating whether or not the left
//						  cursor is visible
//		rightVisible	= const bool& indicating whether or not the right
//						  cursor is visible
//		leftValue		= const double& giving the value of the left cursor
//		rightValue		= const double& giving the value of the right cursor
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue)
{
	if (optionsGrid == NULL)
		return;

	// TODO:  This would be nicer with smart precision so we show enough digits but not too many

	// For each curve, update the cursor values
	int i;
	bool showXDifference(false);
	for (i = 1; i < optionsGrid->GetNumberRows(); i++)
	{
		UpdateSingleCursorValue(i, leftValue, colLeftCursor, leftVisible);
		UpdateSingleCursorValue(i, rightValue, colRightCursor, rightVisible);

		if (leftVisible && rightVisible)
		{
			double left, right;
			if (plotList[i - 1]->GetYAt(leftValue, left) && plotList[i - 1]->GetYAt(rightValue, right))
			{
				optionsGrid->SetCellValue(i, colDifference, wxString::Format("%f", right - left));
				showXDifference = true;
			}
			else
				optionsGrid->SetCellValue(i, colDifference, wxEmptyString);
		}
	}

	if (showXDifference)
		optionsGrid->SetCellValue(0, colDifference, wxString::Format("%f", rightValue - leftValue));
}

//==========================================================================
// Class:			GuiInterface
// Function:		UpdateSingleCursorValue
//
// Description:		Updates a single cursor value.
//
// Input Arguments:
//		row			= const unsigned int& specifying the grid row
//		value		= const double& specifying the value to populate
//		column		= const unsigned int& specifying which grid column to populate
//		isVisible	= const bool& indicating whether or not the cursor is visible
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::UpdateSingleCursorValue(const unsigned int &row,
	double value, const unsigned int &column, const bool &isVisible)
{
	if (isVisible)
	{
		optionsGrid->SetCellValue(0, column, wxString::Format("%f", value));

		bool exact;
		double valueOut;
		if (plotList[row - 1]->GetYAt(value, valueOut, &exact))
		{
			if (exact)
				optionsGrid->SetCellValue(row, column, _T("*") + wxString::Format("%f", valueOut));
			else
				optionsGrid->SetCellValue(row, column, wxString::Format("%f", valueOut));
		}
		else
			optionsGrid->SetCellValue(row, column, wxEmptyString);
	}
	else
	{
		optionsGrid->SetCellValue(0, column, wxEmptyString);
		optionsGrid->SetCellValue(row, column, wxEmptyString);

		// The difference column only exists if both cursors are visible
		optionsGrid->SetCellValue(0, colDifference, wxEmptyString);
		optionsGrid->SetCellValue(row, colDifference, wxEmptyString);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		GetDataFile
//
// Description:		Determines the correct DataFile object to use for the
//					specified file, and returns a pointer to an instance of that
//					object.
//
// Input Arguments:
//		fileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		DataFile*
//
//==========================================================================
DataFile* GuiInterface::GetDataFile(const wxString &fileName)
{
	if (BaumullerFile::IsType(fileName))
		return new BaumullerFile(fileName);
	else if (KollmorgenFile::IsType(fileName))
		return new KollmorgenFile(fileName);
	else if (CustomFile::IsType(fileName))
		return new CustomFile(fileName);
	else if (CustomXMLFile::IsType(fileName))
		return new CustomXMLFile(fileName);

	// Don't even check - if we can't open it with any other types,
	// always try to open it with a generic type
	return new GenericFile(fileName);
}

}// namespace LibPlot2D
