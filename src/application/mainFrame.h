/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  mainFrame.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Main frame for the application.
// History:

#ifndef _MAIN_FRAME_H_
#define _MAIN_FRAME_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/dataset2D.h"
#include "utilities/managedList.h"
#include "utilities/signals/curveFit.h"
#include "application/dataFiles/dataFile.h"

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;

// Local forward declarations
class PlotRenderer;
struct FilterParameters;
class CustomFileFormat;
class Color;
class Filter;
class DataFile;

// The main frame class
class MainFrame : public wxFrame
{
public:
	// Constructor
	MainFrame();

	// Destructor
	~MainFrame();

	// For getting a open/save file name from the user
	wxArrayString GetFileNameFromUser(wxString dialogTitle, wxString defaultDirectory,
		wxString defaultFileName, wxString wildcard, long style);

	// Load a plot from file
	bool LoadFile(const wxString &pathAndFileName);
	bool LoadFiles(const wxArrayString &fileList);
	bool LoadText(const wxString &textData);

	enum PlotContext
	{
		plotContextXAxis,
		plotContextLeftYAxis,
		plotContextRightYAxis,
		plotContextPlotArea
	};

	bool GetXAxisScalingFactor(double &factor, wxString *label = NULL);
	static bool UnitStringToFactor(const wxString &unit, double &factor);

	void CreatePlotContextMenu(const wxPoint &position, const PlotContext &context);
	void DisplayAxisRangeDialog(const PlotContext &axis);

	void UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue);

private:
	// Functions that do some of the frame initialization and control positioning
	void CreateControls(void);
	void SetProperties(void);

	PlotRenderer* CreatePlotArea(wxWindow *parent);
	wxGrid* CreateOptionsGrid(wxWindow *parent);
	wxBoxSizer* CreateButtons(wxWindow *parent);

	// Controls
	wxGrid *optionsGrid;
	PlotRenderer *plotArea;

	enum Columns
	{
		colName = 0,
		colColor,
		colLineSize,
		colMarkerSize,
		colLeftCursor,
		colRightCursor,
		colDifference,
		colVisible,
		colRightAxis,

		colCount
	};

	// For displaying a menu that was crated by this form
	// NOTE:  When calculating the Position to display this context menu,
	// consider that the coordinates for the calling object might be different
	// from the coordinates for this object!
	void CreateGridContextMenu(const wxPoint &position, const unsigned int &row);

	void ClearAllCurves(void);
	void AddCurve(wxString mathString);
	void AddCurve(Dataset2D *data, wxString name);
	void RemoveCurve(const unsigned int &i);

	Color GetNextColor(const unsigned int &index) const;
	void AddTimeRowToGrid(void);
	unsigned int AddDataRowToGrid(const wxString &name);

	void UpdateCurveProperties(const unsigned int &index, const Color &color,
		const bool &visible, const bool &rightAxis);
	void UpdateCurveProperties(const unsigned int &index);

	ManagedList<const Dataset2D> plotList;

	// The event IDs
	enum MainFrameEventID
	{
		idButtonOpen = wxID_HIGHEST + 100,
		idButtonAutoScale,
		idButtonRemoveCurve,
		idButtonReloadData,

		idContextAddMathChannel,
		idContextFRF,

		idContextCreateSignal,

		idContextSetTimeUnits,
		idContextScaleXData,
		idContextPlotDerivative,
		idContextPlotIntegral,
		idContextPlotRMS,
		idContextPlotFFT,
		idContextTimeShift,
		idContextBitMask,

		idContextFilter,
		idContextFitCurve,

		idPlotContextCopy,
		idPlotContextPaste,
		idPlotContextToggleGridlines,
		idPlotContextAutoScale,
		idPlotContextWriteImageFile,
		idPlotContextExportData,

		idPlotContextBGColor,
		idPlotContextGridColor,

		idPlotContextToggleBottomGridlines,// Maintain this order for each axis' context IDs
		idPlotContextAutoScaleBottom,
		idPlotContextSetBottomRange,
		idPlotContextSetBottomLogarithmic,
		idPlotContextEditBottomLabel,

		/*idPlotContextToggleTopGridlines,
		idPlotContextAutoScaleTop,
		idPlotContextSetTopRange,
		idPlotContextSetTopLogarithmic,*/

		idPlotContextToggleLeftGridlines,
		idPlotContextAutoScaleLeft,
		idPlotContextSetLeftRange,
		idPlotContextSetLeftLogarithmic,
		idPlotContextEditLeftLabel,

		idPlotContextToggleRightGridlines,
		idPlotContextAutoScaleRight,
		idPlotContextSetRightRange,
		idPlotContextSetRightLogarithmic,
		idPlotContextEditRightLabel
	};

	wxMenu *CreateAxisContextMenu(const unsigned int &baseEventId) const;
	wxMenu *CreatePlotAreaContextMenu(void) const;

	// Event handlers-----------------------------------------------------
	// Buttons
	void ButtonOpenClickedEvent(wxCommandEvent &event);
	void ButtonAutoScaleClickedEvent(wxCommandEvent &event);
	void ButtonRemoveCurveClickedEvent(wxCommandEvent &event);
	void ButtonReloadDataClickedEvent(wxCommandEvent &event);

	// Grid events
	void GridRightClickEvent(wxGridEvent &event);
	void GridDoubleClickEvent(wxGridEvent &event);
	void GridLeftClickEvent(wxGridEvent &event);
	void GridCellChangeEvent(wxGridEvent &event);
	void GridLabelRightClickEvent(wxGridEvent &event);

	// Context menu events
	void ContextAddMathChannelEvent(wxCommandEvent &event);
	void ContextFRFEvent(wxCommandEvent &event);
	void ContextCreateSignalEvent(wxCommandEvent &event);
	void ContextSetTimeUnitsEvent(wxCommandEvent &event);
	void ContextScaleXDataEvent(wxCommandEvent &event);
	void ContextPlotDerivativeEvent(wxCommandEvent &event);
	void ContextPlotIntegralEvent(wxCommandEvent &event);
	void ContextPlotRMSEvent(wxCommandEvent &event);
	void ContextPlotFFTEvent(wxCommandEvent &event);
	void ContextTimeShiftEvent(wxCommandEvent &event);
	void ContextBitMaskEvent(wxCommandEvent &event);

	void ContextFilterEvent(wxCommandEvent &event);
	void ContextFitCurve(wxCommandEvent &event);

	void ContextCopy(wxCommandEvent &event);
	void ContextPaste(wxCommandEvent &event);
	void ContextToggleGridlines(wxCommandEvent &event);
	void ContextAutoScale(wxCommandEvent &event);
	void ContextWriteImageFile(wxCommandEvent &event);
	void ContextExportData(wxCommandEvent &event);

	void ContextPlotBGColor(wxCommandEvent &event);
	void ContextGridColor(wxCommandEvent &event);

	void ContextToggleGridlinesBottom(wxCommandEvent &event);
	void ContextAutoScaleBottom(wxCommandEvent &event);
	void ContextSetRangeBottom(wxCommandEvent &event);
	void ContextSetLogarithmicBottom(wxCommandEvent &event);
	void ContextEditBottomLabel(wxCommandEvent &event);

	void ContextToggleGridlinesLeft(wxCommandEvent &event);
	void ContextAutoScaleLeft(wxCommandEvent &event);
	void ContextSetRangeLeft(wxCommandEvent &event);
	void ContextSetLogarithmicLeft(wxCommandEvent &event);
	void ContextEditLeftLabel(wxCommandEvent &event);

	void ContextToggleGridlinesRight(wxCommandEvent &event);
	void ContextAutoScaleRight(wxCommandEvent &event);
	void ContextSetRangeRight(wxCommandEvent &event);
	void ContextSetLogarithmicRight(wxCommandEvent &event);
	void ContextEditRightLabel(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	void SetMarkerSize(const unsigned int &curve, const int &size);

	void ShowAppropriateXLabel(void);

	void DisplayMathChannelDialog(wxString defaultInput = wxEmptyString);
	FilterParameters DisplayFilterDialog(void);
	void ApplyFilter(const FilterParameters &parameters, Dataset2D &data);

	bool XScalingFactorIsKnown(double &factor, wxString *label) const;
	static wxString ExtractUnitFromDescription(const wxString &description);
	static bool FindWrappedString(const wxString &s, wxString &contents,
		const wxChar &open, const wxChar &close);

	Dataset2D *GetFFTData(const Dataset2D* data);
	Dataset2D *GetCurveFitData(const unsigned int &order, const Dataset2D* data, wxString &name) const;
	wxString GetCurveFitName(const CurveFit::PolynomialFit &fitData, const unsigned int &row) const;

	void AddFFTCurves(const double& xFactor, Dataset2D *amplitude, Dataset2D *phase,
		Dataset2D *coherence, const wxString &namePortion);

	void UpdateSingleCursorValue(const unsigned int &row, double value,
		const unsigned int &column, const bool &isVisible);

	bool GetCurrentAxisRange(const PlotContext &axis, double &min, double &max) const;
	void SetNewAxisRange(const PlotContext &axis, const double &min, const double &max);

	Filter* GetFilter(const FilterParameters &parameters,
		const double &sampleRate, const double &initialValue) const;

	DataFile* GetDataFile(const wxString &fileName);

	enum FileFormat
	{
		FormatBaumuller,
		FormatKollmorgen,
		FormatFrequency,
		FormatGeneric
	};

	FileFormat currentFileFormat;

	void SetTitleFromFileName(wxString pathAndFileName);
	void SetXDataLabel(wxString label);
	void SetXDataLabel(const FileFormat &format);
	wxString genericXAxisLabel;

	wxString ExtractFileNameFromPath(const wxString &pathAndFileName) const;

	wxString lastFileLoaded;
	DataFile::SelectionData lastSelectionInfo;
	wxArrayString lastDescriptions;

	Dataset2D GetXZoomedDataset(const Dataset2D &fullData) const;

	wxString GenerateTemporaryFileName(const unsigned int &length = 10) const;

	void DoCopy(void);
	void DoPaste(void);

	DECLARE_EVENT_TABLE();

	// Testing methods
	//void TestSignalOperations(void);
};

#endif// _MAIN_FRAME_H_