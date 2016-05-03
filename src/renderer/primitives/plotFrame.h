/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotFrame.h
// Created:  4/27/2016
// Author:  K. Loux
// Description:  Derived from Primitive for masking outer edges of plot area.
// History:

#ifndef PLOT_FRAME_H_
#define PLOT_FRAME_H_

// Local headers
#include "renderer/primitives/primitive.h"

// Local forward declarations
class Axis;

class PlotFrame : public Primitive
{
public:
	PlotFrame(RenderWindow &renderWindow, const Axis& top, const Axis& bottom,
		const Axis& left, const Axis& right);
	virtual ~PlotFrame();

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	virtual bool HasValidParameters();
	virtual void Update();
	virtual void GenerateGeometry();
	virtual void InitializeVertexBuffer();

private:
	const Axis *top, *bottom, *left, *right;
	unsigned int *uiVertices;
};

#endif// PLOT_FRAME_H_