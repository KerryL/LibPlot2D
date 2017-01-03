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

#ifndef LEGEND_H_
#define LEGEND_H_

// Standard C++ headers
#include <vector>
#include <utility>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "lp2d/renderer/primitives/primitive.h"
#include "lp2d/renderer/text.h"
#include "lp2d/renderer/line.h"

namespace LibPlot2D
{

class Legend : public Primitive
{
public:
	// Constructor
	explicit Legend(RenderWindow &renderWindow);

	// Destructor
	virtual ~Legend() = default;

	// Set option methods
	void SetFont(const std::string& fontFileName, const double& size);
	void SetFontColor(const Color &color)
	{ mFontColor = color; RequiresRedraw(); }
	void SetBackgroundColor(const Color &color)
	{ mBackgroundColor = color; RequiresRedraw(); }
	void SetBorderColor(const Color &color)
	{ mBorderColor = color; RequiresRedraw(); }
	void SetBorderSize(const unsigned int &size)
	{ mBorderSize = size; RequiresRedraw(); }
	void SetSampleLineLength(const unsigned int &size)
	{ mSampleLength = size; RequiresRedraw(); }

	enum class PositionReference
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

	void SetWindowReference(const PositionReference &windowRef)
	{ mWindowRef = windowRef; mModified = true; }
	void SetLegendReference(const PositionReference &legendRef)
	{ mLegendRef = legendRef; mModified = true; }

	PositionReference GetWindowReference() const { return mWindowRef; }
	PositionReference GetLegendReference() const { return mLegendRef; }

	// Specifies bottom left corner of legend
	void SetPosition(const double &x, const double &y)
	{ mX = x; mY = y; RequiresRedraw(); }
	void SetDeltaPosition(const double &x, const double &y);

	// These are w.r.t. reference specified by SetWindowRef()
	inline double GetXPos() const { return mX; }
	inline double GetYPos() const { return mY; }
	inline void GetPosition(double &x, double &y) const { x = mX; y = mY; }

	double GetXPos(const PositionReference& legendRef,
		const PositionReference& windowRef) const;
	double GetYPos(const PositionReference& legendRef,
		const PositionReference& windowRef) const;
	void GetPosition(const PositionReference& legendRef,
		const PositionReference& windowRef, double &x, double &y) const;

	inline unsigned int GetHeight() const { return mHeight; }
	inline unsigned int GetWidth() const { return mWidth; }

	struct LegendEntryInfo
	{
		wxString text;
		Color color;
		double lineSize;
		int markerSize;
	};

	void SetContents(const std::vector<LegendEntryInfo> &entries) { mEntries = entries; RequiresRedraw(); }

	bool IsUnder(const unsigned int &x, const unsigned int &y) const;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	virtual bool HasValidParameters();
	virtual void Update(const unsigned int& i);
	virtual void GenerateGeometry();

private:
	static const unsigned int mEntrySpacing;// [pixels]
	Text mText;

	Color mFontColor = Color::ColorBlack;
	Color mBackgroundColor = Color::ColorWhite;
	Color mBorderColor = Color::ColorBlack;

	std::vector<LegendEntryInfo> mEntries;

	double mX = 0.0;
	double mY = 0.0;// [pixels]
	unsigned int mBorderSize = 1;// [pixels]
	unsigned int mSampleLength = 15;// [pixels]
	double mTextHeight;// [pixels]

	std::vector<std::pair<double, double>> GetCornerVertices() const;

	void UpdateBoundingBox();
	unsigned int mHeight;
	unsigned int mWidth;

	PositionReference mWindowRef = PositionReference::BottomLeft;
	PositionReference mLegendRef = PositionReference::Center;
	void GetAdjustedPosition(double &x, double &y) const;

	Line mLines;

	std::vector<Primitive::BufferInfo> mBufferVector;
	Primitive::BufferInfo BuildBackground() const;
	void BuildMarkers();
	void BuildLabelStrings();
	Primitive::BufferInfo AssembleBuffers();
	std::vector<std::pair<double, double>> BuildBorderPoints() const;
	void BuildSampleLines();
	void ConfigureVertexArray(Primitive::BufferInfo& buffer) const;
	void RequiresRedraw();
};

}// namespace LibPlot2D

#endif// LEGEND_H_
