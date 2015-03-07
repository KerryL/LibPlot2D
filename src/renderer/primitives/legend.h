/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2015

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  legend.h
// Created:  3/6/2015
// Author:  K. Loux
// Description:  Derived from Primitive, this class is used to draw plot legends.
// History:

#ifndef LEGEND_H_
#define LEGEND_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "renderer/primitives/primitive.h"

// FTGL forward declarations
class FTFont;

class Legend : public Primitive
{
public:
	// Constructor
	Legend(RenderWindow &renderWindow);

	// Destructor
	virtual ~Legend() {}

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Set option methods
	void SetFont(FTFont *font) { this->font = font; modified = true; }
	void SetFontColor(const Color &color) { fontColor = color; modified = true; }
	void SetBackgroundColor(const Color &color) { backgroundColor = color; modified = true; }
	void SetBorderColor(const Color &color) { borderColor = color; modified = true; }
	void SetBorderSize(const unsigned int &size) { borderSize = size; modified = true; }
	void SetSampleLineLength(const unsigned int &size) { sampleLength = size; modified = true; }

	enum Anchor
	{
		Center,
		BottomLeft,
		BottomCenter,
		BottomRight,
		MiddleRight,
		TopRight,
		TopCenter,
		TopLeft,
		MiddleLeft
	};

	enum PositionReference
	{
		RefBottomLeft,
		RefBottomRight,
		RefTopLeft,
		RefTopRight,
	};

	void SetAnchor(const Anchor &anchor) { this->anchor = anchor; modified = true; }
	void SetPositionReference(const PositionReference &positionRef) { this->positionRef = positionRef; modified = true; }

	// Specifies bottom left corner of legend
	void SetPosition(const double &x, const double &y) { this->x = x; this->y = y; modified = true; }
	void SetDeltaPosition(const double &x, const double &y);

	unsigned int GetHeight() const { return height; }
	unsigned int GetWidth() const { return width; }

	struct LegendEntryInfo
	{
		wxString text;
		Color color;
		unsigned int lineSize;
		int markerSize;
	};

	void SetContents(const std::vector<LegendEntryInfo> &entries) { this->entries = entries; modified = true; };

	bool IsUnder(const unsigned int &x, const unsigned int &y) const;

private:
	static const unsigned int entrySpacing;// [pixels]
	FTFont *font;

	Color fontColor;
	Color backgroundColor;
	Color borderColor;

	std::vector<LegendEntryInfo> entries;

	double x, y;// [pixels]
	unsigned int borderSize;// [pixels]
	unsigned int sampleLength;// [pixels]

	void DrawNextEntry(const double &index) const;
	void DrawBackground() const;
	void DrawBorder() const;
	void DrawCornerVertices() const;
	void DrawMarker(const unsigned int &x, const unsigned int &y, const unsigned int &size) const;

	void UpdateBoundingBox();
	unsigned int height;
	unsigned int width;

	Anchor anchor;
	PositionReference positionRef;
	void GetAdjustedPosition(double &x, double &y) const;
};

#endif// _TEXT_H_