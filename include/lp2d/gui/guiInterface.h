/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  guiInterface.h
// Date:  12/9/2016
// Auth:  K. Loux
// Desc:  Manages interactions between plot grid and plot renderings, handles
//        context menus, etc.

#ifndef GUI_INTERFACE_H_

// Local headers
#include "lp2d/utilities/managedList.h"
#include "lp2d/utilities/dataset2D.h"
#include "lp2d/parser/dataFile.h"

// wxWidgets forward declarations
class wxArrayString;
class wxString;
class wxWindow;

namespace LibPlot2D
{

class GuiInterface
{
public:
	bool LoadFiles(const wxArrayString &filenames, wxWindow* parent);
	bool LoadText(const wxString& data, wxWindow* parent);
	void ReloadData();

	void UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue);

	void AddCurve(wxString mathString);
	void AddCurve(LibPlot2D::Dataset2D *data, wxString name);
	void RemoveCurve(const unsigned int &i);
	void ClearAllCurves();

private:
	ManagedList<const Dataset2D> plotList;

	wxString GenerateTemporaryFileName(const unsigned int &length = 10) const;
	wxString ExtractFileNameFromPath(const wxString &pathAndFileName) const;

	wxArrayString lastFilesLoaded;
	DataFile::SelectionData lastSelectionInfo;
	wxArrayString lastDescriptions;

	DataFile* GetDataFile(const wxString &fileName);

	enum FileFormat
	{
		FormatBaumuller,
		FormatKollmorgen,
		FormatFrequency,
		FormatGeneric
	};

	FileFormat currentFileFormat = FormatGeneric;
	wxString genericXAxisLabel;

	void UpdateSingleCursorValue(const unsigned int &row, double value,
		const unsigned int &column, const bool &isVisible);
};

}// namespace LibPlot2D

#endif// GUI_INTERFACE_H_
