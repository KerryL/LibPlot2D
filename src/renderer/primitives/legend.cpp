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

// Local headers
#include "renderer/primitives/legend.h"
#include "renderer/renderWindow.h"
#include "utilities/math/plotMath.h"

// FTGL headers
#include <FTGL/ftgl.h>

//==========================================================================
// Class:			Legend
// Function:		Constant declarations
//
// Description:		Constant declarations for the Legend class.
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
const unsigned int Legend::entrySpacing(5);// [pixels]

//==========================================================================
// Class:			Legend
// Function:		Legend
//
// Description:		Constructor for the Legend class.
//
// Input Arguments:
//		renderWindow	= RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Legend::Legend(RenderWindow &renderWindow) : Primitive(renderWindow)
{
	fontColor = Color::ColorBlack;
	backgroundColor = Color::ColorWhite;
	borderColor = Color::ColorBlack;

	x = 0;
	y = 0;
	borderSize = 1;
	sampleLength = 15;

	anchor = Center;
	positionRef = RefBottomLeft;

	font = NULL;

	SetDrawOrder(1000);// Draw this last
}

//==========================================================================
// Class:			Legend
// Function:		GenerateGeometry
//
// Description:		Creates the OpenGL instructions to create this object in
//					the scene.
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
void Legend::GenerateGeometry(void)
{
	UpdateBoundingBox();

	glPushMatrix();
		glLoadIdentity();
		renderWindow.ShiftForExactPixelization();

		DrawBackground();
		DrawBorder();

		unsigned int i;
		for (i = 0; i < entries.size(); i++)
			DrawNextEntry(i);

	glPopMatrix();
}

//==========================================================================
// Class:			Legend
// Function:		HasValidParameters
//
// Description:		Checks to see if the information about this object is
//					valid and complete (gives permission to create the object).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
bool Legend::HasValidParameters(void)
{
	if (!font)
		return false;
	else if (entries.size() == 0)
		return false;
		
	return true;
}

//==========================================================================
// Class:			Legend
// Function:		DrawNextEntry
//
// Description:		Draws the next legend entry.
//
// Input Arguments:
//		info	= const LegendEntryInfo&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Legend::DrawNextEntry(const double &index) const
{
	double x, y;
	GetAdjustedPosition(x, y);
	y += height;
	
	FTBBox boundingBox = font->BBox("H");// Some capital letter to assure uniform spacing
	y -= (entrySpacing + boundingBox.Upper().Y()) * (index + 1);

	// Draw sample line
	const unsigned int lineYOffset(entrySpacing);
	glLineWidth(entries[index].lineSize);
	glBegin(GL_LINES);
	glColor4f(entries[index].color.GetRed(), entries[index].color.GetGreen(),
		entries[index].color.GetBlue(), entries[index].color.GetAlpha());
	glVertex2i(x + entrySpacing, y + lineYOffset);
	glVertex2i(x + entrySpacing + sampleLength, y + lineYOffset);
	glEnd();
	
	if (entries[index].markerSize > 0)
		DrawMarker(x + entrySpacing + sampleLength * 0.5, y + lineYOffset, entries[index].markerSize);

	// Draw label text
	glPushMatrix();
		glColor4d(0.0, 0.0, 0.0, 1.0);
		glLoadIdentity();
		renderWindow.ShiftForExactPixelization();
		glTranslated(x + 2 * entrySpacing + sampleLength, y, 0.0);
		font->Render(entries[index].text.mb_str());
	glPopMatrix();
}

//==========================================================================
// Class:			Legend
// Function:		DrawMarker
//
// Description:		Draws the marker at the specified location.
//
// Input Arguments:
//		x		= const unsigned int &
//		y		= const unsigned int &
//		size	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Legend::DrawMarker(const unsigned int &x, const unsigned int &y,
	const unsigned int &size) const
{
	const unsigned int halfSize(size * 2);
	glBegin(GL_QUADS);
	glVertex2i(x - halfSize, y - halfSize);
	glVertex2i(x + halfSize, y - halfSize);
	glVertex2i(x + halfSize, y + halfSize);
	glVertex2i(x - halfSize, y + halfSize);
	glEnd();
}

//==========================================================================
// Class:			Legend
// Function:		DrawBackground
//
// Description:		Draws the background area.
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
void Legend::DrawBackground() const
{
	glBegin(GL_QUADS);
	glColor4d(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
	DrawCornerVertices();
	glEnd();
}

//==========================================================================
// Class:			Legend
// Function:		DrawBorder
//
// Description:		Draws the border around the legend.
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
void Legend::DrawBorder() const
{
	glLineWidth(borderSize);
	glBegin(GL_LINE_LOOP);
	glColor4d(borderColor.GetRed(), borderColor.GetGreen(), borderColor.GetBlue(), borderColor.GetAlpha());
	DrawCornerVertices();
	glEnd();
}

//==========================================================================
// Class:			Legend
// Function:		DrawCornerVertices
//
// Description:		Draws the four corner points
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
void Legend::DrawCornerVertices() const
{
	double x, y;
	GetAdjustedPosition(x, y);

	glVertex2i(x, y);
	glVertex2i(x + width, y);
	glVertex2i(x + width, y + height);
	glVertex2i(x, y + height);
}

//==========================================================================
// Class:			Legend
// Function:		UpdateBoundingBox
//
// Description:		Updates the bounding box height and width variables.
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
void Legend::UpdateBoundingBox()
{
	FTBBox boundingBox;
	unsigned int maxStringWidth(0);
	unsigned int i;
	for (i = 0; i < entries.size(); i++)
	{
		boundingBox = font->BBox(entries[i].text.mb_str());
		if (boundingBox.Upper().X() > maxStringWidth)
			maxStringWidth = boundingBox.Upper().X();
	}
	
	width = 3 * entrySpacing + sampleLength + maxStringWidth;
	
	boundingBox = font->BBox("H");
	height = (boundingBox.Upper().Y() + entrySpacing) * i + entrySpacing;
}

//==========================================================================
// Class:			Legend
// Function:		IsUnder
//
// Description:		Determines if the specified point is under this object.
//
// Input Arguments:
//		x	= const unsigned int&
//		y	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if this object is under the specified point
//
//==========================================================================
bool Legend::IsUnder(const unsigned int &x, const unsigned int &y) const
{
	if (!isVisible)
		return false;

	double adjX, adjY;
	GetAdjustedPosition(adjX, adjY);

	if (adjX <= x && adjX + width >= x &&
		adjY <= y && adjY + height >= y)
		return true;

	return false;
}

//==========================================================================
// Class:			Legend
// Function:		GetAdjustedPosition
//
// Description:		Adjusts the position based on the anchor and position reference
//					(internally, we always use bottom left corner for both).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		x	= double&
//		y	= double&
//
// Return Value:
//		None
//
//==========================================================================
void Legend::GetAdjustedPosition(double &x, double &y) const
{
	switch (positionRef)
	{
	case RefBottomRight:
		x = renderWindow.GetSize().GetWidth() - this->x;
		y = this->y;
		break;

	case RefTopLeft:
		x = this->x;
		y = renderWindow.GetSize().GetHeight() - this->y;
		break;

	case RefTopRight:
		x = renderWindow.GetSize().GetWidth() - this->x;
		y = renderWindow.GetSize().GetHeight() - this->y;
		break;

	default:
	case RefBottomLeft:
		x = this->x;
		y = this->y;
	}

	// At this point, x and y represent the lower left-hand corner of the legend w.r.t. the lower LH window corner
	// this->x and this->y are given w.r.t. the specified position ref
	// output arguments from this function are always w.r.t. lower left corner of render window

	switch (anchor)
	{
	default:
	case Center:
		x -= width * 0.5;
		y -= height * 0.5;
		break;

	case BottomLeft:
		// Matches internal representation, no adjustment required
		break;

	case BottomCenter:
		x -= width * 0.5;
		break;

	case BottomRight:
		x -= width;
		break;

	case MiddleRight:
		x -= width;
		y -= height * 0.5;
		break;

	case TopRight:
		x -= width;
		y -= height;
		break;

	case TopCenter:
		x -= width * 0.5;
		y -= height;
		break;

	case TopLeft:
		y -= height;
		break;

	case MiddleLeft:
		y -= height * 0.5;
		break;
	};
}

//==========================================================================
// Class:			Legend
// Function:		SetDeltaPosition
//
// Description:		Updates the position according to specified reference.
//
// Input Arguments:
//		x	= double&
//		y	= double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Legend::SetDeltaPosition(const double &x, const double &y)
{
	switch (positionRef)
	{
	case RefBottomRight:
		this->x -= x;
		this->y += y;
		break;

	case RefTopLeft:
		this->x += x;
		this->y -= y;
		break;

	case RefTopRight:
		this->x -= x;
		this->y -= y;
		break;

	default:
	case RefBottomLeft:
		this->x += x;
		this->y += y;
	}
	modified = true;
}