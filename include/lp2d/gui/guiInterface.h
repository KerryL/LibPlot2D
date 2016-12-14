/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  guiInterface.h
// Date:  12/9/2016
// Auth:  K. Loux
// Desc:  Manages interactions between plot grid and plot renderings, handles
//        context menus, etc.

#ifndef GUI_INTERFACE_H_
#define GUI_INTERFACE_H_

// Local headers
#include "lp2d/utilities/managedList.h"
#include "lp2d/utilities/dataset2D.h"
#include "lp2d/parser/dataFile.h"
#include "lp2d/renderer/plotRenderer.h"
#include "lp2d/gui/plotListGrid.h"

// Standard C++ headers
#include <memory>

// wxWidgets forward declarations
class wxArrayString;
class wxString;
class wxWindow;
class wxFrame;

namespace LibPlot2D
{

class Filter;

class GuiInterface
{
public:
	GuiInterface(wxFrame* owner);

	bool LoadFiles(const wxArrayString &filenames);
	bool LoadText(const wxString& data);
	void ReloadData();

	void Copy();
	void Paste();

	void UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue);

	void AddCurve(wxString mathString);
	void AddCurve(std::unique_ptr<Dataset2D> data, wxString name);
	void RemoveCurve(const unsigned int &i);
	void RemoveCurves(wxArrayInt curves);
	void RemoveSelectedCurves();
	void ClearAllCurves();

	void SetApplicationTitle(const wxString& title) { applicationTitle = title; }

	enum FileFormat
	{
		FormatBaumuller,
		FormatKollmorgen,
		FormatFrequency,
		FormatGeneric
	};

	FileFormat GetCurrentFileFormat() const { return currentFileFormat; }

	void RefreshRenderer() { renderer->Refresh(); }

	void DisplayAxisRangeDialog(const PlotRenderer::PlotContext &axis);
	void DisplayMathChannelDialog(wxString defaultInput = wxEmptyString);

	void ExportData();

	void GenerateFRF();
	void CreateSignal();
	void SetTimeUnits();

	void ScaleXData(const wxArrayInt& selectedRows);
	void PlotDerivative(const wxArrayInt& selectedRows);
	void PlotIntegral(const wxArrayInt& selectedRows);
	void PlotRMS(const wxArrayInt& selectedRows);
	void PlotFFT(const wxArrayInt& selectedRows);
	void TimeShift(const wxArrayInt& selectedRows);
	void BitMask(const wxArrayInt& selectedRows);
	void FilterCurves(const wxArrayInt& selectedRows);
	void FitCurves(const wxArrayInt& selectedRows);

	std::unique_ptr<Dataset2D> GetXZoomedDataset(
		const std::unique_ptr<const Dataset2D>& fullData) const;

	void SetXDataLabel(wxString label);
	void SetXDataLabel(const FileFormat &format);
	void ShowAppropriateXLabel();

	void UpdateCurveProperties(const unsigned int &index);
	void UpdateLegend();

	static bool UnitStringToFactor(const wxString &unit, double &factor);

private:
	wxFrame* owner;

	void SetPlotListGrid(PlotListGrid* g) { grid = g; }
	void SetRenderWindow(PlotRenderer* r) { renderer = r; }

	friend PlotRenderer::PlotRenderer(GuiInterface& guiInterface,
		wxWindow &wxParent, wxWindowID id, const wxGLAttributes& attr);
	friend PlotListGrid::PlotListGrid(GuiInterface& guiInterface,
		wxWindow* parent, wxWindowID id);

	ManagedList<const Dataset2D> plotList;

	PlotListGrid* grid = nullptr;
	PlotRenderer* renderer = nullptr;

	wxString applicationTitle;

	wxString GenerateTemporaryFileName(const unsigned int &length = 10) const;

	wxArrayString lastFilesLoaded;
	DataFile::SelectionData lastSelectionInfo;
	wxArrayString lastDescriptions;

	DataFile* GetDataFile(const wxString &fileName);

	FileFormat currentFileFormat = FormatGeneric;
	wxString genericXAxisLabel;

	bool GetXAxisScalingFactor(double &factor, wxString *label = nullptr);

	void UpdateSingleCursorValue(const unsigned int &row, double value,
		const unsigned int &column, const bool &isVisible);

	FilterParameters DisplayFilterDialog();
	void ApplyFilter(const FilterParameters &parameters,
		const std::unique_ptr<Dataset2D>& data);

	std::unique_ptr<Dataset2D> GetCurveFitData(const unsigned int &order,
		const std::unique_ptr<const Dataset2D>& data, wxString &name,
		const unsigned int& row) const;
	wxString GetCurveFitName(const CurveFit::PolynomialFit &fitData,
		const unsigned int &row) const;

	std::unique_ptr<Dataset2D> GetFFTData(
		const std::unique_ptr<const Dataset2D>& data);

	Filter* GetFilter(const FilterParameters &parameters,
		const double &sampleRate, const double &initialValue) const;

	void AddFFTCurves(const double& xFactor,
		std::unique_ptr<Dataset2D> amplitude, std::unique_ptr<Dataset2D> phase,
		std::unique_ptr<Dataset2D> coherence, const wxString &namePortion);

	void AddMathChannel();

	void UpdateCurveQuality();

	void SetMarkerSize(const unsigned int &curve, const int &size);

	bool XScalingFactorIsKnown(double &factor, wxString *label) const;
	static wxString ExtractUnitFromDescription(const wxString &description);
	static bool FindWrappedString(const wxString &s, wxString &contents,
		const wxChar &open, const wxChar &close);

	void UpdateCurveProperties(const unsigned int &index,
		const Color &color, const bool &visible,
		const bool &rightAxis);
};

}// namespace LibPlot2D

#endif// GUI_INTERFACE_H_
