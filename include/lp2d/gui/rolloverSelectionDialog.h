/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  rolloverSelectionDialog.h
// Date:  7/8/2017
// Auth:  K. Loux
// Desc:  Dialog for specifying the point at which data wraps around.

#ifndef ROLLOVER_SELECTION_DIALOG_H_
#define ROLLOVER_SELECTION_DIALOG_H_

// wxWidgets headers
#include <wx/dialog.h>

class RolloverSelectionDialog : public wxDialog
{
public:
	RolloverSelectionDialog() : wxDialog() {}

	double GetRolloverPoint() const { return rolloverPoint; }

private:
	double rolloverPoint = 360.0;
};

#endif// ROLLOVER_SELECTION_DIALOG_H_
