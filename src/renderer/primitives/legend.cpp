/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

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
const unsigned int Legend::mEntrySpacing(5);// [pixels]

//=============================================================================
// Class:			Legend
// Function:		Legend
//
// Description:		Constructor for the Legend class.
//
// Input Arguments:
//		mRenderWindow	= RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
Legend::Legend(RenderWindow &mRenderWindow) : Primitive(mRenderWindow),
	mText(mRenderWindow), mLines(mRenderWindow)
{
	SetDrawOrder(3000);// Draw this last
	mBufferInfo.push_back(BufferInfo());// Text
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
	// TODO:  Alternative approach is to only update if mEntries change
	// and to use local modelview to handle position changes.
	if (!mText.IsOK())
		return;

	if (i == 0)// Background, border, mLines and markers
	{
		// Captial "H" gives good idea of text mHeight
		mTextHeight = mText.GetBoundingBox("H").yUp;

		UpdateBoundingBox();

		mBufferVector.push_back(BuildBackground());

		mLines.SetWidth(mBorderSize);
		mLines.SetLineColor(mBorderColor);
		mLines.SetBackgroundColorForAlphaFade();
		mBufferVector.push_back(BufferInfo());
		mLines.Build(BuildBorderPoints(), mBufferVector.back(),
			Line::UpdateMethod::Manual);

		BuildSampleLines();
		BuildMarkers();

		mBufferInfo[i] = AssembleBuffers();
	}
	else if (i == 1)// Text
	{
		mText.SetColor(mFontColor);
		BuildLabelStrings();
		mBufferInfo[i] = mText.BuildText();
	}

	mBufferInfo[i].vertexCountModified = false;
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
	// Background, border, mLines and markers first
	// We can use the Line::DoPrettyDraw to render the background and markers
	// even though they aren't mLines because DoPrettyDraw renders triangle
	// elements.  We only need to ensure that the additional triangles are
	// intended to be built as elements of the index array.
	if (mBufferInfo[0].vertexCount > 0)
	{
		glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
		Line::DoPrettyDraw(mBufferInfo[0].indexBuffer.size());
	}

	// Text last
	if (mText.IsOK() && mBufferInfo[1].vertexCount > 0)
	{
		glBindVertexArray(mBufferInfo[1].GetVertexArrayIndex());
		mText.RenderBufferedGlyph(mBufferInfo[1].vertexCount);
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
	if (mEntries.size() == 0)
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
//		std::vector<std::pair<double, double>>
//
//=============================================================================
std::vector<std::pair<double, double>> Legend::GetCornerVertices() const
{
	double x, y;
	GetAdjustedPosition(x, y);

	std::vector<std::pair<double, double>> points;
	points.push_back(std::make_pair(x, y));
	points.push_back(std::make_pair(x + mWidth, y));
	points.push_back(std::make_pair(x + mWidth, y + mHeight));
	points.push_back(std::make_pair(x, y + mHeight));
	points.push_back(std::make_pair(x, y));

	return points;
}

//=============================================================================
// Class:			Legend
// Function:		UpdateBoundingBox
//
// Description:		Updates the bounding box mHeight and mWidth variables.
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
	for (const auto& entry : mEntries)
	{
		boundingBox = mText.GetBoundingBox(entry.text.ToStdString());
		if (boundingBox.xRight > (int)maxStringWidth)
			maxStringWidth = boundingBox.xRight;
	}
	
	mWidth = 3 * mEntrySpacing + mSampleLength + maxStringWidth;
	mHeight = (mTextHeight + mEntrySpacing) * mEntries.size() + mEntrySpacing;
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
	if (!mText.SetFace(fontFileName))
		return;

	mText.SetColor(mFontColor);

	// For some reason, fonts tend to render more clearly at a larger size.  So
	// we up-scale to render the fonts then down-scale to achieve the desired
	// on-screen size.
	// TODO:  OGL4 Better to use a fixed large size and adjust scale accordingly?
	const double factor(3.0);
	mText.SetSize(size * factor);
	mText.SetScale(1.0 / factor);
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
	if (!mIsVisible)
		return false;

	double adjX, adjY;
	GetAdjustedPosition(adjX, adjY);

	if (adjX <= x && adjX + mWidth >= x &&
		adjY <= y && adjY + mHeight >= y)
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
	switch (mWindowRef)
	{
	default:
	case PositionReference::BottomLeft:
	case PositionReference::MiddleLeft:
	case PositionReference::TopLeft:
		x = mX;
		break;

	case PositionReference::BottomCenter:
	case PositionReference::Center:
	case PositionReference::TopCenter:
		x = mRenderWindow.GetSize().GetWidth() * 0.5 + mX;
		break;

	case PositionReference::BottomRight:
	case PositionReference::MiddleRight:
	case PositionReference::TopRight:
		x = mRenderWindow.GetSize().GetWidth() - mX;
		break;
	}

	switch (mWindowRef)
	{
	default:
	case PositionReference::BottomLeft:
	case PositionReference::BottomCenter:
	case PositionReference::BottomRight:
		y = mY;
		break;

	case PositionReference::MiddleLeft:
	case PositionReference::Center:
	case PositionReference::MiddleRight:
		y = mRenderWindow.GetSize().GetHeight() * 0.5 + mY;
		break;

	case PositionReference::TopLeft:
	case PositionReference::TopCenter:
	case PositionReference::TopRight:
		y = mRenderWindow.GetSize().GetHeight() - mY;
		break;
	}

	// At this point, x and y represent the legendRef corner of the
	// legend w.r.t. the lower LH window corner of the render window

	switch (mLegendRef)
	{
	case PositionReference::BottomLeft:
	case PositionReference::MiddleLeft:
	case PositionReference::TopLeft:
		break;

	default:
	case PositionReference::BottomCenter:
	case PositionReference::Center:
	case PositionReference::TopCenter:
		x -= mWidth * 0.5;
		break;

	case PositionReference::BottomRight:
	case PositionReference::MiddleRight:
	case PositionReference::TopRight:
		x -= mWidth;
		break;
	}

	switch (mLegendRef)
	{
	case PositionReference::BottomLeft:
	case PositionReference::BottomCenter:
	case PositionReference::BottomRight:
		break;

	default:
	case PositionReference::MiddleLeft:
	case PositionReference::Center:
	case PositionReference::MiddleRight:
		y -= mHeight * 0.5;
		break;

	case PositionReference::TopLeft:
	case PositionReference::TopCenter:
	case PositionReference::TopRight:
		y -= mHeight;
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
	switch (mWindowRef)
	{
	default:
	case PositionReference::BottomLeft:
	case PositionReference::MiddleLeft:
	case PositionReference::TopLeft:
	case PositionReference::BottomCenter:
	case PositionReference::Center:
	case PositionReference::TopCenter:
		mX += x;
		break;

	case PositionReference::BottomRight:
	case PositionReference::MiddleRight:
	case PositionReference::TopRight:
		mX -= x;
		break;
	}

	switch (mWindowRef)
	{
	default:
	case PositionReference::BottomLeft:
	case PositionReference::BottomCenter:
	case PositionReference::BottomRight:
	case PositionReference::MiddleLeft:
	case PositionReference::Center:
	case PositionReference::MiddleRight:
		mY += y;
		break;

	case PositionReference::TopLeft:
	case PositionReference::TopCenter:
	case PositionReference::TopRight:
		mY -= y;
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
//					mWindowRef.
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
	// Internally, x and y are location of legendRef w.r.t. mWindowRef, so first
	// we need to back out to a common reference, then apply the specified references

	switch (mWindowRef)
	{
	default:
	case PositionReference::BottomLeft:
	case PositionReference::MiddleLeft:
	case PositionReference::TopLeft:
		x = mX;
		break;

	case PositionReference::BottomCenter:
	case PositionReference::Center:
	case PositionReference::TopCenter:
		x = mRenderWindow.GetSize().GetWidth() * 0.5 + mX;
		break;

	case PositionReference::BottomRight:
	case PositionReference::MiddleRight:
	case PositionReference::TopRight:
		x = mRenderWindow.GetSize().GetWidth() - mX;
		break;
	}

	switch (mWindowRef)
	{
	default:
	case PositionReference::BottomLeft:
	case PositionReference::BottomCenter:
	case PositionReference::BottomRight:
		y = mY;
		break;

	case PositionReference::MiddleLeft:
	case PositionReference::Center:
	case PositionReference::MiddleRight:
		y = mRenderWindow.GetSize().GetHeight() * 0.5 + mY;
		break;

	case PositionReference::TopLeft:
	case PositionReference::TopCenter:
	case PositionReference::TopRight:
		y = mRenderWindow.GetSize().GetHeight() - mY;
		break;
	}

	// At this point, x and y represent the legendRef corner (class value, not argument) of the
	// legend w.r.t. the lower LH window corner of the render window

	switch (mLegendRef)
	{
	case PositionReference::BottomLeft:
	case PositionReference::MiddleLeft:
	case PositionReference::TopLeft:
		break;

	default:
	case PositionReference::BottomCenter:
	case PositionReference::Center:
	case PositionReference::TopCenter:
		x -= mWidth * 0.5;
		break;

	case PositionReference::BottomRight:
	case PositionReference::MiddleRight:
	case PositionReference::TopRight:
		x -= mWidth;
		break;
	}

	switch (mLegendRef)
	{
	case PositionReference::BottomLeft:
	case PositionReference::BottomCenter:
	case PositionReference::BottomRight:
		break;

	default:
	case PositionReference::MiddleLeft:
	case PositionReference::Center:
	case PositionReference::MiddleRight:
		y -= mHeight * 0.5;
		break;

	case PositionReference::TopLeft:
	case PositionReference::TopCenter:
	case PositionReference::TopRight:
		y -= mHeight;
		break;
	}

	// At this point, x and y represent the lower left-hand corner of the
	// legend w.r.t. the lower LH window corner of the render window

	switch (windowRef)
	{
	default:
	case PositionReference::BottomLeft:
	case PositionReference::MiddleLeft:
	case PositionReference::TopLeft:
		break;

	case PositionReference::BottomCenter:
	case PositionReference::Center:
	case PositionReference::TopCenter:
		x -= mRenderWindow.GetSize().GetWidth() * 0.5;
		break;

	case PositionReference::BottomRight:
	case PositionReference::MiddleRight:
	case PositionReference::TopRight:
		x = mRenderWindow.GetSize().GetWidth() - x;
		break;
	}

	switch (windowRef)
	{
	default:
	case PositionReference::BottomLeft:
	case PositionReference::BottomCenter:
	case PositionReference::BottomRight:
		break;

	case PositionReference::MiddleLeft:
	case PositionReference::Center:
	case PositionReference::MiddleRight:
		y -= mRenderWindow.GetSize().GetHeight() * 0.5;
		break;

	case PositionReference::TopLeft:
	case PositionReference::TopCenter:
	case PositionReference::TopRight:
		y = mRenderWindow.GetSize().GetHeight() - y;
		break;
	}

	// At this point, x and y represent the lower left-hand corner of the
	// legend w.r.t. the specified mWindowRef

	switch (legendRef)
	{
	case PositionReference::BottomLeft:
	case PositionReference::MiddleLeft:
	case PositionReference::TopLeft:
		break;

	default:
	case PositionReference::BottomCenter:
	case PositionReference::Center:
	case PositionReference::TopCenter:
		x += mWidth * 0.5;
		break;

	case PositionReference::BottomRight:
	case PositionReference::MiddleRight:
	case PositionReference::TopRight:
		if (windowRef == PositionReference::BottomRight ||
			windowRef == PositionReference::MiddleRight ||
			windowRef == PositionReference::TopRight)
			x -= mWidth;
		else
			x += mWidth;
		break;
	}

	switch (legendRef)
	{
	case PositionReference::BottomLeft:
	case PositionReference::BottomCenter:
	case PositionReference::BottomRight:
		break;

	default:
	case PositionReference::MiddleLeft:
	case PositionReference::Center:
	case PositionReference::MiddleRight:
		y += mHeight * 0.5;
		break;

	case PositionReference::TopLeft:
	case PositionReference::TopCenter:
	case PositionReference::TopRight:
		if (windowRef == PositionReference::TopLeft ||
			windowRef == PositionReference::TopCenter ||
			windowRef == PositionReference::TopRight)
			y -= mHeight;
		else
			y += mHeight;
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
//		std::vector<std::pair<double, double>>
//
//=============================================================================
std::vector<std::pair<double, double>> Legend::BuildBorderPoints() const
{
	std::vector<std::pair<double, double>> corners(GetCornerVertices());
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
	buffer.vertexBuffer.resize(buffer.vertexCount * (4 + mRenderWindow.GetVertexDimension()));
	assert(mRenderWindow.GetVertexDimension() == 2);

	buffer.indexBuffer.resize(6);

	std::vector<std::pair<double, double>> corners(GetCornerVertices());
	buffer.vertexBuffer[0] = static_cast<float>(corners[0].first);
	buffer.vertexBuffer[1] = static_cast<float>(corners[0].second);

	buffer.vertexBuffer[2] = static_cast<float>(corners[1].first);
	buffer.vertexBuffer[3] = static_cast<float>(corners[1].second);

	buffer.vertexBuffer[4] = static_cast<float>(corners[2].first);
	buffer.vertexBuffer[5] = static_cast<float>(corners[2].second);

	buffer.vertexBuffer[6] = static_cast<float>(corners[3].first);
	buffer.vertexBuffer[7] = static_cast<float>(corners[3].second);

	buffer.vertexBuffer[8] = static_cast<float>(mBackgroundColor.GetRed());
	buffer.vertexBuffer[9] = static_cast<float>(mBackgroundColor.GetGreen());
	buffer.vertexBuffer[10] = static_cast<float>(mBackgroundColor.GetBlue());
	buffer.vertexBuffer[11] = static_cast<float>(mBackgroundColor.GetAlpha());

	buffer.vertexBuffer[12] = static_cast<float>(mBackgroundColor.GetRed());
	buffer.vertexBuffer[13] = static_cast<float>(mBackgroundColor.GetGreen());
	buffer.vertexBuffer[14] = static_cast<float>(mBackgroundColor.GetBlue());
	buffer.vertexBuffer[15] = static_cast<float>(mBackgroundColor.GetAlpha());

	buffer.vertexBuffer[16] = static_cast<float>(mBackgroundColor.GetRed());
	buffer.vertexBuffer[17] = static_cast<float>(mBackgroundColor.GetGreen());
	buffer.vertexBuffer[18] = static_cast<float>(mBackgroundColor.GetBlue());
	buffer.vertexBuffer[19] = static_cast<float>(mBackgroundColor.GetAlpha());

	buffer.vertexBuffer[20] = static_cast<float>(mBackgroundColor.GetRed());
	buffer.vertexBuffer[21] = static_cast<float>(mBackgroundColor.GetGreen());
	buffer.vertexBuffer[22] = static_cast<float>(mBackgroundColor.GetBlue());
	buffer.vertexBuffer[23] = static_cast<float>(mBackgroundColor.GetAlpha());

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
	const unsigned int lineYOffset(mEntrySpacing);

	Primitive::BufferInfo buffer;

	double x, y;
	GetAdjustedPosition(x, y);

	x += mEntrySpacing + 0.5 * mSampleLength;
	y += mHeight + lineYOffset;

	for (const auto& entry : mEntries)
	{
		double halfSize(entry.markerSize * 2.0);// This relationship comes from PlotCurve class
		y -= mEntrySpacing + mTextHeight;

		if (halfSize <= 0.0)
			continue;

		buffer.vertexCount = 4;
		buffer.vertexBuffer.resize(buffer.vertexCount * (mRenderWindow.GetVertexDimension() + 4));
		assert(mRenderWindow.GetVertexDimension() == 2);

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

		mBufferVector.push_back(std::move(buffer));
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

	for (const auto& b : mBufferVector)
	{
		buffer.indexBuffer.insert(buffer.indexBuffer.end(),
			b.indexBuffer.begin(), b.indexBuffer.end());
		buffer.vertexBuffer.insert(buffer.vertexBuffer.end(),
			b.vertexBuffer.begin(), b.vertexBuffer.end());
		buffer.vertexCount += b.vertexCount;
	}

	const unsigned int colorStart(
		buffer.vertexCount * mRenderWindow.GetVertexDimension());

	unsigned int j, k(0), m(0), indexShift(0);
	for (auto& b : mBufferVector)
	{
		unsigned int bufferColorStart(
			b.vertexCount * mRenderWindow.GetVertexDimension());

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
	mBufferVector.clear();

	return buffer;
}

//=============================================================================
// Class:			Legend
// Function:		BuildSampleLines
//
// Description:		Appends mLines corresponding to the legend mEntries to the
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
	const unsigned int lineYOffset(mEntrySpacing);

	double x, y;
	GetAdjustedPosition(x, y);

	y += mHeight + lineYOffset;

	for (const auto& entry : mEntries)
	{
		mLines.SetLineColor(entry.color);
		mLines.SetBackgroundColorForAlphaFade();
		mLines.SetWidth(entry.lineSize);

		y -= mEntrySpacing + mTextHeight;

		mBufferVector.push_back(BufferInfo());
		mLines.Build(x + mEntrySpacing, y, x + mEntrySpacing + mSampleLength,
			y, mBufferVector.back(), Line::UpdateMethod::Manual);
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
		sizeof(GLfloat) * bufferInfo.vertexCount * (mRenderWindow.GetVertexDimension() + 4),
		bufferInfo.vertexBuffer.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(mRenderWindow.GetPositionLocation());
	glVertexAttribPointer(mRenderWindow.GetPositionLocation(),
		mRenderWindow.GetVertexDimension(), GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(mRenderWindow.GetColorLocation());
	glVertexAttribPointer(mRenderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * bufferInfo.vertexCount * mRenderWindow.GetVertexDimension()));

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
	y += mHeight;
	
	for (const auto& entry : mEntries)
	{
		y -= mEntrySpacing + mTextHeight;
		mText.SetPosition(x + 2 * mEntrySpacing + mSampleLength, y);
		mText.AppendText(entry.text.ToStdString());
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
	mModified = true;
	for (auto& buffer : mBufferInfo)
		buffer.vertexCountModified = true;
}

}// namespace LibPlot2D
