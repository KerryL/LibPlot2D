/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainFrame.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Main frame for the application.
// History:

#ifndef _MAIN_FRAME_H_
#define _MAIN_FRAME_H_

// Standard C++ headers
#include <fstream>
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/dataset2D.h"
#include "utilities/managedList.h"
#include "utilities/signals/curveFit.h"

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;

// Local forward declarations
class PlotRenderer;
struct FilterParameters;
class CustomFileFormat;
class Color;
class FilterBase;

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
	bool LoadFile(wxString pathAndFileName);

	enum PlotContext
	{
		plotContextXAxis,
		plotContextLeftYAxis,
		plotContextRightYAxis,
		plotContextPlotArea
	};

	bool GetXAxisScalingFactor(double &factor, wxString *label = NULL);

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
	wxButton *openButton;
	wxButton *autoScaleButton;
	wxButton *removeCurveButton;
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

		idContextAddMathChannel,
		idContextFRF,
		idContextSetTimeUnits,
		idContextPlotDerivative,
		idContextPlotIntegral,
		idContextPlotRMS,
		idContextPlotFFT,
		idContextTimeShift,
		idContextBitMask,

		idContextFilter,

		idContextFitCurve,

		idPlotContextToggleGridlines,
		idPlotContextAutoScale,
		idPlotContextWriteImageFile,

		idPlotContextBGColor,
		idPlotContextGridColor,

		idPlotContextToggleBottomGridlines,// Maintain this order for each axis' context IDs
		idPlotContextAutoScaleBottom,
		idPlotContextSetBottomRange,
		idPlotContextSetBottomLogarithmic,

		/*idPlotContextToggleTopGridlines,
		idPlotContextAutoScaleTop,
		idPlotContextSetTopRange,
		idPlotContextSetTopLogarithmic,*/

		idPlotContextToggleLeftGridlines,
		idPlotContextAutoScaleLeft,
		idPlotContextSetLeftRange,
		idPlotContextSetLeftLogarithmic,

		idPlotContextToggleRightGridlines,
		idPlotContextAutoScaleRight,
		idPlotContextSetRightRange,
		idPlotContextSetRightLogarithmic
	};

	wxMenu *CreateAxisContextMenu(const unsigned int &baseEventId) const;
	wxMenu *CreatePlotAreaContextMenu(void) const;

	// Event handlers-----------------------------------------------------
	// Buttons
	void ButtonOpenClickedEvent(wxCommandEvent &event);
	void ButtonAutoScaleClickedEvent(wxCommandEvent &event);
	void ButtonRemoveCurveClickedEvent(wxCommandEvent &event);

	// Grid events
	void GridRightClickEvent(wxGridEvent &event);
	void GridDoubleClickEvent(wxGridEvent &event);
	void GridLeftClickEvent(wxGridEvent &event);
	void GridCellChangeEvent(wxGridEvent &event);

	// Context menu events
	void ContextAddMathChannelEvent(wxCommandEvent &event);
	void ContextFRFEvent(wxCommandEvent &event);
	void ContextSetTimeUnitsEvent(wxCommandEvent &event);
	void ContextPlotDerivativeEvent(wxCommandEvent &event);
	void ContextPlotIntegralEvent(wxCommandEvent &event);
	void ContextPlotRMSEvent(wxCommandEvent &event);
	void ContextPlotFFTEvent(wxCommandEvent &event);
	void ContextTimeShiftEvent(wxCommandEvent &event);
	void ContextBitMaskEvent(wxCommandEvent &event);

	void ContextFilterEvent(wxCommandEvent &event);

	void ContextFitCurve(wxCommandEvent &event);

	void ContextToggleGridlines(wxCommandEvent &event);
	void ContextAutoScale(wxCommandEvent &event);
	void ContextWriteImageFile(wxCommandEvent &event);

	void ContextPlotBGColor(wxCommandEvent &event);
	void ContextGridColor(wxCommandEvent &event);

	void ContextToggleGridlinesBottom(wxCommandEvent &event);
	void ContextAutoScaleBottom(wxCommandEvent &event);
	void ContextSetRangeBottom(wxCommandEvent &event);
	void ContextSetLogarithmicBottom(wxCommandEvent &event);

	void ContextToggleGridlinesLeft(wxCommandEvent &event);
	void ContextAutoScaleLeft(wxCommandEvent &event);
	void ContextSetRangeLeft(wxCommandEvent &event);
	void ContextSetLogarithmicLeft(wxCommandEvent &event);

	void ContextToggleGridlinesRight(wxCommandEvent &event);
	void ContextAutoScaleRight(wxCommandEvent &event);
	void ContextSetRangeRight(wxCommandEvent &event);
	void ContextSetLogarithmicRight(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	void SetMarkerSize(const unsigned int &curve, const int &size);

	void ShowAppropriateXLabel(void);

	void DisplayMathChannelDialog(wxString defaultInput = wxEmptyString);
	FilterParameters DisplayFilterDialog(void);
	void ApplyFilter(const FilterParameters &parameters, Dataset2D &data);

	bool XScalingFactorIsKnown(double &factor, wxString *label) const;
	bool UnitStringToFactor(const wxString &unit, double &factor) const;
	wxString ExtractUnitFromDescription(const wxString &description) const;
	bool FindWrappedString(const wxString &s, wxString &contents,
		const wxChar &open, const wxChar &close) const;

	Dataset2D *GetFFTData(const Dataset2D* data, const double &timeScalingFactor);
	Dataset2D *GetCurveFitData(const unsigned int &order, const Dataset2D* data, wxString &name) const;
	wxString GetCurveFitName(const CurveFit::PolynomialFit &fitData, const unsigned int &row) const;

	void UpdateSingleCursorValue(const unsigned int &row, double value,
		const unsigned int &column, const bool &isVisible);

	bool GetCurrentAxisRange(const PlotContext &axis, double &min, double &max) const;
	void SetNewAxisRange(const PlotContext &axis, const double &min, const double &max);

	FilterBase* GetFilter(const FilterParameters &parameters,
		const double &sampleRate, const double &initialValue) const;

	// Load file methods
	bool LoadCustomFile(wxString pathAndFileName, CustomFileFormat &customFormat);
	bool LoadTxtFile(wxString pathAndFileName);
	bool LoadCsvFile(wxString pathAndFileName);
	bool LoadGenericDelimitedFile(wxString pathAndFileName, CustomFileFormat *customFormat = NULL);
	bool LoadBaumullerFile(wxString pathAndFileName);
	bool LoadKollmorgenFile(wxString pathAndFileName);

	bool IsBaumullerFile(const wxString &pathAndFileName);
	bool IsKollmorgenFile(const wxString &pathAndFileName);

	bool ExtractData(std::ifstream &file, const wxString &delimiter, std::vector<double> *data,
		const wxArrayString &descriptions) const;
	void AddData(const std::vector<double> *data, const wxArrayString &descriptions,
		const double *timeStep = NULL, const std::vector<double> *scales = NULL);
	wxArrayString ParseLineIntoColumns(wxString line, const wxString &delimiter,
		const bool &ignoreConsecutiveDelimiters = true) const;
	unsigned int GetPopulatedCount(const wxArrayString &list) const;

	wxArrayString GetBaumullerDescriptions(std::ifstream &file, const wxString &delimiter) const;
	wxArrayString GetKollmorgenDescriptions(std::ifstream &file, const wxString &delimiter, double &samplingPeriod) const;
	wxArrayString GetGenericDescriptions(const wxString &fileName, const wxArrayString &delimiterList,
		wxString &delimiter, unsigned int &headerLines);

	void GenerateGenericNames(const wxArrayString &previousLines, const wxArrayString &currentLine,
		const wxString &delimiter, wxArrayString &descriptions) const;
	wxArrayString GenerateDummyNames(const unsigned int &count) const;
	bool ListIsNumeric(const wxArrayString &list) const;

	wxArrayString GetDelimiterList(const CustomFileFormat *customFormat) const;

	void CompensateGenericChoices(wxArrayInt &choices) const;
	void RemoveUnwantedDescriptions(wxArrayString &descriptions, const wxArrayInt &choices) const;

	bool ProcessGenericFile(const wxString &fileName, wxArrayString &descriptions,
		const unsigned int &headerLines, const wxString &delimiter, const std::vector<double> &scales);

	void SkipLines(std::ifstream &file, const unsigned int &count) const;

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

	Dataset2D GetXZoomedDataset(const Dataset2D &fullData) const;

	DECLARE_EVENT_TABLE();

	// Testing methods
	//void TestSignalOperations(void);
};

#endif// _MAIN_FRAME_H_