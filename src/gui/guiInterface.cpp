/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  guiInterface.cpp
// Date:  12/9/2016
// Auth:  K. Loux
// Desc:  Manages interactions between plot mGrid and plot renderings, handles
//        context menus, etc.

// Local headers
#include "lp2d/gui/guiInterface.h"
#include "lp2d/gui/createSignalDialog.h"
#include "lp2d/gui/fftDialog.h"
#include "lp2d/gui/filterDialog.h"
#include "lp2d/gui/frfDialog.h"
#include "lp2d/gui/rangeLimitsDialog.h"
#include "lp2d/gui/rolloverSelectionDialog.h"
#include "lp2d/parser/dataFile.h"
#include "lp2d/parser/baumullerFile.h"
#include "lp2d/parser/customFile.h"
#include "lp2d/parser/customXMLFile.h"
#include "lp2d/parser/genericFile.h"
#include "lp2d/parser/kollmorgenFile.h"
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
#include <algorithm>

namespace LibPlot2D
{

//=============================================================================
// Class:			GuiInterface
// Function:		GuiInterface
//
// Description:		Constructor for GuiInterface class.
//
// Input Arguments:
//		owner	= wxFrame*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
GuiInterface::GuiInterface(wxFrame* owner) : mOwner(owner)
{
}

//=============================================================================
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
//=============================================================================
bool GuiInterface::LoadFiles(const wxArrayString &fileList)
{
	unsigned int i, j;
	std::vector<bool> loaded(fileList.size());
	std::vector<std::unique_ptr<DataFile>> files(fileList.size());
	typedef std::map<wxArrayString, DataFile::SelectionData,
		ArrayStringCompare> SelectionMap;
	SelectionMap selectionInfoMap;
	SelectionMap::const_iterator it;
	DataFile::SelectionData selectionInfo;
	bool atLeastOneFileLoaded(false);
	for (i = 0; i < fileList.Count(); ++i)
	{
		files[i] = mFileTypeManager.GetDataFile(fileList[i]);
		if (!files[i])
		{
			// TODO:  Error message?
			continue;
		}

		files[i]->Initialize();
		it = selectionInfoMap.find(files[i]->GetAllDescriptions());
		if (it == selectionInfoMap.end())
		{
			if (files[i]->DescriptionsMatch(mLastDescriptions))
				selectionInfo = mLastSelectionInfo;
			else
				selectionInfo.selections.Clear();

			if (mPlotList.GetCount() == 0 && files[i]->GetAllDescriptions().Count() == 2)// 2 descriptions because X column counts as one
			{
				// No existing data (no need for "Remove Existing Curves?") and
				// only one curve in file - no need to ask anything of user.
				selectionInfo.selections.Clear();// In case lastSelectionInfo was used to initialize selectionInfo
				selectionInfo.selections.Add(0);
				selectionInfoMap[files[i]->GetAllDescriptions()] = selectionInfo;
			}
			else
			{
				files[i]->GetSelectionsFromUser(selectionInfo, mOwner);
				if (selectionInfo.selections.Count() < 1)
					return false;
				selectionInfoMap[files[i]->GetAllDescriptions()] = selectionInfo;
			}
		}
		else
			selectionInfo = it->second;

		loaded[i] = selectionInfo.selections.Count() > 0 && files[i]->Load(selectionInfo);
		atLeastOneFileLoaded = atLeastOneFileLoaded || loaded[i];
	}

	if (!atLeastOneFileLoaded)
		return false;

	if (selectionInfo.removeExisting)
		ClearAllCurves();

	wxString curveName;
	for (i = 0; i < fileList.Count(); ++i)
	{
		if (!loaded[i])
			continue;

		for (j = 0; j < files[i]->GetDataCount(); ++j)
		{
			if (fileList.Count() > 1)
				curveName = files[i]->GetDescription(j + 1) + _T(" : ") + GuiUtilities::ExtractFileNameFromPath(fileList[i]);
			else
				curveName = files[i]->GetDescription(j + 1);
			AddCurve(std::move(files[i]->GetDataset(j)), curveName);
		}
	}

	if (mOwner)
	{
		if (fileList.Count() > 1)
			mOwner->SetTitle(_T("Multiple Files - ") + mApplicationTitle);
		else
			mOwner->SetTitle(GuiUtilities::ExtractFileNameFromPath(fileList[0]) + _T("- ") + mApplicationTitle);
	}

	mGenericXAxisLabel = files[0]->GetDescription(0);
	SetXDataLabel(mGenericXAxisLabel);
	mRenderer->SaveCurrentZoom();

	// Because fileList is a reference (and may refer to lastFilesLoaded), we need to check for self-assignment
	if (&fileList != &mLastFilesLoaded)
		mLastFilesLoaded = fileList;
	mLastSelectionInfo = selectionInfo;
	mLastDescriptions = files[files.size() - 1]->GetAllDescriptions();

	return true;
}

//=============================================================================
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
//=============================================================================
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
			_T("Could Not Delete File"), wxICON_ERROR, mOwner);
		return false;
	}

	tempFile << textData;
	tempFile.close();

	bool fileLoaded = LoadFiles(wxArrayString(1, &tempFileName));
	if (remove(tempFileName.mb_str()) != 0)
		wxMessageBox(_T("Error deleting temporary file '") + tempFileName + _T("'."),
		_T("Could Not Delete File"), wxICON_ERROR, mOwner);

	if (fileLoaded && mOwner)
		mOwner->SetTitle(_T("Clipboard Data - ") + mApplicationTitle);

	return fileLoaded;
}

//=============================================================================
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
//=============================================================================
void GuiInterface::ReloadData()
{
	if (mLastFilesLoaded.IsEmpty())
		return;

	LoadFiles(mLastFilesLoaded);
}

//=============================================================================
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
//=============================================================================
wxString GuiInterface::GenerateTemporaryFileName(const unsigned int &length) const
{
	wxString name;
	unsigned int i;
	for (i = 0; i < length; ++i)
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

//=============================================================================
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
//=============================================================================
void GuiInterface::ClearAllCurves()
{
	while (mPlotList.GetCount() > 0)
		RemoveCurve(0);
}

//=============================================================================
// Class:			GuiInterface
// Function:		AddCurve
//
// Description:		Adds a new dataset to the plot, created by operating on
//					existing datasets.
//
// Input Arguments:
//		mathString	= wxString describing the desired math operations
//		name		= wxString to use for the display name
//		visible		= const bool& flag indicating whether or not the curve is
//					  initially visible
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void GuiInterface::AddCurve(wxString mathString, wxString name,
	const bool& visible)
{
	// String will be empty if the user cancelled
	if (mathString.IsEmpty())
		return;

	// Parse string and determine what the new dataset should look like
	ExpressionTree expression(&mPlotList);
	std::unique_ptr<Dataset2D> mathChannel(std::make_unique<Dataset2D>());

	double xAxisFactor;
	GetXAxisScalingFactor(xAxisFactor);// No warning here:  it's only an issue for FFTs and filters; warning are generated then

	wxString errors = expression.Solve(mathString, *mathChannel, xAxisFactor);

	if (!errors.IsEmpty())
	{
		wxMessageBox(_T("Could not solve expression:\n\n") + errors,
			_T("Error Solving Expression"), wxICON_ERROR, mOwner);
		DisplayMathChannelDialog(mathString);
		return;
	}

	if (name.IsEmpty())
		name = mathString.Upper();

	AddCurve(std::move(mathChannel), name, visible);
}

//=============================================================================
// Class:			GuiInterface
// Function:		AddCurve
//
// Description:		Adds an existing dataset to the plot.
//
// Input Arguments:
//		data	= std::unique_ptr<Dataset2D> to add
//		name	= wxString specifying the label for the curve
//		visible	= const bool& flag indicating whether or not the curve is
//				  initially visible
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void GuiInterface::AddCurve(std::unique_ptr<Dataset2D> data, wxString name, const bool& visible)
{
	mPlotList.Add(std::move(data));

	mRenderer->AddCurve(*mPlotList.Back());
	if (mGrid)
	{
		mGrid->BeginBatch();
		if (mGrid->GetNumberRows() == 0)
			mGrid->AddTimeRow();
		const unsigned int index(mGrid->AddDataRow(name, visible));
		mGrid->EndBatch();

		mGrid->Scroll(-1, mGrid->GetNumberRows());

		UpdateCurveProperties(index - 1, mGrid->GetNextColor(index), visible, false);
	}

	UpdateCurveQuality();
	mRenderer->UpdateDisplay();
}

//=============================================================================
// Class:			GuiInterface
// Function:		HideAllCurves
//
// Description:		Sets visibility to false for all curves.
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
void GuiInterface::HideAllCurves()
{
	for (int row = 1; row < mGrid->GetNumberRows(); ++row)
	{
		mGrid->SetCellValue(row, static_cast<int>(PlotListGrid::Column::Visible), _T("0"));
		UpdateCurveProperties(row - 1);
	}

	mRenderer->UpdateDisplay();
}

//=============================================================================
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
//=============================================================================
void GuiInterface::RemoveCurve(const unsigned int &i)
{
	if (mGrid)
	{
		mGrid->DeleteRows(i + 1);

		if (mGrid->GetNumberRows() == 1)
			mGrid->DeleteRows();

		mGrid->AutoSizeColumns();
	}

	mRenderer->RemoveCurve(i);
	mPlotList.Remove(i);

	UpdateCurveQuality();
	UpdateLegend();
}

//=============================================================================
// Class:			GuiInterface
// Function:		RemoveCurves
//
// Description:		Removes a set of curves from the plot.
//
// Input Arguments:
//		curves	= wxArrayInt
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void GuiInterface::RemoveCurves(wxArrayInt curves)
{
	curves.Sort([](int* a, int* b)
	{
		if (*a < *b)
			return 1;
		else if (*b < *a)
			return -1;
		return 0;
	});

	for (const auto& curve : curves)
		// minus 1 because we remove based on curve index, not row index
		RemoveCurve(curve - 1);

	mRenderer->UpdateDisplay();
}

//=============================================================================
// Class:			GuiInterface
// Function:		RemoveSelectedCurves
//
// Description:		Removes the selected curves from the plot list.
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
void GuiInterface::RemoveSelectedCurves()
{
	RemoveCurves(mGrid->GetSelectedRows());
}

//=============================================================================
// Class:			GuiInterface
// Function:		UpdateCursorValues
//
// Description:		Updates the values for the cursors and their differences
//					in the options mGrid.
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
//=============================================================================
void GuiInterface::UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue)
{
	if (!mGrid)
		return;

	// TODO:  This would be nicer with smart precision so we show enough digits but not too many

	// For each curve, update the cursor values
	int i;
	bool showXDifference(false);
	for (i = 1; i < mGrid->GetNumberRows(); ++i)
	{
		UpdateSingleCursorValue(i, leftValue, PlotListGrid::Column::LeftCursor, leftVisible);
		UpdateSingleCursorValue(i, rightValue, PlotListGrid::Column::RightCursor, rightVisible);

		if (leftVisible && rightVisible)
		{
			double left, right;
			if (mPlotList[i - 1]->GetYAt(leftValue, left) && mPlotList[i - 1]->GetYAt(rightValue, right))
			{
				mGrid->SetCellValue(i, static_cast<int>(PlotListGrid::Column::Difference), wxString::Format("%f", right - left));
				showXDifference = true;
			}
			else
				mGrid->SetCellValue(i, static_cast<int>(PlotListGrid::Column::Difference), wxEmptyString);
		}
	}

	if (showXDifference)
		mGrid->SetCellValue(0, static_cast<int>(PlotListGrid::Column::Difference), wxString::Format("%f", rightValue - leftValue));
}

//=============================================================================
// Class:			GuiInterface
// Function:		UpdateSingleCursorValue
//
// Description:		Updates a single cursor value.
//
// Input Arguments:
//		row			= const unsigned int& specifying the mGrid row
//		value		= const double& specifying the value to populate
//		column		= const PlotListGrid::Column& specifying which mGrid column to populate
//		isVisible	= const bool& indicating whether or not the cursor is visible
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void GuiInterface::UpdateSingleCursorValue(const unsigned int &row,
	double value, const PlotListGrid::Column &column, const bool &isVisible)
{
	if (isVisible)
	{
		mGrid->SetCellValue(0, static_cast<int>(column), wxString::Format("%f", value));

		bool exact;
		double valueOut;
		if (mPlotList[row - 1]->GetYAt(value, valueOut, &exact))
		{
			if (exact)
				mGrid->SetCellValue(row, static_cast<int>(column), _T("*") + wxString::Format("%f", valueOut));
			else
				mGrid->SetCellValue(row, static_cast<int>(column), wxString::Format("%f", valueOut));
		}
		else
			mGrid->SetCellValue(row, static_cast<int>(column), wxEmptyString);
	}
	else
	{
		mGrid->SetCellValue(0, static_cast<int>(column), wxEmptyString);
		mGrid->SetCellValue(row, static_cast<int>(column), wxEmptyString);

		// The difference column only exists if both cursors are visible
		mGrid->SetCellValue(0, static_cast<int>(PlotListGrid::Column::Difference), wxEmptyString);
		mGrid->SetCellValue(row, static_cast<int>(PlotListGrid::Column::Difference), wxEmptyString);
	}
}

//=============================================================================
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
//=============================================================================
void GuiInterface::RegisterAllBuiltInFileTypes()
{
	RegisterFileType<BaumullerFile>();
	RegisterFileType<KollmorgenFile>();
	RegisterFileType<CustomFile>();
	RegisterFileType<CustomXMLFile>();
	RegisterFileType<GenericFile>();
}

//=============================================================================
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
//=============================================================================
void GuiInterface::ExportData()
{
	// TODO:  Eliminate need for this
	if (!mGrid)
		return;

	wxString wildcard(_T("Comma Separated (*.csv)|*.csv"));
	wildcard.append("|Tab Delimited (*.txt)|*.txt");

	wxArrayString pathAndFileName = GuiUtilities::GetFileNameFromUser(mOwner, _T("Save As"),
		wxEmptyString, wxEmptyString, wildcard, wxFD_SAVE);

	if (pathAndFileName.Count() == 0)
		return;

	if (wxFile::Exists(pathAndFileName[0]))
	{
		if (wxMessageBox(_T("File exists.  Overwrite?"), _T("Overwrite File?"), wxYES_NO, mOwner) == wxNO)
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
			_T("Error Writing File"), wxICON_ERROR, mOwner);
		return;
	}

	unsigned int i, j(0);
	wxString temp;
	for (i = 1; i < mPlotList.GetCount() + 1; ++i)
	{
		if (mGrid->GetCellValue(i, static_cast<int>(PlotListGrid::Column::Name)).Contains(_T("FFT")) ||
			mGrid->GetCellValue(i, static_cast<int>(PlotListGrid::Column::Name)).Contains(_T("FRF")))
			outFile << _T("Frequency [Hz]") << delimiter;
		else
		{
			if (delimiter.Cmp(",") == 0)
			{
				temp = mGenericXAxisLabel;
				temp.Replace(",", ";");
				outFile << temp << delimiter;
			}
			else
				outFile << mGenericXAxisLabel << delimiter;
		}

		if (delimiter.Cmp(",") == 0)
		{
			temp = mGrid->GetCellValue(i, static_cast<int>(PlotListGrid::Column::Name));
			temp.Replace(",", ";");
			outFile << temp;
		}
		else
			outFile << mGrid->GetCellValue(i, static_cast<int>(PlotListGrid::Column::Name));

		if (i == mPlotList.GetCount())
			outFile << std::endl;
		else
			outFile << delimiter;
	}

	outFile.precision(14);

	bool done(false);
	while (!done)
	{
		done = true;
		for (i = 0; i < mPlotList.GetCount(); ++i)
		{
			if (j < mPlotList[i]->GetNumberOfPoints())
				outFile << mPlotList[i]->GetX()[j] << delimiter << mPlotList[i]->GetY()[j];
			else
				outFile << delimiter;

			if (i == mPlotList.GetCount() - 1)
				outFile << std::endl;
			else
				outFile << delimiter;

			if (j + 1 < mPlotList[i]->GetNumberOfPoints())
				done = false;
		}

		++j;
	}

	outFile.close();
}

//=============================================================================
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
//=============================================================================
void GuiInterface::GenerateFRF()
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, mOwner);

	wxArrayString descriptions;
	if (mGrid)
	{
		int i;
		for (i = 1; i < mGrid->GetNumberRows(); ++i)
			descriptions.Add(mGrid->GetCellValue(i, 0));
	}
	else
	{
		// TODO:  What here?
	}

	FRFDialog dialog(mOwner, descriptions);
	if (dialog.ShowModal() != wxID_OK)
		return;

	std::unique_ptr<Dataset2D> amplitude(std::make_unique<Dataset2D>()), phase, coherence;

	if (dialog.GetComputePhase())
		phase = std::make_unique<Dataset2D>();
	if (dialog.GetComputeCoherence())
		coherence = std::make_unique<Dataset2D>();

	if (!PlotMath::XDataConsistentlySpaced(*mPlotList[dialog.GetInputIndex()]) ||
		!PlotMath::XDataConsistentlySpaced(*mPlotList[dialog.GetOutputIndex()]))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, mOwner);

	FastFourierTransform::ComputeFRF(*mPlotList[dialog.GetInputIndex()],
		*mPlotList[dialog.GetOutputIndex()], dialog.GetNumberOfAverages(),
		FastFourierTransform::WindowType::Hann, dialog.GetModuloPhase(), *amplitude, phase.get(), coherence.get());

	AddFFTCurves(factor, std::move(amplitude), std::move(phase), std::move(coherence), wxString::Format("[%u] to [%u]",
		dialog.GetInputIndex() + 1, dialog.GetOutputIndex() + 1));
}

//=============================================================================
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
//=============================================================================
void GuiInterface::CreateSignal()
{
	double startTime(0.0);// [sec]
	double duration(10.0);// [sec]
	double sampleRate(100.0);// [Hz]

	double factor(1.0);
	if (mPlotList.GetCount() > 0)
	{
		GetXAxisScalingFactor(factor);

		// Use first curve to pull time and frequency information
		sampleRate = 1.0 / PlotMath::GetAverageXSpacing(*mPlotList[0]) * factor;
		startTime = mPlotList[0]->GetX()[0] / factor;
		duration = mPlotList[0]->GetX().back() / factor - startTime;
	}

	CreateSignalDialog dialog(mOwner, startTime, duration, sampleRate);

	if (dialog.ShowModal() != wxID_OK)
		return;

	dialog.GetSignal()->MultiplyXData(factor);
	AddCurve(std::move(dialog.GetSignal()), dialog.GetSignalName());

	// Set time units if it hasn't been done already
	double dummy;
	if (!GetXAxisScalingFactor(dummy))
	{
		mGenericXAxisLabel = _T("Time [sec]");
		SetXDataLabel(mGenericXAxisLabel);
		//plotArea->SaveCurrentZoom();// TODO:  Is this necessary?
	}
}

//=============================================================================
// Class:			GuiInterface
// Function:		AddFFTCurves
//
// Description:		Adds the FFT curves to the plot list.
//
// Input Arguments:
//		xFactor	= const double& scaling factor to convert X units to Hz
//		amplitude	= std::unique_ptr<Dataset2D>
//		phase		= std::unique_ptr<Dataset2D>
//		coherence	= std::unique_ptr<Dataset2D>
//		namePortion	= const wxString& identifying the input/output signals
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void GuiInterface::AddFFTCurves(const double& xFactor,
	std::unique_ptr<Dataset2D> amplitude, std::unique_ptr<Dataset2D> phase,
	std::unique_ptr<Dataset2D> coherence, const wxString &namePortion)
{
	amplitude->MultiplyXData(xFactor);
	AddCurve(std::move(amplitude), _T("FRF Amplitude, ") + namePortion + _T(", [dB]"));
	SetMarkerSize(mPlotList.GetCount() - 2, 0);

	if (phase)
	{
		phase->MultiplyXData(xFactor);
		AddCurve(std::move(phase), _T("FRF Phase, ") + namePortion + _T(", [deg]"));
		SetMarkerSize(mPlotList.GetCount() - 2, 0);
	}

	if (coherence)
	{
		coherence->MultiplyXData(xFactor);
		AddCurve(std::move(coherence), _T("FRF Coherence, ") + namePortion + _T(", [-]"));
		SetMarkerSize(mPlotList.GetCount() - 2, 0);
	}
}

//=============================================================================
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
//=============================================================================
void GuiInterface::SetTimeUnits()
{
	double f;
	wxString units;

	// Check to see if we already have some confidence in our x-axis units
	if (GetXAxisScalingFactor(f, &units))
	{
		// Ask the user to confirm, since we don't think we need their help
		if (wxMessageBox(_T("Time units are being interpreted as ") + units +
			_T(", are you sure you want to change them?"), _T("Are You Sure?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, mOwner) == wxNO)
			return;
	}

	// Ask the user to specify the correct units
	wxString userUnits;
	userUnits = ::wxGetTextFromUser(_T("Specify time units (e.g. \"msec\" or \"minutes\")"),
		_T("Specify Units"), _T("seconds"), mOwner);

	// If the user cancelled, we will have a blank string
	if (userUnits.IsEmpty())
		return;

	if (!SetTimeUnits(userUnits))
		wxMessageBox(_T("Could not understand units \"") + userUnits + _T("\"."), _T("Error Setting Units"), wxICON_ERROR, mOwner);
}

//=============================================================================
// Class:			GuiInterface
// Function:		SetTimeUnits
//
// Description:		Available for the user to clarify the time units when we
//					are unable to determine them easily from the input file.
//
// Input Arguments:
//		unitString	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//=============================================================================
bool GuiInterface::SetTimeUnits(const wxString& unitString)
{
	wxString currentLabel(mGrid ? wxString() : mGrid->GetCellValue(0, static_cast<int>(PlotListGrid::Column::Name)));
	mGenericXAxisLabel = _T("Time [") + unitString + _T("]");
	SetXDataLabel(mGenericXAxisLabel);

	double f;
	if (!GetXAxisScalingFactor(f, nullptr))
	{
		SetXDataLabel(currentLabel);
		return false;
	}

	return true;
}

//=============================================================================
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
//=============================================================================
void GuiInterface::ScaleXData(const wxArrayInt& selectedRows)
{
	if (!mGrid)// TODO:  Eliminate need for this check
		return;

	double factor(0.0);
	wxString factorText(_T("0.0"));

	while (!factorText.ToDouble(&factor) || factor == 0.0)
	{
		factorText = ::wxGetTextFromUser(_T("Specify scaling factor:"),
		_T("Specify Factor"), _T("1"), mOwner);
		if (factorText.IsEmpty())
			return;
	}

	// If applied to the row 0, apply to all curves
	if (selectedRows.Count() == 1 && selectedRows[0] == 0)
	{
		unsigned int stopIndex(mPlotList.GetCount());
		unsigned int i;
		for (i = 0; i < stopIndex; ++i)
		{
			std::unique_ptr<Dataset2D> scaledData(std::make_unique<Dataset2D>(*mPlotList[i]));
			scaledData->MultiplyXData(factor);
			AddCurve(std::move(scaledData), mGrid->GetCellValue(i + 1, static_cast<int>(PlotListGrid::Column::Name)));

			mGrid->SetCellBackgroundColour(i + stopIndex + 1, static_cast<int>(PlotListGrid::Column::Color),
				mGrid->GetCellBackgroundColour(i + 1, static_cast<int>(PlotListGrid::Column::Color)));
			mGrid->SetCellValue(i + stopIndex + 1, static_cast<int>(PlotListGrid::Column::LineSize),
				mGrid->GetCellValue(i + 1, static_cast<int>(PlotListGrid::Column::LineSize)));
			mGrid->SetCellValue(i + stopIndex + 1, static_cast<int>(PlotListGrid::Column::MarkerSize),
				mGrid->GetCellValue(i + 1, static_cast<int>(PlotListGrid::Column::MarkerSize)));
			mGrid->SetCellValue(i + stopIndex + 1, static_cast<int>(PlotListGrid::Column::Visible),
				mGrid->GetCellValue(i + 1, static_cast<int>(PlotListGrid::Column::Visible)));
			mGrid->SetCellValue(i + stopIndex + 1, static_cast<int>(PlotListGrid::Column::RightAxis),
				mGrid->GetCellValue(i + 1, static_cast<int>(PlotListGrid::Column::RightAxis)));

			UpdateCurveProperties(i + stopIndex);
		}

		for (i = stopIndex; i > 0; --i)
			RemoveCurve(i - 1);
	}
	// If applied to any other row, apply only to that row (by duplicating curve)
	else
	{
		for (const auto& row : selectedRows)
		{
			std::unique_ptr<Dataset2D> scaledData(std::make_unique<Dataset2D>(*mPlotList[row - 1]));
			scaledData->MultiplyXData(factor);
			AddCurve(std::move(scaledData), mGrid->GetCellValue(row, static_cast<int>(PlotListGrid::Column::Name))
				+ wxString::Format(", X-scaled by %f", factor));
		}
	}
}

//=============================================================================
// Class:			GuiInterface
// Function:		PlotDerivative
//
// Description:		Adds a curve showing the derivative of the selected mGrid
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
//=============================================================================
void GuiInterface::PlotDerivative(const wxArrayInt& selectedRows)
{
	// Create new dataset containing the derivative of dataset and add it to the plot
	for (const auto& row : selectedRows)
	{
		std::unique_ptr<Dataset2D> newData(std::make_unique<Dataset2D>(
			DiscreteDerivative::ComputeTimeHistory(*mPlotList[row - 1])));

		wxString name(_T("d/dt(") + mGrid->GetCellValue(row, static_cast<int>(PlotListGrid::Column::Name)) + _T(")"));
		AddCurve(std::move(newData), name);
	}
}

//=============================================================================
// Class:			GuiInterface
// Function:		PlotIntegral
//
// Description:		Adds a curve showing the integral of the selected mGrid
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
//=============================================================================
void GuiInterface::PlotIntegral(const wxArrayInt& selectedRows)
{
	// Create new dataset containing the integral of dataset and add it to the plot
	for (const auto& row : selectedRows)
	{
		std::unique_ptr<Dataset2D> newData(std::make_unique<Dataset2D>(
			DiscreteIntegral::ComputeTimeHistory(*mPlotList[row - 1])));

		wxString name(_T("integral(") + mGrid->GetCellValue(row, static_cast<int>(PlotListGrid::Column::Name)) + _T(")"));
		AddCurve(std::move(newData), name);
	}
}

//=============================================================================
// Class:			GuiInterface
// Function:		PlotRMS
//
// Description:		Adds a curve showing the RMS of the selected mGrid
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
//=============================================================================
void GuiInterface::PlotRMS(const wxArrayInt& selectedRows)
{
	// Create new dataset containing the RMS of dataset and add it to the plot
	for (const auto& row : selectedRows)
	{
		std::unique_ptr<Dataset2D> newData(std::make_unique<Dataset2D>(
			RootMeanSquare::ComputeTimeHistory(*mPlotList[row - 1])));

		wxString name(_T("RMS(") + mGrid->GetCellValue(row, static_cast<int>(PlotListGrid::Column::Name)) + _T(")"));
		AddCurve(std::move(newData), name);
	}
}

//=============================================================================
// Class:			GuiInterface
// Function:		PlotFFT
//
// Description:		Adds a curve showing the FFT of the selected mGrid
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
//=============================================================================
void GuiInterface::PlotFFT(const wxArrayInt& selectedRows)
{
	for (const auto& row : selectedRows)
	{
		std::unique_ptr<Dataset2D> newData(GetFFTData(mPlotList[row - 1]));
		if (!newData)
			continue;

		wxString name(_T("FFT(") + mGrid->GetCellValue(row, static_cast<int>(PlotListGrid::Column::Name)) + _T(")"));
		AddCurve(std::move(newData), name);
		SetMarkerSize(mGrid->GetNumberRows() - 2, 0);
	}
}

//=============================================================================
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
//=============================================================================
void GuiInterface::BitMask(const wxArrayInt& selectedRows)
{
	unsigned long bit;
	wxString bitString = wxGetTextFromUser(_T("Specify the bit to plot:"),
		_T("Bit Seleciton"), _T("0"), mOwner);
	if (bitString.IsEmpty())
		return;
	else if (!bitString.ToULong(&bit))
	{
		wxMessageBox(_T("Bit value must be a positive integer."),
			_T("Bit Selection Error"), wxICON_ERROR, mOwner);
		return;
	}

	for (const auto& row : selectedRows)
	{
		std::unique_ptr<Dataset2D> newData(std::make_unique<Dataset2D>(
			PlotMath::ApplyBitMask(*mPlotList[row - 1], bit)));

		wxString name(mGrid->GetCellValue(row, static_cast<int>(PlotListGrid::Column::Name))
			+ _T(", Bit ") + wxString::Format("%lu", bit));
		AddCurve(std::move(newData), name);
	}
}

//=============================================================================
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
//=============================================================================
void GuiInterface::TimeShift(const wxArrayInt& selectedRows)
{
	double shift(0.0);
	wxString shiftText = ::wxGetTextFromUser(
		_T("Specify the time to add to time data in original data:\n")
		_T("Use same units as time series.  Positive values shift curve to the right."),
		_T("Time Shift"), _T("0"), mOwner);

	if (!shiftText.ToDouble(&shift) || shift == 0.0)
		return;

	// Create new dataset containing the RMS of dataset and add it to the plot
	for (const auto& row : selectedRows)
	{
		std::unique_ptr<Dataset2D> newData(
			std::make_unique<Dataset2D>(*mPlotList[row - 1]));
		newData->XShift(shift);

		wxString name(mGrid->GetCellValue(row, static_cast<int>(PlotListGrid::Column::Name))
			+ _T(", t = t0 + "));
		name += shiftText;
		AddCurve(std::move(newData), name);
	}
}

//=============================================================================
// Class:			GuiInterface
// Function:		UnwrapData
//
// Description:		Unwraps the specified curves.
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
//=============================================================================
void GuiInterface::UnwrapData(const wxArrayInt& selectedRows)
{
	RolloverSelectionDialog dialog(mOwner);
	if (dialog.ShowModal() != wxID_OK)
		return;

	const double rolloverPoint(dialog.GetRolloverPoint());
	for (const auto& row : selectedRows)
	{
		std::unique_ptr<Dataset2D> newData(
			std::make_unique<Dataset2D>(*mPlotList[row - 1]));
		newData->UnwrapData(rolloverPoint);

		wxString name(mGrid->GetCellValue(row, static_cast<int>(PlotListGrid::Column::Name))
			+ _T(", Unwrapped at ") + wxString::Format(_T("%f"), rolloverPoint));
		AddCurve(std::move(newData), name);
	}
}

//=============================================================================
// Class:			GuiInterface
// Function:		WrapData
//
// Description:		Wraps the specified curves.
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
//=============================================================================
void GuiInterface::WrapData(const wxArrayInt& selectedRows)
{
	RolloverSelectionDialog dialog(mOwner);
	if (dialog.ShowModal() != wxID_OK)
		return;

	const double rolloverPoint(dialog.GetRolloverPoint());
	for (const auto& row : selectedRows)
	{
		std::unique_ptr<Dataset2D> newData(
			std::make_unique<Dataset2D>(*mPlotList[row - 1]));
		newData->WrapData(rolloverPoint);

		wxString name(mGrid->GetCellValue(row, static_cast<int>(PlotListGrid::Column::Name))
			+ _T(", Wrapped at ") + wxString::Format(_T("%f"), rolloverPoint));
		AddCurve(std::move(newData), name);
	}
}

//=============================================================================
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
//=============================================================================
void GuiInterface::FilterCurves(const wxArrayInt& selectedRows)
{
	FilterParameters filterParameters(DisplayFilterDialog());
	if (filterParameters.order == 0)
		return;

	// Create new dataset containing the FFT of dataset and add it to the plot
	for (const auto& row : selectedRows)
	{
		std::unique_ptr<Dataset2D> newData(std::make_unique<Dataset2D>(*mPlotList[row - 1]));

		ApplyFilter(filterParameters, newData);

		wxString name = FilterDialog::GetFilterNamePrefix(filterParameters)
			+ _T(" (") + mGrid->GetCellValue(row, static_cast<int>(PlotListGrid::Column::Name)) + _T(")");
		AddCurve(std::move(newData), name);
	}
}

//=============================================================================
// Class:			GuiInterface
// Function:		FitCurves
//
// Description:		Fits a curve to the dataset selected in the mGrid control.
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
//=============================================================================
void GuiInterface::FitCurves(const wxArrayInt& selectedRows)
{
	// Ask the user what order to use for the polynomial
	unsigned long order;
	wxString orderString = ::wxGetTextFromUser(_T("Specify the order of the polynomial fit:"),
		_T("Polynomial Curve Fit"), _T("2"), mOwner);

	// If cancelled, the orderString will be empty.  It is possible that the user cleared the textbox
	// and clicked OK, but we'll ignore this case since we can't tell the difference
	if (orderString.IsEmpty())
		return;

	if (!orderString.ToULong(&order))
	{
		wxMessageBox(_T("ERROR:  Order must be a positive integer!"), _T("Error Fitting Curve"), wxICON_ERROR, mOwner);
		return;
	}

	for (const auto& row : selectedRows)
	{
		wxString name;
		std::unique_ptr<Dataset2D> newData(GetCurveFitData(
			order, mPlotList[row - 1], name, row));

		AddCurve(std::move(newData), name);
	}
}

//=============================================================================
// Class:			GuiInterface
// Function:		GetCurveFitData
//
// Description:		Fits a curve of the specified order to the specified data
//					and returns a dataset containing the curve.
//
// Input Arguments:
//		order	= const unsigned int&
//		data	= const std::unique_ptr<const Dataset2D>&
//		row		= const unsigned int&
//
// Output Arguments:
//		name	= wxString&
//
// Return Value:
//		std::unique_ptr<Dataset2D>
//
//=============================================================================
std::unique_ptr<Dataset2D> GuiInterface::GetCurveFitData(const unsigned int &order,
	const std::unique_ptr<const Dataset2D>& data, wxString &name, const unsigned int& row) const
{
	CurveFit::PolynomialFit fitData = CurveFit::DoPolynomialFit(*data, order);

	std::unique_ptr<Dataset2D> newData(std::make_unique<Dataset2D>(*data));
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); ++i)
		newData->GetY()[i] = CurveFit::EvaluateFit(newData->GetX()[i], fitData);

	name = GetCurveFitName(fitData, row);

	return newData;
}

//=============================================================================
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
//=============================================================================
wxString GuiInterface::GetCurveFitName(const CurveFit::PolynomialFit &fitData,
	const unsigned int &row) const
{
	wxString name, termString;
	//name.Printf("Order %lu Fit([%i]), R^2 = %0.2f", order, row, fitData.rSquared);
	name.Printf("Fit [%i] (R^2 = %0.2f): ", row, fitData.rSquared);

	unsigned int i;
	for (i = 0; i <= fitData.order; ++i)
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

//=============================================================================
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
//=============================================================================
void GuiInterface::DisplayMathChannelDialog(wxString defaultInput)
{
	// Display input dialog in which user can specify the math desired
	wxString message(_T("Enter the math you would like to perform:\n\n"));
	message.Append(_T("    Use [x] notation to specify channels, where x = 0 is Time, x = 1 is the first data channel, etc.\n"));
	message.Append(_T("    Valid operations are: +, -, *, /, %, ddt, int, fft and trigonometric functions.\n"));
	message.Append(_T("    Use () to specify order of operations"));

	AddCurve(::wxGetTextFromUser(message, _T("Specify Math Channel"), defaultInput, mOwner));
}

//=============================================================================
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
//=============================================================================
void GuiInterface::DisplayAxisRangeDialog(const PlotRenderer::PlotContext &axis)
{
	double min, max;
	if (!mRenderer->GetCurrentAxisRange(axis, min, max))
		return;

	RangeLimitsDialog dialog(mOwner, min, max);
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
		wxMessageBox(_T("ERROR:  Limits must unique!"), _T("Error Setting Limits"), wxICON_ERROR, mOwner);
		return;
	}

	mRenderer->SetNewAxisRange(axis, min, max);
	mRenderer->SaveCurrentZoom();
}

//=============================================================================
// Class:			GuiInterface
// Function:		ForceEqualAxisScaling
//
// Description:		Forces x- and y-axis scaling to be equal
//
// Input Arguments:
//		scalingEqual	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void GuiInterface::ForceEqualAxisScaling(const bool& scalingEqual)
{
	mRenderer->SetEqualScaling(scalingEqual);
}

//=============================================================================
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
//=============================================================================
FilterParameters GuiInterface::DisplayFilterDialog()
{
	FilterDialog dialog(mRenderer);
	if (dialog.ShowModal() != wxID_OK)
	{
		FilterParameters parameters;
		parameters.order = 0;
		return parameters;
	}

	return dialog.GetFilterParameters();
}

//=============================================================================
// Class:			GuiInterface
// Function:		ApplyFilter
//
// Description:		Applies the specified filter to the specified dataset.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//		data		= const std::unique_ptr<Dataset2D>&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void GuiInterface::ApplyFilter(const FilterParameters &parameters,
	const std::unique_ptr<Dataset2D>& data)
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Cutoff frequency may be incorrect!"),
			_T("Accuracy Warning"), wxICON_WARNING, mOwner);

	if (!PlotMath::XDataConsistentlySpaced(*data))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, mOwner);

	std::unique_ptr<Filter> filter(GetFilter(
		parameters, factor / data->GetAverageDeltaX(), data->GetY()[0]));

	for (auto &y : data->GetY())
		y = filter->Apply(y);

	// For phaseless filter, re-apply the same filter backwards
	if (parameters.phaseless)
	{
		data->Reverse();
		filter->Initialize(data->GetY()[0]);
		for (auto &y : data->GetY())
			y = filter->Apply(y);
		data->Reverse();
	}
}

//=============================================================================
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
//		std::unique_ptr<Filter>
//
//=============================================================================
std::unique_ptr<Filter> GuiInterface::GetFilter(const FilterParameters &parameters,
	const double &sampleRate, const double &initialValue) const
{
	return std::make_unique<Filter>(sampleRate,
		Filter::CoefficientsFromString(std::string(parameters.numerator.mb_str())),
		Filter::CoefficientsFromString(std::string(parameters.denominator.mb_str())),
		initialValue);
}

//=============================================================================
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
//=============================================================================
void GuiInterface::UpdateLegend()
{
	if (!mGrid)// TODO:  Eliminate need for this check
		return;

	double lineSize;
	long markerSize;
	std::vector<LibPlot2D::Legend::LegendEntryInfo> entries;
	LibPlot2D::Legend::LegendEntryInfo info;
	int i;
	for (i = 1; i < mGrid->GetNumberRows(); ++i)
	{
		if (!CurveIsVisible(i - 1))
			continue;

		mGrid->GetCellValue(i, static_cast<int>(PlotListGrid::Column::LineSize)).ToDouble(&lineSize);
		info.lineSize = lineSize;
		info.color = LibPlot2D::Color(mGrid->GetCellBackgroundColour(i, static_cast<int>(PlotListGrid::Column::Color)));
		info.text = mGrid->GetCellValue(i, static_cast<int>(PlotListGrid::Column::Name));

		mGrid->GetCellValue(i, static_cast<int>(PlotListGrid::Column::MarkerSize)).ToLong(&markerSize);
		if (mRenderer->CurveMarkersVisible(i - 1))
				info.markerSize = std::max(markerSize, 1L);
		else
			info.markerSize = markerSize;

		entries.push_back(info);
	}
	mRenderer->UpdateLegend(entries);
	mRenderer->UpdateDisplay();
}

//=============================================================================
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
//=============================================================================
void GuiInterface::SetMarkerSize(const unsigned int &curve, const int &size)
{
	mGrid->SetCellValue(curve + 1, static_cast<int>(PlotListGrid::Column::MarkerSize), wxString::Format("%i", size));
	UpdateCurveProperties(curve);
}

//=============================================================================
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
//=============================================================================
void GuiInterface::UpdateCurveQuality()
{
	//if (mRenderer->GetTotalPointCount() > highQualityCurvePointLimit)
		mRenderer->SetCurveQuality(LibPlot2D::PlotRenderer::CurveQuality::HighWrite);
	/*else
		mRenderer->SetCurveQuality(static_cast<PlotRenderer::CurveQuality>(
		PlotRenderer::CurveQuality::HighStatic | PlotRenderer::CurveQuality::HighWrite));*/// TODO:  Fix this after line rendering is improved
}

//=============================================================================
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
//=============================================================================
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

//=============================================================================
// Class:			GuiInterface
// Function:		GetFFTData
//
// Description:		Returns a dataset containing an FFT of the specified data.
//
// Input Arguments:
//		data	= const std::unique_ptr<const Dataset2D>&
//
// Output Arguments:
//		None
//
// Return Value:
//		std::unique_ptr<Dataset2D>
//
//=============================================================================
std::unique_ptr<Dataset2D> GuiInterface::GetFFTData(
	const std::unique_ptr<const Dataset2D>& data)
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, mOwner);

	LibPlot2D::FFTDialog dialog(mOwner, data->GetNumberOfPoints(),
		data->GetNumberOfZoomedPoints(mRenderer->GetXMin(), mRenderer->GetXMax()),
		data->GetAverageDeltaX() / factor);

	if (dialog.ShowModal() != wxID_OK)
		return nullptr;

	if (!LibPlot2D::PlotMath::XDataConsistentlySpaced(*data))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, mOwner);

	std::unique_ptr<Dataset2D> newData(std::move(
		FastFourierTransform::ComputeFFT([&dialog, &data, this]()
	{
		if (dialog.GetUseZoomedData())
			return *GetXZoomedDataset(data);
		else
			return *data;
	}(), dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap(),
		dialog.GetSubtractMean())));

	newData->MultiplyXData(factor);

	return newData;
}

//=============================================================================
// Class:			GuiInterface
// Function:		GetXZoomedDataset
//
// Description:		Returns a dataset containing only the data within the
//					current zoomed x-limits.
//
// Input Arguments:
//		fullData	= const std::unique_ptr<const Dataset2D>&
//
// Output Arguments:
//		None
//
// Return Value:
//		std::unique_ptr<Dataset2D>
//
//=============================================================================
std::unique_ptr<Dataset2D> GuiInterface::GetXZoomedDataset(
	const std::unique_ptr<const Dataset2D>& fullData) const
{
	unsigned int i, startIndex(0), endIndex(0);
	while (fullData->GetX()[startIndex] < mRenderer->GetXMin() &&
		startIndex < fullData->GetNumberOfPoints())
		++startIndex;
	endIndex = startIndex;
	while (fullData->GetX()[endIndex] < mRenderer->GetXMax() &&
		endIndex < fullData->GetNumberOfPoints())
		++endIndex;

	std::unique_ptr<Dataset2D> data(std::make_unique<Dataset2D>(endIndex - startIndex));
	for (i = startIndex; i < endIndex; ++i)
	{
		data->GetX()[i - startIndex] = fullData->GetX()[i];
		data->GetY()[i - startIndex] = fullData->GetY()[i];
	}

	return data;
}

//=============================================================================
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
//=============================================================================
void GuiInterface::ShowAppropriateXLabel()
{
	if (!mGrid)// TODO:  Eliminate need for this check
		return;

	// If the only visible curves are frequency plots, change the x-label
	int i;
	bool showFrequencyLabel(false);
	for (i = 1; i < mGrid->GetNumberRows(); ++i)
	{
		if (mGrid->GetCellValue(i, static_cast<int>(PlotListGrid::Column::Visible)).Cmp(_T("1")) == 0)
		{
			if (mGrid->GetCellValue(i, static_cast<int>(PlotListGrid::Column::Name)).Mid(0, 3).CmpNoCase(_T("FFT")) == 0 ||
				mGrid->GetCellValue(i, static_cast<int>(PlotListGrid::Column::Name)).Mid(0, 3).CmpNoCase(_T("FRF")) == 0)
				showFrequencyLabel = true;
			else
			{
				showFrequencyLabel = false;
				break;
			}
		}
	}

	if (showFrequencyLabel)
		SetXDataLabel(FileFormat::Frequency);
	else
		SetXDataLabel(mCurrentFileFormat);
}

//=============================================================================
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
//=============================================================================
bool GuiInterface::GetXAxisScalingFactor(double &factor, wxString *label)
{
	if (XScalingFactorIsKnown(factor, label))
		return true;

	wxString unit = ExtractUnitFromDescription(mGenericXAxisLabel);

	unit = unit.Trim().Trim(false);
	if (label)
		label->assign(unit);

	return UnitStringToFactor(unit, factor);
}

//=============================================================================
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
//=============================================================================
bool GuiInterface::XScalingFactorIsKnown(double &factor, wxString *label) const
{
	if (mCurrentFileFormat == FileFormat::Baumuller)
	{
		factor = 1000.0;
		if (label)
			label->assign(_T("msec"));
		return true;
	}
	else if (mCurrentFileFormat == FileFormat::Kollmorgen)
	{
		factor = 1.0;
		if (label)
			label->assign(_T("sec"));
		return true;
	}

	return false;
}

//=============================================================================
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
//=============================================================================
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

	for (const auto& delimiter : delimiters)
	{
		int location(description.Find(delimiter.mb_str()));
		if (location != wxNOT_FOUND && location < static_cast<int>(description.Len()) - 1)
		{
			unit = description.Mid(location + 1);
			break;
		}
	}

	return unit;
}

//=============================================================================
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
//=============================================================================
bool GuiInterface::FindWrappedString(const wxString &s, wxString &contents,
	const wxChar &open, const wxChar &close)
{
	if (s.Len() < 3)
		return false;

	if (s.Last() == close)
	{
		int i;
		for (i = s.Len() - 2; i >= 0; --i)
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

//=============================================================================
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
//=============================================================================
void GuiInterface::SetXDataLabel(wxString label)
{
	mRenderer->SetXLabel(label);
	if (!mGrid || mGrid->GetNumberRows() == 0)// TODO:  Eliminate need for this check
		return;

	mGrid->SetCellValue(0, static_cast<int>(PlotListGrid::Column::Name), label);
}

//=============================================================================
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
//=============================================================================
void GuiInterface::SetXDataLabel(const FileFormat &format)
{
	switch (format)
	{
	case FileFormat::Frequency:
		SetXDataLabel(_T("Frequency [Hz]"));
		break;

	default:
	case FileFormat::Generic:
	case FileFormat::Kollmorgen:
	case FileFormat::Baumuller:
		SetXDataLabel(mGenericXAxisLabel);
	}
}

//=============================================================================
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
//=============================================================================
void GuiInterface::UpdateCurveProperties(const unsigned int &index)
{
	if (!mGrid)// TODO:  Eliminate need for this check
		return;

	Color color;
	color.Set(mGrid->GetCellBackgroundColour(index + 1, static_cast<int>(PlotListGrid::Column::Color)));
	UpdateCurveProperties(index, color, CurveIsVisible(index), CurveIsOnRightAxis(index));
}

//=============================================================================
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
//=============================================================================
void GuiInterface::UpdateCurveProperties(const unsigned int &index,
	const Color &color, const bool &visible, const bool &rightAxis)
{
	if (!mGrid)// TODO:  Eliminate need for this check
		return;

	double lineSize;
	long markerSize;
	mGrid->GetCellValue(index + 1, static_cast<int>(PlotListGrid::Column::LineSize)).ToDouble(&lineSize);
	mGrid->GetCellValue(index + 1, static_cast<int>(PlotListGrid::Column::MarkerSize)).ToLong(&markerSize);
	mRenderer->SetCurveProperties(index, color, visible, rightAxis, lineSize, markerSize);

	UpdateLegend();

	mRenderer->SaveCurrentZoom();
}

//=============================================================================
// Class:			GuiInterface
// Function:		Copy
//
// Description:		Copies the plot area image to the clipboard.
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
void GuiInterface::Copy()
{
	mRenderer->DoCopy();
}

//=============================================================================
// Class:			GuiInterface
// Function:		Paste
//
// Description:		Pastes from the clipboard.
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
void GuiInterface::Paste()
{
	mRenderer->DoPaste();
}

//=============================================================================
// Class:			GuiInterface
// Function:		CurveIsVisible
//
// Description:		Returns true if the specified curve is visible.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//=============================================================================
bool GuiInterface::CurveIsVisible(const unsigned int& i) const
{
	const wxString visibilityValue(mGrid->GetCellValue(i + 1, static_cast<int>(PlotListGrid::Column::Visible)));
	return !(visibilityValue.IsEmpty() || visibilityValue.Cmp(_T("0")) == 0);
}

//=============================================================================
// Class:			GuiInterface
// Function:		CurveIsOnRightAxis
//
// Description:		Returns true if the specified curve is plotted against the
//					right-hand axis.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//=============================================================================
bool GuiInterface::CurveIsOnRightAxis(const unsigned int& i) const
{
	const wxString rightAxisValue(mGrid->GetCellValue(i + 1, static_cast<int>(PlotListGrid::Column::RightAxis)));
	return !(rightAxisValue.IsEmpty() || rightAxisValue.Cmp(_T("0")) == 0);
}

}// namespace LibPlot2D
