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
#include "lp2d/gui/createSignalDialog.h"
#include "lp2d/gui/fftDialog.h"
#include "lp2d/gui/filterDialog.h"
#include "lp2d/gui/frfDialog.h"
#include "lp2d/gui/rangeLimitsDialog.h"
#include "lp2d/parser/dataFile.h"
#include "lp2d/parser/baumullerFile.h"
#include "lp2d/parser/customFile.h"
#include "lp2d/parser/customXMLFile.h"
#include "lp2d/parser/genericFile.h"
#include "lp2d/parser/KollmorgenFile.h"
#include "lp2d/utilities/arrayStringCompare.h"
#include "lp2d/utilities/math/expressionTree.h"
#include "lp2d/utilities/math/plotMath.h"
#include "lp2d/utilities/signals/derivative.h"
#include "lp2d/utilities/signals/rms.h"
#include "lp2d/utilities/signals/integral.h"
#include "lp2d/utilities/signals/fft.h"
#include "lp2d/utilities/signals/filter.h"
#include "lp2d/utilities/guiUtilities.h"
#include "lp2d/libPlot2D.h"

// wxWidgets headers
#include <wx/wx.h>
#include <wx/file.h>

// Standard C++ headers
#include <map>

namespace LibPlot2D
{

GuiInterface::GuiInterface(wxFrame* owner) : owner(owner)
{
}

//==========================================================================
// Class:			GuiInterface
// Function:		LoadFiles
//
// Description:		Method for loading a multiple files.
//
// Input Arguments:
//		fileList	= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		true for files successfully loaded, false otherwise
//
//==========================================================================
bool GuiInterface::LoadFiles(const wxArrayString &fileList)
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
				files[i]->GetSelectionsFromUser(selectionInfo, owner);
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
				curveName = files[i]->GetDescription(j + 1) + _T(" : ") + GuiUtilities::ExtractFileNameFromPath(fileList[i]);
			else
				curveName = files[i]->GetDescription(j + 1);
			AddCurve(files[i]->GetDataset(j), curveName);
		}
	}

	if (owner)
	{
		if (fileList.Count() > 1)
			owner->SetTitle(_T("Multiple Files - ") + applicationTitle);
		else
			owner->SetTitle(GuiUtilities::ExtractFileNameFromPath(fileList[0]) + _T("- ") + applicationTitle);
	}

	genericXAxisLabel = files[0]->GetDescription(0);
	SetXDataLabel(genericXAxisLabel);
	renderer->SaveCurrentZoom();

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
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool GuiInterface::LoadText(const wxString &textData)
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
			_T("Could Not Delete File"), wxICON_ERROR, owner);
		return false;
	}

	tempFile << textData;
	tempFile.close();

	bool fileLoaded = LoadFiles(wxArrayString(1, &tempFileName));
	if (remove(tempFileName.mb_str()) != 0)
		wxMessageBox(_T("Error deleting temporary file '") + tempFileName + _T("'."),
		_T("Could Not Delete File"), wxICON_ERROR, owner);

	if (fileLoaded && owner)
		owner->SetTitle(_T("Clipboard Data - ") + applicationTitle);

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
		wxMessageBox(_T("Could not solve expression:\n\n") + errors, _T("Error Solving Expression"), wxICON_ERROR, owner);
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

	grid->BeginBatch();
	if (grid->GetNumberRows() == 0)
		grid->AddTimeRow();
	unsigned int index(grid->AddDataRow(name));
	grid->EndBatch();

	grid->Scroll(-1, grid->GetNumberRows());

	renderer->AddCurve(*data);
	UpdateCurveProperties(index - 1, grid->GetNextColor(index), true, false);

	UpdateCurveQuality();
	renderer->UpdateDisplay();
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
	grid->DeleteRows(i + 1);

	if (grid->GetNumberRows() == 1)
		grid->DeleteRows();

	grid->AutoSizeColumns();

	renderer->RemoveCurve(i);
	plotList.Remove(i);

	UpdateCurveQuality();
	UpdateLegend();
}

//==========================================================================
// Class:			GuiInterface
// Function:		RemoveCurves
//
// Description:		Removes a set of curves from the plot.
//
// Input Arguments:
//		curves	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::RemoveCurves(const wxArrayInt& curves)
{
	// TODO:  Fix this
	// Workaround for now
	int i;
	for (i = grid->GetNumberRows() - 1; i > 0; i--)
	{
		if (grid->IsInSelection(i, 0))
			RemoveCurve(i - 1);
	}

	renderer->UpdateDisplay();
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
	if (grid == nullptr)
		return;

	// TODO:  This would be nicer with smart precision so we show enough digits but not too many

	// For each curve, update the cursor values
	int i;
	bool showXDifference(false);
	for (i = 1; i < grid->GetNumberRows(); i++)
	{
		UpdateSingleCursorValue(i, leftValue, PlotListGrid::ColLeftCursor, leftVisible);
		UpdateSingleCursorValue(i, rightValue, PlotListGrid::ColRightCursor, rightVisible);

		if (leftVisible && rightVisible)
		{
			double left, right;
			if (plotList[i - 1]->GetYAt(leftValue, left) && plotList[i - 1]->GetYAt(rightValue, right))
			{
				grid->SetCellValue(i, PlotListGrid::ColDifference, wxString::Format("%f", right - left));
				showXDifference = true;
			}
			else
				grid->SetCellValue(i, PlotListGrid::ColDifference, wxEmptyString);
		}
	}

	if (showXDifference)
		grid->SetCellValue(0, PlotListGrid::ColDifference, wxString::Format("%f", rightValue - leftValue));
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
		grid->SetCellValue(0, column, wxString::Format("%f", value));

		bool exact;
		double valueOut;
		if (plotList[row - 1]->GetYAt(value, valueOut, &exact))
		{
			if (exact)
				grid->SetCellValue(row, column, _T("*") + wxString::Format("%f", valueOut));
			else
				grid->SetCellValue(row, column, wxString::Format("%f", valueOut));
		}
		else
			grid->SetCellValue(row, column, wxEmptyString);
	}
	else
	{
		grid->SetCellValue(0, column, wxEmptyString);
		grid->SetCellValue(row, column, wxEmptyString);

		// The difference column only exists if both cursors are visible
		grid->SetCellValue(0, PlotListGrid::ColDifference, wxEmptyString);
		grid->SetCellValue(row, PlotListGrid::ColDifference, wxEmptyString);
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

//==========================================================================
// Class:			GuiInterface
// Function:		ExportData
//
// Description:		Exports the data to file.
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
void GuiInterface::ExportData()
{
	wxString wildcard(_T("Comma Separated (*.csv)|*.csv"));
	wildcard.append("|Tab Delimited (*.txt)|*.txt");

	wxArrayString pathAndFileName = GuiUtilities::GetFileNameFromUser(owner, _T("Save As"),
		wxEmptyString, wxEmptyString, wildcard, wxFD_SAVE);

	if (pathAndFileName.Count() == 0)
		return;

	if (wxFile::Exists(pathAndFileName[0]))
	{
		if (wxMessageBox(_T("File exists.  Overwrite?"), _T("Overwrite File?"), wxYES_NO, owner) == wxNO)
			return;
	}

	wxString delimiter;
	if (pathAndFileName[0].Mid(pathAndFileName[0].Last('.')).CmpNoCase(_T(".txt")) == 0)
		delimiter = _T("\t");
	else
		delimiter = _T(",");// FIXME:  Need to handle descriptions containing commas so we don't have problems with import later on

	// Export both x and y data in case of asynchronous data or FFT, etc.
	std::ofstream outFile(pathAndFileName[0].mb_str(), std::ios::out);
	if (!outFile.is_open() || !outFile.good())
	{
		wxMessageBox(_T("Could not open '") + pathAndFileName[0] + _T("' for output."),
			_T("Error Writing File"), wxICON_ERROR, owner);
		return;
	}

	unsigned int i, j(0);
	wxString temp;
	for (i = 1; i < plotList.GetCount() + 1; i++)
	{
		if (grid->GetCellValue(i, PlotListGrid::ColName).Contains(_T("FFT")) ||
			grid->GetCellValue(i, PlotListGrid::ColName).Contains(_T("FRF")))
			outFile << _T("Frequency [Hz]") << delimiter;
		else
		{
			if (delimiter.Cmp(",") == 0)
			{
				temp = genericXAxisLabel;
				temp.Replace(",", ";");
				outFile << temp << delimiter;
			}
			else
				outFile << genericXAxisLabel << delimiter;
		}

		if (delimiter.Cmp(",") == 0)
		{
			temp = grid->GetCellValue(i, PlotListGrid::ColName);
			temp.Replace(",", ";");
			outFile << temp;
		}
		else
			outFile << grid->GetCellValue(i, PlotListGrid::ColName);

		if (i == plotList.GetCount())
			outFile << std::endl;
		else
			outFile << delimiter;
	}

	outFile.precision(14);

	bool done(false);
	while (!done)
	{
		done = true;
		for (i = 0; i < plotList.GetCount(); i++)
		{
			if (j < plotList[i]->GetNumberOfPoints())
				outFile << plotList[i]->GetXData(j) << delimiter << plotList[i]->GetYData(j);
			else
				outFile << delimiter;

			if (i == plotList.GetCount() - 1)
				outFile << std::endl;
			else
				outFile << delimiter;

			if (j + 1 < plotList[i]->GetNumberOfPoints())
				done = false;
		}

		j++;
	}

	outFile.close();
}

//==========================================================================
// Class:			GuiInterface
// Function:		GenerateFRF
//
// Description:		Generates a frequency response function.
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
void GuiInterface::GenerateFRF()
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, owner);

	wxArrayString descriptions;
	int i;
	for (i = 1; i < grid->GetNumberRows(); i++)
		descriptions.Add(grid->GetCellValue(i, 0));

	FRFDialog dialog(owner, descriptions);
	if (dialog.ShowModal() != wxID_OK)
		return;

	Dataset2D *amplitude = new Dataset2D, *phase = nullptr, *coherence = nullptr;

	if (dialog.GetComputePhase())
		phase = new Dataset2D;
	if (dialog.GetComputeCoherence())
		coherence = new Dataset2D;

	if (!PlotMath::XDataConsistentlySpaced(*plotList[dialog.GetInputIndex()]) ||
		!PlotMath::XDataConsistentlySpaced(*plotList[dialog.GetOutputIndex()]))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, owner);

	FastFourierTransform::ComputeFRF(*plotList[dialog.GetInputIndex()],
		*plotList[dialog.GetOutputIndex()], dialog.GetNumberOfAverages(),
		FastFourierTransform::WindowHann, dialog.GetModuloPhase(), *amplitude, phase, coherence);

	AddFFTCurves(factor, amplitude, phase, coherence, wxString::Format("[%u] to [%u]",
		dialog.GetInputIndex(), dialog.GetOutputIndex()));
}

//==========================================================================
// Class:			GuiInterface
// Function:		CreateSignal
//
// Description:		Displays dialog for creating various signals.
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
void GuiInterface::CreateSignal()
{
	double startTime(0.0);// [sec]
	double duration(10.0);// [sec]
	double sampleRate(100.0);// [Hz]

	double factor(1.0);
	if (plotList.GetCount() > 0)
	{
		GetXAxisScalingFactor(factor);

		// Use first curve to pull time and frequency information
		sampleRate = 1.0 / PlotMath::GetAverageXSpacing(*plotList[0]) * factor;
		startTime = plotList[0]->GetXData(0) / factor;
		duration = plotList[0]->GetXData(plotList[0]->GetNumberOfPoints() - 1) / factor - startTime;
	}

	CreateSignalDialog dialog(owner, startTime, duration, sampleRate);

	if (dialog.ShowModal() != wxID_OK)
		return;

	AddCurve(&dialog.GetSignal()->MultiplyXData(factor), dialog.GetSignalName());

	// Set time units if it hasn't been done already
	double dummy;
	if (!GetXAxisScalingFactor(dummy))
	{
		genericXAxisLabel = _T("Time [sec]");
		SetXDataLabel(genericXAxisLabel);
		//plotArea->SaveCurrentZoom();// TODO:  Is this necessary?
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		AddFFTCurves
//
// Description:		Adds the FFT curves to the plot list.
//
// Input Arguments:
//		xFactor	= const double& scaling factor to convert X units to Hz
//		amplitude	= Dataset2D*
//		phase		= Dataset2D*
//		coherence	= Dataset2D*
//		namePortion	= const wxString& identifying the input/output signals
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::AddFFTCurves(const double& xFactor,
	Dataset2D *amplitude, Dataset2D *phase,
	Dataset2D *coherence, const wxString &namePortion)
{
	AddCurve(&(amplitude->MultiplyXData(xFactor)), _T("FRF Amplitude, ") + namePortion + _T(", [dB]"));
	SetMarkerSize(grid->GetNumberRows() - 2, 0);

	if (phase)
	{
		AddCurve(&(phase->MultiplyXData(xFactor)), _T("FRF Phase, ") + namePortion + _T(", [deg]"));
		SetMarkerSize(grid->GetNumberRows() - 2, 0);
	}

	if (coherence)
	{
		AddCurve(&(coherence->MultiplyXData(xFactor)), _T("FRF Coherence, ") + namePortion + _T(", [-]"));
		SetMarkerSize(grid->GetNumberRows() - 2, 0);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		SetTimeUnits
//
// Description:		Available for the user to clarify the time units when we
//					are unable to determine them easily from the input file.
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
void GuiInterface::SetTimeUnits()
{
	double f;
	wxString units;

	// Check to see if we already have some confidence in our x-axis units
	if (GetXAxisScalingFactor(f, &units))
	{
		// Ask the user to confirm, since we don't think we need their help
		if (wxMessageBox(_T("Time units are being interpreted as ") + units +
			_T(", are you sure you want to change them?"), _T("Are You Sure?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, owner) == wxNO)
			return;
	}

	// Ask the user to specify the correct units
	wxString userUnits;
	userUnits = ::wxGetTextFromUser(_T("Specify time units (e.g. \"msec\" or \"minutes\")"),
		_T("Specify Units"), _T("seconds"), owner);

	// If the user cancelled, we will have a blank string
	if (userUnits.IsEmpty())
		return;

	// Check to make sure we understand what the user specified
	wxString currentLabel(grid->GetCellValue(0, PlotListGrid::ColName));
	genericXAxisLabel = _T("Time, [") + userUnits + _T("]");
	SetXDataLabel(genericXAxisLabel);
	if (!GetXAxisScalingFactor(f, &units))
	{
		// Set the label back to what it used to be and warn the user
		SetXDataLabel(currentLabel);
		wxMessageBox(_T("Could not understand units \"") + userUnits + _T("\"."), _T("Error Setting Units"), wxICON_ERROR, owner);
	}
}
//==========================================================================
// Class:			GuiInterface
// Function:		ScaleXData
//
// Description:		Scales the X-data by the specified factor.
//
// Input Arguments:
//		selectedRows	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::ScaleXData(const wxArrayInt& selectedRows)
{
	double factor(0.0);
	wxString factorText(_T("0.0"));

	while (!factorText.ToDouble(&factor) || factor == 0.0)
	{
		factorText = ::wxGetTextFromUser(_T("Specify scaling factor:"),
		_T("Specify Factor"), _T("1"), owner);
		if (factorText.IsEmpty())
			return;
	}

	// If applied to the row 0, apply to all curves
	if (selectedRows.Count() == 1 && selectedRows[0] == 0)
	{
		unsigned int stopIndex(plotList.GetCount());
		unsigned int i;
		for (i = 0; i < stopIndex; i++)
		{
			Dataset2D *scaledData = new Dataset2D(*plotList[i]);
			scaledData->MultiplyXData(factor);
			AddCurve(scaledData, grid->GetCellValue(i + 1, PlotListGrid::ColName));

			grid->SetCellBackgroundColour(i + stopIndex + 1, PlotListGrid::ColColor,
				grid->GetCellBackgroundColour(i + 1, PlotListGrid::ColColor));
			grid->SetCellValue(i + stopIndex + 1, PlotListGrid::ColLineSize,
				grid->GetCellValue(i + 1, PlotListGrid::ColLineSize));
			grid->SetCellValue(i + stopIndex + 1, PlotListGrid::ColMarkerSize,
				grid->GetCellValue(i + 1, PlotListGrid::ColMarkerSize));
			grid->SetCellValue(i + stopIndex + 1, PlotListGrid::ColVisible,
				grid->GetCellValue(i + 1, PlotListGrid::ColVisible));
			grid->SetCellValue(i + stopIndex + 1, PlotListGrid::ColRightAxis,
				grid->GetCellValue(i + 1, PlotListGrid::ColRightAxis));

			UpdateCurveProperties(i + stopIndex);
		}

		for (i = stopIndex; i > 0; i--)
			RemoveCurve(i - 1);
	}
	// If applied to any other row, apply only to that row (by duplicating curve)
	else
	{
		unsigned int i;
		for (i = 0; i < selectedRows.Count(); i++)
		{
			Dataset2D *scaledData = new Dataset2D(*plotList[selectedRows[i] - 1]);
			scaledData->MultiplyXData(factor);
			AddCurve(scaledData, grid->GetCellValue(selectedRows[i], PlotListGrid::ColName)
				+ wxString::Format(", X-scaled by %f", factor));
		}
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		PlotDerivative
//
// Description:		Adds a curve showing the derivative of the selected grid
//					row to the plot.
//
// Input Arguments:
//		selectedRows	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::PlotDerivative(const wxArrayInt& selectedRows)
{
	// Create new dataset containing the derivative of dataset and add it to the plot
	unsigned int i;
	for (i = 0; i < selectedRows.Count(); i++)
	{
		Dataset2D *newData = new Dataset2D(
			DiscreteDerivative::ComputeTimeHistory(*plotList[selectedRows[i] - 1]));

		wxString name = _T("d/dt(") + grid->GetCellValue(selectedRows[i], PlotListGrid::ColName) + _T(")");
		AddCurve(newData, name);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		PlotIntegral
//
// Description:		Adds a curve showing the integral of the selected grid
//					row to the plot.
//
// Input Arguments:
//		selectedRows	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::PlotIntegral(const wxArrayInt& selectedRows)
{
	// Create new dataset containing the integral of dataset and add it to the plot
	unsigned int i;
	for (i = 0; i < selectedRows.Count(); i++)
	{
		Dataset2D *newData = new Dataset2D(
			DiscreteIntegral::ComputeTimeHistory(*plotList[selectedRows[i] - 1]));

		wxString name = _T("integral(") + grid->GetCellValue(selectedRows[i], PlotListGrid::ColName) + _T(")");
		AddCurve(newData, name);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		PlotRMS
//
// Description:		Adds a curve showing the RMS of the selected grid
//					row to the plot.
//
// Input Arguments:
//		selectedRows	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::PlotRMS(const wxArrayInt& selectedRows)
{
	// Create new dataset containing the RMS of dataset and add it to the plot
	unsigned int i;
	for (i = 0; i < selectedRows.Count(); i++)
	{
		Dataset2D *newData = new Dataset2D(
			RootMeanSquare::ComputeTimeHistory(*plotList[selectedRows[i] - 1]));

		wxString name = _T("RMS(") + grid->GetCellValue(selectedRows[i], PlotListGrid::ColName) + _T(")");
		AddCurve(newData, name);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		PlotFFT
//
// Description:		Adds a curve showing the FFT of the selected grid
//					row to the plot.
//
// Input Arguments:
//		selectedRows	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::PlotFFT(const wxArrayInt& selectedRows)
{
	unsigned int i;
	for (i = 0; i < selectedRows.Count(); i++)
	{
		Dataset2D *newData(GetFFTData(plotList[selectedRows[i] - 1]));
		if (!newData)
			continue;

		wxString name = _T("FFT(") + grid->GetCellValue(selectedRows[i], PlotListGrid::ColName) + _T(")");
		AddCurve(newData, name);
		SetMarkerSize(grid->GetNumberRows() - 2, 0);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		BitMask
//
// Description:		Creates bit mask for the specified curve.
//
// Input Arguments:
//		selectedRows	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::BitMask(const wxArrayInt& selectedRows)
{
	unsigned long bit;
	wxString bitString = wxGetTextFromUser(_T("Specify the bit to plot:"), _T("Bit Seleciton"), _T("0"), owner);
	if (bitString.IsEmpty())
		return;
	else if (!bitString.ToULong(&bit))
	{
		wxMessageBox(_T("Bit value must be a positive integer."), _T("Bit Selection Error"), wxICON_ERROR, owner);
		return;
	}

	unsigned int i;
	for (i = 0; i < selectedRows.Count(); i++)
	{
		Dataset2D *newData = new Dataset2D(
			PlotMath::ApplyBitMask(*plotList[selectedRows[i] - 1], bit));

		wxString name = grid->GetCellValue(selectedRows[i], PlotListGrid::ColName) + _T(", Bit ") + wxString::Format("%lu", bit);
		AddCurve(newData, name);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		TimeShift
//
// Description:		Adds a new curve equivalent to the selected curve shifted
//					by the specified amount.
//
// Input Arguments:
//		selectedRows	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::TimeShift(const wxArrayInt& selectedRows)
{
	double shift(0.0);
	wxString shiftText = ::wxGetTextFromUser(
		_T("Specify the time to add to time data in original data:\n")
		_T("Use same units as time series.  Positive values shift curve to the right."),
		_T("Time Shift"), _T("0"), owner);

	if (!shiftText.ToDouble(&shift) || shift == 0.0)
		return;

	// Create new dataset containing the RMS of dataset and add it to the plot
	unsigned int i;
	for (i = 0; i < selectedRows.Count(); i++)
	{
		Dataset2D *newData = new Dataset2D(*plotList[selectedRows[i] - 1]);

		newData->XShift(shift);

		wxString name = grid->GetCellValue(selectedRows[i], PlotListGrid::ColName) + _T(", t = t0 + ");
		name += shiftText;
		AddCurve(newData, name);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		FilterCurves
//
// Description:		Displays a dialog allowing the user to specify the filter,
//					and adds the filtered curve to the plot.
//
// Input Arguments:
//		selectedRows	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::FilterCurves(const wxArrayInt& selectedRows)
{
	FilterParameters filterParameters(DisplayFilterDialog());
	if (filterParameters.order == 0)
		return;

	// Create new dataset containing the FFT of dataset and add it to the plot
	unsigned int i;
	for (i = 0; i < selectedRows.Count(); i++)
	{
		const Dataset2D *currentData = plotList[selectedRows[i] - 1];
		Dataset2D *newData = new Dataset2D(*currentData);

		ApplyFilter(filterParameters, *newData);

		wxString name = FilterDialog::GetFilterNamePrefix(filterParameters)
			+ _T(" (") + grid->GetCellValue(selectedRows[i], PlotListGrid::ColName) + _T(")");
		AddCurve(newData, name);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		FitCurves
//
// Description:		Fits a curve to the dataset selected in the grid control.
//					User is asked to specify the order of the fit.
//
// Input Arguments:
//		selectedRows	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::FitCurves(const wxArrayInt& selectedRows)
{
	// Ask the user what order to use for the polynomial
	unsigned long order;
	wxString orderString = ::wxGetTextFromUser(_T("Specify the order of the polynomial fit:"),
		_T("Polynomial Curve Fit"), _T("2"), owner);

	// If cancelled, the orderString will be empty.  It is possible that the user cleared the textbox
	// and clicked OK, but we'll ignore this case since we can't tell the difference
	if (orderString.IsEmpty())
		return;

	if (!orderString.ToULong(&order))
	{
		wxMessageBox(_T("ERROR:  Order must be a positive integer!"), _T("Error Fitting Curve"), wxICON_ERROR, owner);
		return;
	}

	unsigned int i;
	for (i = 0; i < selectedRows.Count(); i++)
	{
		wxString name;
		Dataset2D* newData(GetCurveFitData(order, plotList[selectedRows[i] - 1], name, selectedRows[i]));

		AddCurve(newData, name);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		GetCurveFitData
//
// Description:		Fits a curve of the specified order to the specified data
//					and returns a dataset containing the curve.
//
// Input Arguments:
//		order	= const unsigned int&
//		data	= const Dataset2D*
//		row		= const unsigned int&
//
// Output Arguments:
//		name	= wxString&
//
// Return Value:
//		::Dataset2D*
//
//==========================================================================
Dataset2D* GuiInterface::GetCurveFitData(const unsigned int &order,
	const Dataset2D* data, wxString &name, const unsigned int& row) const
{
	CurveFit::PolynomialFit fitData = CurveFit::DoPolynomialFit(*data, order);

	Dataset2D *newData = new Dataset2D(*data);
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); i++)
		newData->GetYPointer()[i] = CurveFit::EvaluateFit(newData->GetXData(i), fitData);

	name = GetCurveFitName(fitData, row);

	delete [] fitData.coefficients;

	return newData;
}

//==========================================================================
// Class:			GuiInterface
// Function:		GetCurveFitName
//
// Description:		Determines an appropriate name for a curve fit dataset.
//
// Input Arguments:
//		fitData	= const CurveFit::PolynomialFit&
//		row		= const unsigned int& specifying the dataset ID that was fit
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString indicating the name for the fit
//
//==========================================================================
wxString GuiInterface::GetCurveFitName(const CurveFit::PolynomialFit &fitData,
	const unsigned int &row) const
{
	wxString name, termString;
	//name.Printf("Order %lu Fit([%i]), R^2 = %0.2f", order, row, fitData.rSquared);
	name.Printf("Fit [%i] (R^2 = %0.2f): ", row, fitData.rSquared);

	unsigned int i;
	for (i = 0; i <= fitData.order; i++)
	{
		if (i == 0)
			termString.Printf("%1.2e", fitData.coefficients[i]);
		else if (i == 1)
			termString.Printf("%0.2ex", fabs(fitData.coefficients[i]));
		else
			termString.Printf("%0.2ex^%i", fabs(fitData.coefficients[i]), i);

		if (i < fitData.order)
		{
			if (fitData.coefficients[i + 1] > 0.0)
				termString.Append(_T(" + "));
			else
				termString.Append(_T(" - "));
		}
		name.Append(termString);
	}

	return name;
}

//==========================================================================
// Class:			GuiInterface
// Function:		DisplayMathChannelDialog
//
// Description:		Displays an input dialog that allows the user to enter a
//					math expression.  If an expression is entered, it attempts
//					to add the channel.
//
// Input Arguments:
//		defaultInput	= wxString (optional)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::DisplayMathChannelDialog(wxString defaultInput)
{
	// Display input dialog in which user can specify the math desired
	wxString message(_T("Enter the math you would like to perform:\n\n"));
	message.Append(_T("    Use [x] notation to specify channels, where x = 0 is Time, x = 1 is the first data channel, etc.\n"));
	message.Append(_T("    Valid operations are: +, -, *, /, %, ddt, int, fft and trigonometric functions.\n"));
	message.Append(_T("    Use () to specify order of operations"));

	AddCurve(::wxGetTextFromUser(message, _T("Specify Math Channel"), defaultInput, owner));
}

//==========================================================================
// Class:			GuiInterface
// Function:		DisplayAxisRangeDialog
//
// Description:		Displays an input dialog that allows the user to set the
//					range for an axis.
//
// Input Arguments:
//		axis	= const PlotRenderer::PlotContext& specifying the axis which is to be resized
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::DisplayAxisRangeDialog(const PlotRenderer::PlotContext &axis)
{
	double min, max;
	if (!renderer->GetCurrentAxisRange(axis, min, max))
		return;

	RangeLimitsDialog dialog(owner, min, max);
	if (dialog.ShowModal() != wxID_OK)
		return;

	// Get the new limits (and correct if they entered the larger value in the min box)
	if (dialog.GetMinimum() < dialog.GetMaximum())
	{
		min = dialog.GetMinimum();
		max = dialog.GetMaximum();
	}
	else
	{
		max = dialog.GetMinimum();
		min = dialog.GetMaximum();
	}

	// Make sure the limits aren't equal
	if (min == max)
	{
		wxMessageBox(_T("ERROR:  Limits must unique!"), _T("Error Setting Limits"), wxICON_ERROR, owner);
		return;
	}

	renderer->SetNewAxisRange(axis, min, max);
	renderer->SaveCurrentZoom();
}

//==========================================================================
// Class:			GuiInterface
// Function:		DisplayFilterDialog
//
// Description:		Dispalys a dialog box allowing the user to specify a filter,
//					returns the specified parameters.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		FilterParameters describing the user-specified filter (order = 0 for cancelled dialog)
//
//==========================================================================
FilterParameters GuiInterface::DisplayFilterDialog()
{
	FilterDialog dialog(renderer);
	if (dialog.ShowModal() != wxID_OK)
	{
		FilterParameters parameters;
		parameters.order = 0;
		return parameters;
	}

	return dialog.GetFilterParameters();
}

//==========================================================================
// Class:			GuiInterface
// Function:		ApplyFilter
//
// Description:		Applies the specified filter to the specified dataset.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//		data		= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::ApplyFilter(const FilterParameters &parameters,
	Dataset2D &data)
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Cutoff frequency may be incorrect!"),
			_T("Accuracy Warning"), wxICON_WARNING, owner);

	if (!PlotMath::XDataConsistentlySpaced(data))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, owner);

	Filter *filter = GetFilter(parameters, factor / data.GetAverageDeltaX(), data.GetYData(0));

	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetYPointer()[i] = filter->Apply(data.GetYData(i));

	// For phaseless filter, re-apply the same filter backwards
	if (parameters.phaseless)
	{
		data.Reverse();
		filter->Initialize(data.GetYData(0));
		for (i = 0; i < data.GetNumberOfPoints(); i++)
			data.GetYPointer()[i] = filter->Apply(data.GetYData(i));
		data.Reverse();
	}

	delete filter;
}

//==========================================================================
// Class:			GuiInterface
// Function:		GetFilter
//
// Description:		Returns a filter matching the specified parameters.
//
// Input Arguments:
//		parameters		= const FilterParameters&
//		sampleRate		= const double& [Hz]
//		initialValue	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		Filter*
//
//==========================================================================
Filter* GuiInterface::GetFilter(const FilterParameters &parameters,
	const double &sampleRate, const double &initialValue) const
{
	return new Filter(sampleRate,
		Filter::CoefficientsFromString(std::string(parameters.numerator.mb_str())),
		Filter::CoefficientsFromString(std::string(parameters.denominator.mb_str())),
		initialValue);
}

//==========================================================================
// Class:			GuiInterface
// Function:		UpdateLegend
//
// Description:		Updates the contents of the legend actor.
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
void GuiInterface::UpdateLegend()
{
	double lineSize;
	long markerSize;
	std::vector<LibPlot2D::Legend::LegendEntryInfo> entries;
	LibPlot2D::Legend::LegendEntryInfo info;
	int i;
	for (i = 1; i < grid->GetNumberRows(); i++)
	{
		if (grid->GetCellValue(i, PlotListGrid::ColVisible).IsEmpty())
			continue;
			
		grid->GetCellValue(i, PlotListGrid::ColLineSize).ToDouble(&lineSize);
		grid->GetCellValue(i, PlotListGrid::ColMarkerSize).ToLong(&markerSize);
		info.color = LibPlot2D::Color(grid->GetCellBackgroundColour(i, PlotListGrid::ColColor));
		info.lineSize = lineSize;
		info.markerSize = markerSize;
		info.text = grid->GetCellValue(i, PlotListGrid::ColName);
		entries.push_back(info);
	}
	renderer->UpdateLegend(entries);
}

//==========================================================================
// Class:			GuiInterface
// Function:		SetMarkerSize
//
// Description:		Sets the marker size for the specified curve.
//
// Input Arguments:
//		curve	= const unsigned int&
//		size	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::SetMarkerSize(const unsigned int &curve, const int &size)
{
	grid->SetCellValue(curve + 1, PlotListGrid::ColMarkerSize, wxString::Format("%i", size));
	UpdateCurveProperties(curve);
}

//==========================================================================
// Class:			GuiInterface
// Function:		UpdateCurveQuality
//
// Description:		Sets curve quality according to how many lines need to
//					be rendered.
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
void GuiInterface::UpdateCurveQuality()
{
	//if (renderer->GetTotalPointCount() > highQualityCurvePointLimit)
		renderer->SetCurveQuality(LibPlot2D::PlotRenderer::QualityHighWrite);
	/*else
		renderer->SetCurveQuality(static_cast<PlotRenderer::CurveQuality>(
		PlotRenderer::QualityHighStatic | PlotRenderer::QualityHighWrite));*/// TODO:  Fix this after line rendering is improved
}

//==========================================================================
// Class:			GuiInterface
// Function:		UnitStringToFactor
//
// Description:		Converts from a unit string to a factor value.
//
// Input Arguments:
//		unit	= const wxString&
//
// Output Arguments:
//		factor	= double&
//
// Return Value:
//		bool, true if unit can be converted, false otherwise
//
//==========================================================================
bool GuiInterface::UnitStringToFactor(const wxString &unit, double &factor)
{
	// We'll recognize the following units:
	// h, hr, hours -> factor = 1.0 / 3600.0
	// m, min, minutes -> factor = 1.0 / 60.0
	// s, sec, seconds -> factor = 1.0
	// ms, msec, milliseconds -> factor = 1000.0
	// us, usec, microseconds -> factor = 1000000.0

	if (unit.CmpNoCase(_T("h")) == 0 || unit.CmpNoCase(_T("hr")) == 0 || unit.CmpNoCase(_T("hours")) == 0)
		factor = 1.0 / 3600.0;
	else if (unit.CmpNoCase(_T("m")) == 0 || unit.CmpNoCase(_T("min")) == 0 || unit.CmpNoCase(_T("minutes")) == 0)
		factor = 1.0 / 60.0;
	else if (unit.CmpNoCase(_T("s")) == 0 || unit.CmpNoCase(_T("sec")) == 0 || unit.CmpNoCase(_T("seconds")) == 0)
		factor = 1.0;
	else if (unit.CmpNoCase(_T("ms")) == 0 || unit.CmpNoCase(_T("msec")) == 0 || unit.CmpNoCase(_T("milliseconds")) == 0)
		factor = 1000.0;
	else if (unit.CmpNoCase(_T("us")) == 0 || unit.CmpNoCase(_T("usec")) == 0 || unit.CmpNoCase(_T("microseconds")) == 0)
		factor = 1000000.0;
	else
	{
		// Assume a factor of 1
		factor = 1.0;
		return false;
	}

	return true;
}

//==========================================================================
// Class:			GuiInterface
// Function:		GetFFTData
//
// Description:		Returns a dataset containing an FFT of the specified data.
//
// Input Arguments:
//		data	= const LibPlot2D::Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		LibPlot2D::Dataset2D* pointing to a dataset contining the new FFT data
//
//==========================================================================
Dataset2D* GuiInterface::GetFFTData(const Dataset2D* data)
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, owner);

	LibPlot2D::FFTDialog dialog(owner, data->GetNumberOfPoints(),
		data->GetNumberOfZoomedPoints(renderer->GetXMin(), renderer->GetXMax()),
		data->GetAverageDeltaX() / factor);

	if (dialog.ShowModal() != wxID_OK)
		return nullptr;

	if (!LibPlot2D::PlotMath::XDataConsistentlySpaced(*data))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, owner);

	LibPlot2D::Dataset2D *newData;

	if (dialog.GetUseZoomedData())
		newData = new LibPlot2D::Dataset2D(LibPlot2D::FastFourierTransform::ComputeFFT(GetXZoomedDataset(*data),
			dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap(),
			dialog.GetSubtractMean()));
	else
		newData = new LibPlot2D::Dataset2D(LibPlot2D::FastFourierTransform::ComputeFFT(*data,
			dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap(),
			dialog.GetSubtractMean()));

	newData->MultiplyXData(factor);

	return newData;
}

//==========================================================================
// Class:			GuiInterface
// Function:		GetXZoomedDataset
//
// Description:		Returns a dataset containing only the data within the
//					current zoomed x-limits.
//
// Input Arguments:
//		fullData	= const LibPlot2D::Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		LibPlot2D::Dataset2D
//
//==========================================================================
Dataset2D GuiInterface::GetXZoomedDataset(const Dataset2D &fullData) const
{
	unsigned int i, startIndex(0), endIndex(0);
	while (fullData.GetXData(startIndex) < renderer->GetXMin() &&
		startIndex < fullData.GetNumberOfPoints())
		startIndex++;
	endIndex = startIndex;
	while (fullData.GetXData(endIndex) < renderer->GetXMax() &&
		endIndex < fullData.GetNumberOfPoints())
		endIndex++;

	LibPlot2D::Dataset2D data(endIndex - startIndex);
	for (i = startIndex; i < endIndex; i++)
	{
		data.GetXPointer()[i - startIndex] = fullData.GetXData(i);
		data.GetYPointer()[i - startIndex] = fullData.GetYData(i);
	}

	return data;
}

//==========================================================================
// Class:			GuiInterface
// Function:		ShowAppropriateXLabel
//
// Description:		Updates the x-axis label as necessary.
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
void GuiInterface::ShowAppropriateXLabel()
{
	// If the only visible curves are frequency plots, change the x-label
	int i;
	bool showFrequencyLabel(false);
	for (i = 1; i < grid->GetNumberRows(); i++)
	{
		if (grid->GetCellValue(i, PlotListGrid::ColVisible).Cmp(_T("1")) == 0)
		{
			if (grid->GetCellValue(i, PlotListGrid::ColName).Mid(0, 3).CmpNoCase(_T("FFT")) == 0 ||
				grid->GetCellValue(i, PlotListGrid::ColName).Mid(0, 3).CmpNoCase(_T("FRF")) == 0)
				showFrequencyLabel = true;
			else
			{
				showFrequencyLabel = false;
				break;
			}
		}
	}

	if (showFrequencyLabel)
		SetXDataLabel(FormatFrequency);
	else
		SetXDataLabel(currentFileFormat);
}

//==========================================================================
// Class:			GuiInterface
// Function:		GetXAxisScalingFactor
//
// Description:		Attempts to determine the scaling factor required to convert
//					the X-axis into seconds (assuming X-axis has units of time).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		factor	= double&, scaling factor
//		label	= wxString*, pointer to string, to be populated with the unit string
//
// Return Value:
//		bool; true for success, false otherwise
//
//==========================================================================
bool GuiInterface::GetXAxisScalingFactor(double &factor, wxString *label)
{
	if (XScalingFactorIsKnown(factor, label))
		return true;

	wxString unit = ExtractUnitFromDescription(genericXAxisLabel);

	unit = unit.Trim().Trim(false);
	if (label)
		label->assign(unit);

	return UnitStringToFactor(unit, factor);
}

//==========================================================================
// Class:			GuiInterface
// Function:		XScalingFactorIsKnown
//
// Description:		If the x-axis scaling factor is known, determines its value.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		factor	= double&
//		label	= wxString*
//
// Return Value:
//		bool, true if known, false otherwise
//
//==========================================================================
bool GuiInterface::XScalingFactorIsKnown(double &factor, wxString *label) const
{
	if (currentFileFormat == FormatBaumuller)
	{
		factor = 1000.0;
		if (label)
			label->assign(_T("msec"));
		return true;
	}
	else if (currentFileFormat == FormatKollmorgen)
	{
		factor = 1.0;
		if (label)
			label->assign(_T("sec"));
		return true;
	}

	return false;
}

//==========================================================================
// Class:			GuiInterface
// Function:		ExtractUnitFromDescription
//
// Description:		Parses the description looking for a unit string.  This
//					will recognize the following as unit strings:
//					X Series Name [unit]
//					X Series Name (unit)
//					X Series Name *delimiter* unit
//
// Input Arguments:
//		description	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the unit porition of the description
//
//==========================================================================
wxString GuiInterface::ExtractUnitFromDescription(const wxString &description)
{
	wxString unit;
	if (FindWrappedString(description, unit, '[', ']'))
		return unit;
	else if (FindWrappedString(description, unit, '(', ')'))
		return unit;

	// Check for last string following a delimiter
	wxArrayString delimiters;
	delimiters.Add(_T(","));
	delimiters.Add(_T(";"));
	delimiters.Add(_T("-"));
	delimiters.Add(_T(":"));

	int location;
	unsigned int i;
	for (i = 0; i < delimiters.size(); i++)
	{
		location = description.Find(delimiters[i].mb_str());
		if (location != wxNOT_FOUND && location < (int)description.Len() - 1)
		{
			unit = description.Mid(location + 1);
			break;
		}
	}

	return unit;
}

//==========================================================================
// Class:			GuiInterface
// Function:		FindWrappedString
//
// Description:		Determines if the specified string contains a string wrapped
//					with the specified characters.
//
// Input Arguments:
//		s		= const wxString&
//		open	= const wxChar& specifying the opening wrapping character
//		close	= const wxChar& specifying the closing warpping character
//
// Output Arguments:
//		contents	= wxString&
//
// Return Value:
//		bool, true if a wrapped string is found, false otherwise
//
//==========================================================================
bool GuiInterface::FindWrappedString(const wxString &s, wxString &contents,
	const wxChar &open, const wxChar &close)
{
	if (s.Len() < 3)
		return false;

	if (s.Last() == close)
	{
		int i;
		for (i = s.Len() - 2; i >= 0; i--)
		{
			if (s.at(i) == open)
			{
				contents = s.Mid(i + 1, s.Len() - i - 2);
				return true;
			}
		}
	}

	return false;
}

//==========================================================================
// Class:			GuiInterface
// Function:		SetXDataLabel
//
// Description:		Sets the x-data labels to the specified string.
//
// Input Arguments:
//		label	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::SetXDataLabel(wxString label)
{
	grid->SetCellValue(0, PlotListGrid::ColName, label);
	renderer->SetXLabel(label);
}

//==========================================================================
// Class:			GuiInterface
// Function:		SetXDataLabel
//
// Description:		Sets the x-data labels according to the opened file type.
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
void GuiInterface::SetXDataLabel(const FileFormat &format)
{
	switch (format)
	{
	case FormatFrequency:
		SetXDataLabel(_T("Frequency [Hz]"));
		break;

	default:
	case FormatGeneric:
	case FormatKollmorgen:
	case FormatBaumuller:
		SetXDataLabel(genericXAxisLabel);
	}
}

//==========================================================================
// Class:			GuiInterface
// Function:		UpdateCurveProperties
//
// Description:		Updates the specified curve properties.
//
// Input Arguments:
//		index	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::UpdateCurveProperties(const unsigned int &index)
{
	Color color;
	color.Set(grid->GetCellBackgroundColour(index + 1, PlotListGrid::ColColor));
	UpdateCurveProperties(index, color,
		!grid->GetCellValue(index + 1, PlotListGrid::ColVisible).IsEmpty(),
		!grid->GetCellValue(index + 1, PlotListGrid::ColRightAxis).IsEmpty());
}

//==========================================================================
// Class:			GuiInterface
// Function:		UpdateCurveProperties
//
// Description:		Updates the specified curve properties to match the arguments.
//
// Input Arguments:
//		index		= const unsigned int&
//		color		= const Color&
//		visible		= const bool&
//		rightAxis	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiInterface::UpdateCurveProperties(const unsigned int &index,
	const Color &color, const bool &visible, const bool &rightAxis)
{
	double lineSize;
	long markerSize;
	UpdateLegend();// Must come first in order to be updated simultaneously with line
	grid->GetCellValue(index + 1, PlotListGrid::ColLineSize).ToDouble(&lineSize);
	grid->GetCellValue(index + 1, PlotListGrid::ColMarkerSize).ToLong(&markerSize);
	renderer->SetCurveProperties(index, color, visible, rightAxis, lineSize, markerSize);
	renderer->SaveCurrentZoom();
}

void GuiInterface::Copy()
{
	renderer->DoCopy();
}

void GuiInterface::Paste()
{
	renderer->DoPaste();
}

}// namespace LibPlot2D
