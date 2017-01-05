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

/// Class for representing a plot legend.  Lists the names of all visible
/// curves and shows a sample of the curve itself.
class Legend : public Primitive
{
public:
	/// Constructor.
	///
	/// \param renderWindow The window that owns this primitive.
	explicit Legend(RenderWindow &renderWindow);
	~Legend() = default;

	/// Position reference enumeration.
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

	/// Structure for describing a legend entry.
	struct LegendEntryInfo
	{
		wxString text;///< Text to display next to curve sample.
		Color color;///< Curve color.
		double lineSize;///< Size of curve line.
		int markerSize;///< Size of curve marker.
	};

	/// \name Set option methods
	/// @{

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

	void SetWindowReference(const PositionReference &windowRef)
	{ mWindowRef = windowRef; mModified = true; }
	void SetLegendReference(const PositionReference &legendRef)
	{ mLegendRef = legendRef; mModified = true; }

	/// Specifies bottom left corner of legend.
	///
	/// \param x X-location of left side of legend.
	/// \param y Y-location of bottom of legend.
	void SetPosition(const double &x, const double &y)
	{ mX = x; mY = y; RequiresRedraw(); }

	/// Sets the change in position (for use when dragging).
	///
	/// \param x Change in x-position.
	/// \param y Change in y-position.
	void SetDeltaPosition(const double &x, const double &y);

	void SetContents(const std::vector<LegendEntryInfo> &entries)
	{ mEntries = entries; RequiresRedraw(); }

	/// @}

	/// \name Getters
	/// @{

	PositionReference GetWindowReference() const { return mWindowRef; }
	PositionReference GetLegendReference() const { return mLegendRef; }

	/// Gets the x-location with respect to the reference specified by
	/// SetWindowRef().
	/// \returns The x-location of the legend.
	inline double GetXPos() const { return mX; }

	/// Gets the y-location with respect to the reference specified by
	/// SetWindowRef().
	/// \returns The y-location of the legend.
	inline double GetYPos() const { return mY; }

	/// Gets the location with respect to the reference specified by
	/// SetWindowRef().
	///
	/// \param x [out] The x-location of the legend.
	/// \param y [out] The y-location of the legend.
	inline void GetPosition(double &x, double &y) const { x = mX; y = mY; }

	double GetXPos(const PositionReference& legendRef,
		const PositionReference& windowRef) const;
	double GetYPos(const PositionReference& legendRef,
		const PositionReference& windowRef) const;
	void GetPosition(const PositionReference& legendRef,
		const PositionReference& windowRef, double &x, double &y) const;

	inline unsigned int GetHeight() const { return mHeight; }
	inline unsigned int GetWidth() const { return mWidth; }

	/// @}

	/// Checks to see if the specified pixel is within the area covered by this
	/// object.
	///
	/// \param x X-location of test point.
	/// \param y Y-location of test point.
	///
	/// \returns True if (\p x, \p y) lies within the legend area.
	bool IsUnder(const unsigned int &x, const unsigned int &y) const;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool HasValidParameters() override;
	void Update(const unsigned int& i) override;
	void GenerateGeometry() override;

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
