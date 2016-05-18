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
#include <utility>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "renderer/primitives/primitive.h"
#include "renderer/text.h"
#include "renderer/line.h"

class Legend : public Primitive
{
public:
	// Constructor
	Legend(RenderWindow &renderWindow);

	// Destructor
	virtual ~Legend() {}

	// Set option methods
	void SetFont(const std::string& fontFileName, const double& size);
	void SetFontColor(const Color &color) { fontColor = color; modified = true; }
	void SetBackgroundColor(const Color &color) { backgroundColor = color; modified = true; }
	void SetBorderColor(const Color &color) { borderColor = color; modified = true; }
	void SetBorderSize(const unsigned int &size) { borderSize = size; modified = true; }
	void SetSampleLineLength(const unsigned int &size) { sampleLength = size; modified = true; }

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
	void SetPosition(const double &x, const double &y) { this->x = x; this->y = y; modified = true; }
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

	void SetContents(const std::vector<LegendEntryInfo> &entries) { this->entries = entries; modified = true; }// TODO:  OGL4 This will need to change size of bufferInfo

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
	Primitive::BufferInfo AssembleBuffers();
	std::vector<std::pair<double, double> > BuildBorderPoints() const;
	void BuildSampleLines();
	void ConfigureVertexArray(Primitive::BufferInfo& buffer) const;
};

#endif// LEGEND_H_