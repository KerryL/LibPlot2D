/*=============================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2015

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  legend.h
// Date:  3/6/2015
// Auth:  K. Loux
// Desc:  Derived from Primitive, this class is used to draw plot legends.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "lp2d/renderer/primitives/legend.h"
#include "lp2d/renderer/renderWindow.h"

namespace LibPlot2D
{

//=============================================================================
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
//=============================================================================
const unsigned int Legend::entrySpacing(5);// [pixels]

//=============================================================================
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
//=============================================================================
Legend::Legend(RenderWindow &renderWindow) : Primitive(renderWindow),
	text(renderWindow), lines(renderWindow)
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

	bufferInfo.push_back(BufferInfo());// Text
}

//=============================================================================
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
//=============================================================================
void Legend::Update(const unsigned int& i)
{
	// TODO:  Alternative approach is to only update if entries change
	// and to use local modelview to handle position changes.
	if (!text.IsOK())
		return;

	if (i == 0)// Background, border, lines and markers
	{
		// Captial "H" gives good idea of text height
		textHeight = text.GetBoundingBox("H").yUp;

		UpdateBoundingBox();

		bufferVector.push_back(BuildBackground());

		lines.SetWidth(borderSize);
		lines.SetLineColor(borderColor);
		lines.SetBackgroundColorForAlphaFade();
		lines.Build(BuildBorderPoints(), Line::UpdateManual);
		bufferVector.push_back(lines.GetBufferInfo());

		BuildSampleLines();
		BuildMarkers();

		bufferInfo[i] = AssembleBuffers();
	}
	else if (i == 1)// Text
	{
		text.SetColor(fontColor);
		BuildLabelStrings();
		bufferInfo[i] = text.BuildText();
	}

	bufferInfo[i].vertexCountModified = false;
}

//=============================================================================
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
//=============================================================================
void Legend::GenerateGeometry()
{
	// Background, border, lines and markers first
	// We can use the Line::DoPrettyDraw to render the background and markers
	// even though they aren't lines because DoPrettyDraw renders triangle
	// elements.  We only need to ensure that the additional triangles are
	// intended to be built as elements of the index array.
	if (bufferInfo[0].vertexCount > 0)
	{
		glBindVertexArray(bufferInfo[0].GetVertexArrayIndex());
		Line::DoPrettyDraw(bufferInfo[0].indexBuffer.size());
	}

	// Text last
	if (text.IsOK() && bufferInfo[1].vertexCount > 0)
	{
		glBindVertexArray(bufferInfo[1].GetVertexArrayIndex());
		text.RenderBufferedGlyph(bufferInfo[1].vertexCount);
	}
}

//=============================================================================
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
//=============================================================================
bool Legend::HasValidParameters()
{
	if (entries.size() == 0)
		return false;
		
	return true;
}

//=============================================================================
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
//=============================================================================
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

//=============================================================================
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
//=============================================================================
void Legend::UpdateBoundingBox()
{
	Text::BoundingBox boundingBox;
	unsigned int maxStringWidth(0);
	for (const auto& entry : entries)
	{
		boundingBox = text.GetBoundingBox(entry.text.ToStdString());
		if (boundingBox.xRight > (int)maxStringWidth)
			maxStringWidth = boundingBox.xRight;
	}
	
	width = 3 * entrySpacing + sampleLength + maxStringWidth;
	height = (textHeight + entrySpacing) * entries.size() + entrySpacing;
}

//=============================================================================
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
//=============================================================================
void Legend::SetFont(const std::string& fontFileName, const double& size)
{
	if (!text.SetFace(fontFileName))
		return;

	text.SetColor(fontColor);

	// For some reason, fonts tend to render more clearly at a larger size.  So
	// we up-scale to render the fonts then down-scale to achieve the desired
	// on-screen size.
	// TODO:  OGL4 Better to use a fixed large size and adjust scale accordingly?
	const double factor(3.0);
	text.SetSize(size * factor);
	text.SetScale(1.0 / factor);
}

//=============================================================================
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
//=============================================================================
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

//=============================================================================
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
//=============================================================================
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

//=============================================================================
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
//=============================================================================
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
	
	RequiresRedraw();
}

//=============================================================================
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
//=============================================================================
double Legend::GetXPos(const PositionReference& legendRef, const PositionReference& windowRef) const
{
	double x, y;
	GetPosition(legendRef, windowRef, x, y);
	return x;
}

//=============================================================================
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
//=============================================================================
double Legend::GetYPos(const PositionReference& legendRef, const PositionReference& windowRef) const
{
	double x, y;
	GetPosition(legendRef, windowRef, x, y);
	return y;
}

//=============================================================================
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
//=============================================================================
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

//=============================================================================
// Class:			Legend
// Function:		BuildBorderPoints
//
// Description:		Returns a vector containing pairs of border points.
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
//=============================================================================
std::vector<std::pair<double, double> > Legend::BuildBorderPoints() const
{
	std::vector<std::pair<double, double> > corners(GetCornerVertices());
	corners.push_back(corners[0]);

	return corners;
}

//=============================================================================
// Class:			Legend
// Function:		BuildBackground
//
// Description:		Builds the buffer required to draw the background.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive::BufferInfo
//
//=============================================================================
Primitive::BufferInfo Legend::BuildBackground() const
{
	Primitive::BufferInfo buffer;

	buffer.vertexCount = 4;
	buffer.vertexBuffer.resize(buffer.vertexCount * (4 + renderWindow.GetVertexDimension()));
	assert(renderWindow.GetVertexDimension() == 2);

	buffer.indexBuffer.resize(6);

	std::vector<std::pair<double, double> > corners(GetCornerVertices());
	buffer.vertexBuffer[0] = (float)corners[0].first;
	buffer.vertexBuffer[1] = (float)corners[0].second;

	buffer.vertexBuffer[2] = (float)corners[1].first;
	buffer.vertexBuffer[3] = (float)corners[1].second;

	buffer.vertexBuffer[4] = (float)corners[2].first;
	buffer.vertexBuffer[5] = (float)corners[2].second;

	buffer.vertexBuffer[6] = (float)corners[3].first;
	buffer.vertexBuffer[7] = (float)corners[3].second;

	buffer.vertexBuffer[8] = (float)backgroundColor.GetRed();
	buffer.vertexBuffer[9] = (float)backgroundColor.GetGreen();
	buffer.vertexBuffer[10] = (float)backgroundColor.GetBlue();
	buffer.vertexBuffer[11] = (float)backgroundColor.GetAlpha();

	buffer.vertexBuffer[12] = (float)backgroundColor.GetRed();
	buffer.vertexBuffer[13] = (float)backgroundColor.GetGreen();
	buffer.vertexBuffer[14] = (float)backgroundColor.GetBlue();
	buffer.vertexBuffer[15] = (float)backgroundColor.GetAlpha();

	buffer.vertexBuffer[16] = (float)backgroundColor.GetRed();
	buffer.vertexBuffer[17] = (float)backgroundColor.GetGreen();
	buffer.vertexBuffer[18] = (float)backgroundColor.GetBlue();
	buffer.vertexBuffer[19] = (float)backgroundColor.GetAlpha();

	buffer.vertexBuffer[20] = (float)backgroundColor.GetRed();
	buffer.vertexBuffer[21] = (float)backgroundColor.GetGreen();
	buffer.vertexBuffer[22] = (float)backgroundColor.GetBlue();
	buffer.vertexBuffer[23] = (float)backgroundColor.GetAlpha();

	buffer.indexBuffer[0] = 0;
	buffer.indexBuffer[1] = 1;
	buffer.indexBuffer[2] = 2;

	buffer.indexBuffer[3] = 2;
	buffer.indexBuffer[4] = 3;
	buffer.indexBuffer[5] = 0;

	return buffer;
}

//=============================================================================
// Class:			Legend
// Function:		BuildMarkers
//
// Description:		Builds the buffers for rendering plot markers.
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
void Legend::BuildMarkers()
{
	const unsigned int lineYOffset(entrySpacing);

	Primitive::BufferInfo buffer;

	double x, y;
	GetAdjustedPosition(x, y);

	x += entrySpacing + 0.5 * sampleLength;
	y += height + lineYOffset;

	for (const auto& entry : entries)
	{
		double halfSize(entry.markerSize * 2.0);// This relationship comes from PlotCurve class
		y -= entrySpacing + textHeight;

		if (halfSize <= 0.0)
			continue;

		buffer.vertexCount = 4;
		buffer.vertexBuffer.resize(buffer.vertexCount * (renderWindow.GetVertexDimension() + 4));
		assert(renderWindow.GetVertexDimension() == 2);

		buffer.indexBuffer.resize(6);

		buffer.vertexBuffer[0] = x - halfSize;
		buffer.vertexBuffer[1] = y - halfSize;

		buffer.vertexBuffer[2] = x - halfSize;
		buffer.vertexBuffer[3] = y + halfSize;

		buffer.vertexBuffer[4] = x + halfSize;
		buffer.vertexBuffer[5] = y + halfSize;

		buffer.vertexBuffer[6] = x + halfSize;
		buffer.vertexBuffer[7] = y - halfSize;

		buffer.vertexBuffer[8] = entry.color.GetRed();
		buffer.vertexBuffer[9] = entry.color.GetGreen();
		buffer.vertexBuffer[10] = entry.color.GetBlue();
		buffer.vertexBuffer[11] = entry.color.GetAlpha();

		buffer.vertexBuffer[12] = entry.color.GetRed();
		buffer.vertexBuffer[13] = entry.color.GetGreen();
		buffer.vertexBuffer[14] = entry.color.GetBlue();
		buffer.vertexBuffer[15] = entry.color.GetAlpha();

		buffer.vertexBuffer[16] = entry.color.GetRed();
		buffer.vertexBuffer[17] = entry.color.GetGreen();
		buffer.vertexBuffer[18] = entry.color.GetBlue();
		buffer.vertexBuffer[19] = entry.color.GetAlpha();

		buffer.vertexBuffer[20] = entry.color.GetRed();
		buffer.vertexBuffer[21] = entry.color.GetGreen();
		buffer.vertexBuffer[22] = entry.color.GetBlue();
		buffer.vertexBuffer[23] = entry.color.GetAlpha();

		buffer.indexBuffer[0] = 0;
		buffer.indexBuffer[1] = 1;
		buffer.indexBuffer[2] = 2;

		buffer.indexBuffer[3] = 2;
		buffer.indexBuffer[4] = 3;
		buffer.indexBuffer[5] = 0;

		bufferVector.push_back(buffer);
	}
}

//=============================================================================
// Class:			Legend
// Function:		AssembleBuffers
//
// Description:		Combines the triangle buffers into a single BufferInfo
//					object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive::BufferInfo
//
//=============================================================================
Primitive::BufferInfo Legend::AssembleBuffers()
{
	Primitive::BufferInfo buffer;

	for (const auto& b : bufferVector)
	{
		buffer.indexBuffer.insert(buffer.indexBuffer.end(),
			b.indexBuffer.begin(), b.indexBuffer.end());
		buffer.vertexBuffer.insert(buffer.vertexBuffer.end(),
			b.vertexBuffer.begin(), b.vertexBuffer.end());
		buffer.vertexCount += b.vertexCount;
	}

	const unsigned int colorStart(
		buffer.vertexCount * renderWindow.GetVertexDimension());

	unsigned int j, k(0), m(0), indexShift(0);
	for (auto& b : bufferVector)
	{
		unsigned int bufferColorStart(
			b.vertexCount * renderWindow.GetVertexDimension());

		for (j = 0; j < b.vertexCount; ++j)
		{
			buffer.vertexBuffer[k * 2] = b.vertexBuffer[j * 2];
			buffer.vertexBuffer[k * 2 + 1] = b.vertexBuffer[j * 2 + 1];

			buffer.vertexBuffer[colorStart + k * 4] = b.vertexBuffer[bufferColorStart + j * 4];
			buffer.vertexBuffer[colorStart + k * 4 + 1] = b.vertexBuffer[bufferColorStart + j * 4 + 1];
			buffer.vertexBuffer[colorStart + k * 4 + 2] = b.vertexBuffer[bufferColorStart + j * 4 + 2];
			buffer.vertexBuffer[colorStart + k * 4 + 3] = b.vertexBuffer[bufferColorStart + j * 4 + 3];
			++k;
		}

		for (const auto& ib : b.indexBuffer)
			buffer.indexBuffer[m++] = ib + indexShift;

		indexShift += b.vertexCount;

		b.FreeOpenGLObjects();
	}

	ConfigureVertexArray(buffer);
	bufferVector.clear();

	return buffer;
}

//=============================================================================
// Class:			Legend
// Function:		BuildSampleLines
//
// Description:		Appends lines corresponding to the legend entries to the
//					buffer vector.
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
void Legend::BuildSampleLines()
{
	const unsigned int lineYOffset(entrySpacing);

	double x, y;
	GetAdjustedPosition(x, y);

	y += height + lineYOffset;

	for (const auto& entry : entries)
	{
		lines.SetLineColor(entry.color);
		lines.SetBackgroundColorForAlphaFade();
		lines.SetWidth(entry.lineSize);

		y -= entrySpacing + textHeight;

		lines.Build(x + entrySpacing, y,
			x + entrySpacing + sampleLength, y, Line::UpdateManual);
		bufferVector.push_back(lines.GetBufferInfo());
	}
}

//=============================================================================
// Class:			Legend
// Function:		ConfigureVertexArray
//
// Description:		Handles configuration of OpenGL vertex array object.
//
// Input Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive::BufferInfo
//
//=============================================================================
void Legend::ConfigureVertexArray(Primitive::BufferInfo& bufferInfo) const
{
	bufferInfo.GetOpenGLIndices(true);
	glBindVertexArray(bufferInfo.GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * bufferInfo.vertexCount * (renderWindow.GetVertexDimension() + 4),
		bufferInfo.vertexBuffer.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(renderWindow.GetPositionLocation());
	glVertexAttribPointer(renderWindow.GetPositionLocation(),
		renderWindow.GetVertexDimension(), GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(renderWindow.GetColorLocation());
	glVertexAttribPointer(renderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * bufferInfo.vertexCount * renderWindow.GetVertexDimension()));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferInfo.GetIndexBufferIndex());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * bufferInfo.indexBuffer.size(),
		bufferInfo.indexBuffer.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

//=============================================================================
// Class:			Legend
// Function:		BuildLabelStrings
//
// Description:		Generates text for each entry.
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
void Legend::BuildLabelStrings()
{
	double x, y;
	GetAdjustedPosition(x, y);
	y += height;
	
	for (const auto& entry : entries)
	{
		y -= entrySpacing + textHeight;
		text.SetPosition(x + 2 * entrySpacing + sampleLength, y);
		text.AppendText(entry.text.ToStdString());
	}
}

//=============================================================================
// Class:			Legend
// Function:		RequiresRedraw
//
// Description:		Sets flags indicating we want to re-perform OpenGL stuff.
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
void Legend::RequiresRedraw()
{
	modified = true;
	for (auto& buffer : bufferInfo)
		buffer.vertexCountModified = true;
}

}// namespace LibPlot2D
