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
	void SetFontColor(const Color &color) { fontColor = color; RequiresRedraw(); }
	void SetBackgroundColor(const Color &color) { backgroundColor = color; RequiresRedraw(); }
	void SetBorderColor(const Color &color) { borderColor = color; RequiresRedraw(); }
	void SetBorderSize(const unsigned int &size) { borderSize = size; RequiresRedraw(); }
	void SetSampleLineLength(const unsigned int &size) { sampleLength = size; RequiresRedraw(); }

	enum PositionReference
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

	void SetWindowReference(const PositionReference &windowRef) { this->windowRef = windowRef; modified = true; }
	void SetLegendReference(const PositionReference &legendRef) { this->legendRef = legendRef; modified = true; }

	PositionReference GetWindowReference() const { return windowRef; }
	PositionReference GetLegendReference() const { return legendRef; }

	// Specifies bottom left corner of legend
	void SetPosition(const double &x, const double &y) { this->x = x; this->y = y; RequiresRedraw(); }
	void SetDeltaPosition(const double &x, const double &y);

	// These are w.r.t. reference specified by SetWindowRef()
	inline double GetXPos() const { return x; }
	inline double GetYPos() const { return y; }
	inline void GetPosition(double &x, double &y) const { x = this->x; y = this->y; }

	double GetXPos(const PositionReference& legendRef, const PositionReference& windowRef) const;
	double GetYPos(const PositionReference& legendRef, const PositionReference& windowRef) const;
	void GetPosition(const PositionReference& legendRef, const PositionReference& windowRef, double &x, double &y) const;

	inline unsigned int GetHeight() const { return height; }
	inline unsigned int GetWidth() const { return width; }

	struct LegendEntryInfo
	{
		wxString text;
		Color color;
		double lineSize;
		int markerSize;
	};

	void SetContents(const std::vector<LegendEntryInfo> &entries) { this->entries = entries; RequiresRedraw(); }

	bool IsUnder(const unsigned int &x, const unsigned int &y) const;

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	virtual bool HasValidParameters();
	virtual void Update(const unsigned int& i);
	virtual void GenerateGeometry();

private:
	static const unsigned int entrySpacing;// [pixels]
	Text text;

	Color fontColor;
	Color backgroundColor;
	Color borderColor;

	std::vector<LegendEntryInfo> entries;

	double x, y;// [pixels]
	unsigned int borderSize;// [pixels]
	unsigned int sampleLength;// [pixels]
	double textHeight;// [pixels]

	std::vector<std::pair<double, double> > GetCornerVertices() const;

	void UpdateBoundingBox();
	unsigned int height;
	unsigned int width;

	PositionReference windowRef;
	PositionReference legendRef;
	void GetAdjustedPosition(double &x, double &y) const;

	Line lines;

	std::vector<Primitive::BufferInfo> bufferVector;
	Primitive::BufferInfo BuildBackground() const;
	void BuildMarkers();
	void BuildLabelStrings();
	Primitive::BufferInfo AssembleBuffers();
	std::vector<std::pair<double, double> > BuildBorderPoints() const;
	void BuildSampleLines();
	void ConfigureVertexArray(Primitive::BufferInfo& buffer) const;
	void RequiresRedraw();
};

}// namespace LibPlot2D

#endif// LEGEND_H_
