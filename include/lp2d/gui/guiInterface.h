/*=============================================================================
                                   LibPlot2D
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
#include "lp2d/parser/fileTypeManager.h"

// Standard C++ headers
#include <memory>
#include <type_traits>

// wxWidgets forward declarations
class wxArrayString;
class wxString;
class wxWindow;
class wxFrame;

namespace LibPlot2D
{

class Filter;
struct FilterParameters;

/// Class for managing interactions between different GUI components.
class GuiInterface
{
public:
	/// Constructor.
	///
	/// \param owner Object owning the associated GUI components.
	explicit GuiInterface(wxFrame* owner);

	/// Loads the specified files.  When the first file is loaded, user will be
	/// prompted to select which data channels to extract.  If the file format
	/// (including header rows, etc.) of subsequent files is the same, the same
	/// extraction options will be used.  If the file format or file header
	/// changes, the user will be prompted to make extraction selections again.
	///
	/// \param fileList List of files to load.
	///
	/// \returns True if all files were successfully loaded.
	bool LoadFiles(const wxArrayString &fileList);

	/// Loads the specified text data as if it were read from a file.
	///
	/// \param data Text data to parse.
	///
	/// \returns True if the data was successfully loaded.
	bool LoadText(const wxString& data);

	/// Reloads the last set of data that was loaded with a single
	/// drag-and-drop or Open button click.
	void ReloadData();

	void Copy();///< Copies rendered image to clipboard.
	void Paste();///< Pastes delimited text as curve data.

	/// Updates the y-values associated with the intersections of plot cursors
	/// and plot curves.
	///
	/// \param leftVisible  True if the left cursor is visible in the plot
	///                     area.
	/// \param rightVisible True if the left cursor is visible in the plot
	///                     area.
	/// \param leftValue    The x-value at which the left cursor intersects the
	///                     bottom axis.
	/// \param rightValue   The x-value at which the right cursor intersects
	///                     the bottom axis.
	void UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue);

	/// Adds a new curve to the plot, based on the specified mathematical
	/// expression.
	///
	/// \param mathString Expression to evaluate to create the new curve.
	void AddCurve(wxString mathString);

	/// Adds a new curve representing the specified data set.
	///
	/// \param data Dataset for which the curve will be generated.
	/// \param name Name to display to identify the curve.
	void AddCurve(std::unique_ptr<Dataset2D> data, wxString name);

	/// Removes the specified curve from the plot.
	///
	/// \param i Index of the curve to remove.
	void RemoveCurve(const unsigned int &i);

	/// Removes the specified curves from the plot.
	///
	/// \param curves List of curve indices to remove.
	void RemoveCurves(wxArrayInt curves);

	/// Removes all curves which are currently selected in the plot list grid.
	void RemoveSelectedCurves();

	void ClearAllCurves();///< Removes all curves from the plot.

	/// Sets the text to display in the main frame's title bar.
	///
	/// \param title Text to display.
	void SetApplicationTitle(const wxString& title) { mApplicationTitle = title; }

	/// Enumeration for identifying known file formats that require special
	/// handling of axis labels, etc.
	enum class FileFormat
	{
		Baumuller,
		Kollmorgen,
		Frequency,///< Indicates that the x-axis units should be Hz.
		Generic
	};

	/// Adds a descendant of DataFile to the list of recognizable file types.
	template<typename T>
	void RegisterFileType();

	/// Registers all file types that are included with LibPlot2D.
	void RegisterAllBuiltInFileTypes();

	/// Gets the flag indicating the current file format.
	/// \returns The flag indicating the current file format.
	FileFormat GetCurrentFileFormat() const { return mCurrentFileFormat; }

	/// Forces a refresh of the renderer.
	void RefreshRenderer() { mRenderer->Refresh(); }

	/// Displays a dialog allowing the user to specify the range of value for
	/// an axis.
	///
	/// \param axis Context enumeration describing which axis to modify.
	void DisplayAxisRangeDialog(const PlotRenderer::PlotContext &axis);

	/// Displays a dialog allowing the user to specify an expression string,
	/// from which a new Dataset2D can be generated.
	///
	/// \param defaultInput Initial value of the expression string.
	void DisplayMathChannelDialog(wxString defaultInput = wxEmptyString);

	void ExportData();///< Exports the plotted data to a column-delimited file.

	/// Displays a dialog allowing the user to generate a curve representing a
	/// frequency response function.
	void GenerateFRF();

	/// Displays a dialog allowing the user to generate a new signal.
	void CreateSignal();

	/// Prompts the user to enter a string describing the current units for the
	/// x-axis.
	void SetTimeUnits();

	/// \name Methods for generating new curves by modifying existing data sets.
	/// @{

	void ScaleXData(const wxArrayInt& selectedRows);
	void PlotDerivative(const wxArrayInt& selectedRows);
	void PlotIntegral(const wxArrayInt& selectedRows);
	void PlotRMS(const wxArrayInt& selectedRows);
	void PlotFFT(const wxArrayInt& selectedRows);
	void TimeShift(const wxArrayInt& selectedRows);
	void BitMask(const wxArrayInt& selectedRows);
	void FilterCurves(const wxArrayInt& selectedRows);
	void FitCurves(const wxArrayInt& selectedRows);

	/// @}

	/// Gets a prunned data set containing only the points that lie within the
	/// current range of the x-axis.
	///
	/// \param fullData Complete data set.
	///
	/// \returns A new, prunned, data set.
	std::unique_ptr<Dataset2D> GetXZoomedDataset(
		const std::unique_ptr<const Dataset2D>& fullData) const;

	// TODO:  This x-axis label stuff needs to be cleaned up.

	/// Sets the x-axis label to the specified string.
	///
	/// \param label X-axis label.
	void SetXDataLabel(wxString label);

	/// Sets the x-axis label according to the specified file format.
	///
	/// \param format Desired file format.
	void SetXDataLabel(const FileFormat &format);

	/// Makes a guess as to the appropriate label for the x-axis.
	void ShowAppropriateXLabel();

	/// Updates the properties of the specified curve.
	///
	/// \param index Index of curve to update.
	void UpdateCurveProperties(const unsigned int &index);
	void UpdateLegend();///< Forces an update of the plot legen.

	/// Computes a scale factor for converting time data to or from seconds.
	///
	/// \param unit         String describing the desired units.
	/// \param factor [out] Scale factor.
	///
	/// \returns True if the \p unit string was recognized.
	static bool UnitStringToFactor(const wxString &unit, double &factor);

private:
	wxFrame* mOwner;

	void SetPlotListGrid(PlotListGrid* grid) { mGrid = grid; }
	void SetRenderWindow(PlotRenderer* renderer) { mRenderer = renderer; }

	friend PlotRenderer::PlotRenderer(GuiInterface& guiInterface,
		wxWindow &wxParent, wxWindowID id, const wxGLAttributes& attr);
	friend PlotListGrid::PlotListGrid(GuiInterface& guiInterface,
		wxWindow* parent, wxWindowID id);

	ManagedList<const Dataset2D> plotList;

	PlotListGrid* mGrid = nullptr;
	PlotRenderer* mRenderer = nullptr;

	wxString mApplicationTitle;

	wxString GenerateTemporaryFileName(const unsigned int &length = 10) const;

	wxArrayString mLastFilesLoaded;
	DataFile::SelectionData mLastSelectionInfo;
	wxArrayString mLastDescriptions;

	FileTypeManager mFileTypeManager;

	FileFormat mCurrentFileFormat = FileFormat::Generic;
	wxString mGenericXAxisLabel;

	bool GetXAxisScalingFactor(double &factor, wxString *label = nullptr);

	void UpdateSingleCursorValue(const unsigned int &row, double value,
		const PlotListGrid::Column &column, const bool &isVisible);

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

	std::unique_ptr<Filter> GetFilter(const FilterParameters &parameters,
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


template<typename T>
void GuiInterface::RegisterFileType()
{
	static_assert(std::is_base_of<DataFile, T>::value,
		"T must be a descendant of DataFile");
	mFileTypeManager.RegisterFileType(T::IsType, DataFile::Create<T>);
}

}// namespace LibPlot2D

#endif// GUI_INTERFACE_H_
