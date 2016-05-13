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

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "renderer/primitives/legend.h"
#include "renderer/renderWindow.h"
#include "renderer/line.h"

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
Legend::Legend(RenderWindow &renderWindow) : Primitive(renderWindow), text(renderWindow)
{
	fontColor = Color::ColorBlack;
	backgroundColor = Color::ColorWhite;
	borderColor = Color::ColorBlack;

	x = 0;
	y = 0;
	borderSize = 1;
	sampleLength = 15;

	windowRef = BottomLeft;
	legendRef = Center;

	SetDrawOrder(3000);// Draw this last
}

//==========================================================================
// Class:			Legend
// Function:		Update
//
// Description:		Updates the GL buffers associated with this object.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Legend::Update(const unsigned int& i)
{
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
void Legend::GenerateGeometry()
{
	/*UpdateBoundingBox();

	glPushMatrix();
		glLoadIdentity();
		renderWindow.ShiftForExactPixelization();

		DrawBackground();
		DrawBorder();

		unsigned int i;
		for (i = 0; i < entries.size(); i++)
			DrawNextEntry(i);

	glPopMatrix();*/
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
bool Legend::HasValidParameters()
{
	if (entries.size() == 0)
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
	// TODO:  Update for OGL4
	/*double x, y;
	GetAdjustedPosition(x, y);
	y += height;
	
	FTBBox boundingBox = font->BBox("H");// Some capital letter to assure uniform spacing
	y -= (entrySpacing + boundingBox.Upper().Y()) * (index + 1);

	// Draw sample line
	const unsigned int lineYOffset(entrySpacing);
	Line line;
	line.SetWidth(entries[index].lineSize);
	line.SetLineColor(entries[index].color);
	line.SetBackgroundColorForAlphaFade();
	line.Draw(x + entrySpacing, y + lineYOffset, x + entrySpacing + sampleLength, y + lineYOffset);
	
	if (entries[index].markerSize > 0)
		DrawMarker(x + entrySpacing + sampleLength * 0.5, y + lineYOffset, entries[index].markerSize);

	// Draw label text
	glPushMatrix();
		glColor4d(0.0, 0.0, 0.0, 1.0);
		glLoadIdentity();
		renderWindow.ShiftForExactPixelization();
		glTranslated(x + 2 * entrySpacing + sampleLength, y, 0.0);
		font->Render(entries[index].text.mb_str());
	glPopMatrix();*/
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
	// TODO:  Update for OGL4
	/*const unsigned int halfSize(size * 2);
	glBegin(GL_QUADS);
	glVertex2i(x - halfSize, y - halfSize);
	glVertex2i(x + halfSize, y - halfSize);
	glVertex2i(x + halfSize, y + halfSize);
	glVertex2i(x - halfSize, y + halfSize);
	glEnd();*/
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
	// TODO:  Update for OGL4
	/*glBegin(GL_QUADS);
	glColor4d(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
	
	double x, y;
	GetAdjustedPosition(x, y);

	glVertex2i(x, y);
	glVertex2i(x + width, y);
	glVertex2i(x + width, y + height);
	glVertex2i(x, y + height);

	glEnd();*/
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
	// TODO:  Update for OGL4
	/*Line border;
	border.SetWidth(borderSize);
	border.SetLineColor(borderColor);
	border.SetBackgroundColorForAlphaFade();
	border.Draw(GetCornerVertices());*/
}

//==========================================================================
// Class:			Legend
// Function:		GetCornerVertices
//
// Description:		Returns the four corner points in a vector.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		std::vector<std::pair<double, double> >
//
//==========================================================================
std::vector<std::pair<double, double> > Legend::GetCornerVertices() const
{
	double x, y;
	GetAdjustedPosition(x, y);

	std::vector<std::pair<double, double> > points;
	points.push_back(std::make_pair(x, y));
	points.push_back(std::make_pair(x + width, y));
	points.push_back(std::make_pair(x + width, y + height));
	points.push_back(std::make_pair(x, y + height));
	points.push_back(std::make_pair(x, y));

	return points;
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
/*	FTBBox boundingBox;
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
	height = (boundingBox.Upper().Y() + entrySpacing) * i + entrySpacing;*/
	// TODO:  Fix
}

//==========================================================================
// Class:			Legend
// Function:		SetFont
//
// Description:		Sets up the font object.
//
// Input Arguments:
//		fontFileName	= const std::string&
//		size			= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Legend::SetFont(const std::string& fontFileName, const double& size)
{
	// TODO:  Implement
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
// Description:		Adjusts the position based on the position references.
//					The output is (x, y) of the lower LH corner of the legend
//					w.r.t. the lower LH corner of the render window.
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
	switch (windowRef)
	{
	default:
	case BottomLeft:
	case MiddleLeft:
	case TopLeft:
		x = this->x;
		break;

	case BottomCenter:
	case Center:
	case TopCenter:
		x = renderWindow.GetSize().GetWidth() * 0.5 + this->x;
		break;

	case BottomRight:
	case MiddleRight:
	case TopRight:
		x = renderWindow.GetSize().GetWidth() - this->x;
		break;
	}

	switch (windowRef)
	{
	default:
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
		y = this->y;
		break;

	case MiddleLeft:
	case Center:
	case MiddleRight:
		y = renderWindow.GetSize().GetHeight() * 0.5 + this->y;
		break;

	case TopLeft:
	case TopCenter:
	case TopRight:
		y = renderWindow.GetSize().GetHeight() - this->y;
		break;
	}

	// At this point, x and y represent the legendRef corner of the
	// legend w.r.t. the lower LH window corner of the render window

	switch (legendRef)
	{
	case BottomLeft:
	case MiddleLeft:
	case TopLeft:
		break;

	default:
	case BottomCenter:
	case Center:
	case TopCenter:
		x -= width * 0.5;
		break;

	case BottomRight:
	case MiddleRight:
	case TopRight:
		x -= width;
		break;
	}

	switch (legendRef)
	{
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
		break;

	default:
	case MiddleLeft:
	case Center:
	case MiddleRight:
		y -= height * 0.5;
		break;

	case TopLeft:
	case TopCenter:
	case TopRight:
		y -= height;
		break;
	}
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
	switch (windowRef)
	{
	default:
	case BottomLeft:
	case MiddleLeft:
	case TopLeft:
	case BottomCenter:
	case Center:
	case TopCenter:
		this->x += x;
		break;

	case BottomRight:
	case MiddleRight:
	case TopRight:
		this->x -= x;
		break;
	}

	switch (windowRef)
	{
	default:
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
	case MiddleLeft:
	case Center:
	case MiddleRight:
		this->y += y;
		break;

	case TopLeft:
	case TopCenter:
	case TopRight:
		this->y -= y;
		break;
	}
	
	modified = true;
}

//==========================================================================
// Class:			Legend
// Function:		GetXPos
//
// Description:		Gets the x-position w.r.t. the specified references.
//
// Input Arguments:
//		legendRef	= const PositionReference&
//		windowRef	= const PositionReference&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double Legend::GetXPos(const PositionReference& legendRef, const PositionReference& windowRef) const
{
	double x, y;
	GetPosition(legendRef, windowRef, x, y);
	return x;
}

//==========================================================================
// Class:			Legend
// Function:		GetXPos
//
// Description:		Gets the y-position w.r.t. the specified references.
//
// Input Arguments:
//		ref	= const PositionReference&
//		windowRef	= const PositionReference&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double Legend::GetYPos(const PositionReference& legendRef, const PositionReference& windowRef) const
{
	double x, y;
	GetPosition(legendRef, windowRef, x, y);
	return y;
}

//==========================================================================
// Class:			Legend
// Function:		GetPosition
//
// Description:		Gets the position w.r.t. the specified references.  Read
//					this as position of specified legendRef w.r.t. specified
//					windowRef.
//
// Input Arguments:
//		legendRef	= const PositionReference&
//		windowRef	= const PositionReference&
//
// Output Arguments:
//		x			= double&
//		y			= double&
//
// Return Value:
//		None
//
//==========================================================================
void Legend::GetPosition(const PositionReference& legendRef,
	const PositionReference& windowRef, double &x, double &y) const
{
	// Internally, x and y are location of legendRef w.r.t. windowRef, so first
	// we need to back out to a common reference, then apply the specified references

	switch (this->windowRef)
	{
	default:
	case BottomLeft:
	case MiddleLeft:
	case TopLeft:
		x = this->x;
		break;

	case BottomCenter:
	case Center:
	case TopCenter:
		x = renderWindow.GetSize().GetWidth() * 0.5 + this->x;
		break;

	case BottomRight:
	case MiddleRight:
	case TopRight:
		x = renderWindow.GetSize().GetWidth() - this->x;
		break;
	}

	switch (this->windowRef)
	{
	default:
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
		y = this->y;
		break;

	case MiddleLeft:
	case Center:
	case MiddleRight:
		y = renderWindow.GetSize().GetHeight() * 0.5 + this->y;
		break;

	case TopLeft:
	case TopCenter:
	case TopRight:
		y = renderWindow.GetSize().GetHeight() - this->y;
		break;
	}

	// At this point, x and y represent the legendRef corner (class value, not argument) of the
	// legend w.r.t. the lower LH window corner of the render window

	switch (this->legendRef)
	{
	case BottomLeft:
	case MiddleLeft:
	case TopLeft:
		break;

	default:
	case BottomCenter:
	case Center:
	case TopCenter:
		x -= width * 0.5;
		break;

	case BottomRight:
	case MiddleRight:
	case TopRight:
		x -= width;
		break;
	}

	switch (this->legendRef)
	{
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
		break;

	default:
	case MiddleLeft:
	case Center:
	case MiddleRight:
		y -= height * 0.5;
		break;

	case TopLeft:
	case TopCenter:
	case TopRight:
		y -= height;
		break;
	}

	// At this point, x and y represent the lower left-hand corner of the
	// legend w.r.t. the lower LH window corner of the render window

	switch (windowRef)
	{
	default:
	case BottomLeft:
	case MiddleLeft:
	case TopLeft:
		break;

	case BottomCenter:
	case Center:
	case TopCenter:
		x -= renderWindow.GetSize().GetWidth() * 0.5;
		break;

	case BottomRight:
	case MiddleRight:
	case TopRight:
		x = renderWindow.GetSize().GetWidth() - x;
		break;
	}

	switch (windowRef)
	{
	default:
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
		break;

	case MiddleLeft:
	case Center:
	case MiddleRight:
		y -= renderWindow.GetSize().GetHeight() * 0.5;
		break;

	case TopLeft:
	case TopCenter:
	case TopRight:
		y = renderWindow.GetSize().GetHeight() - y;
		break;
	}

	// At this point, x and y represent the lower left-hand corner of the
	// legend w.r.t. the specified windowRef

	switch (legendRef)
	{
	case BottomLeft:
	case MiddleLeft:
	case TopLeft:
		break;

	default:
	case BottomCenter:
	case Center:
	case TopCenter:
		x += width * 0.5;
		break;

	case BottomRight:
	case MiddleRight:
	case TopRight:
		if (windowRef == BottomRight ||
			windowRef == MiddleRight ||
			windowRef == TopRight)
			x -= width;
		else
			x += width;
		break;
	}

	switch (legendRef)
	{
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
		break;

	default:
	case MiddleLeft:
	case Center:
	case MiddleRight:
		y += height * 0.5;
		break;

	case TopLeft:
	case TopCenter:
	case TopRight:
		if (windowRef == TopLeft ||
			windowRef == TopCenter ||
			windowRef == TopRight)
			y -= height;
		else
			y += height;
		break;
	}
}
