/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotFrame.cpp
// Created:  4/27/2016
// Author:  K. Loux
// Description:  Derived from Primitive for masking outer edges of plot area.
// History:

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "renderer/primitives/plotFrame.h"
#include "renderer/primitives/axis.h"
#include "renderer/renderWindow.h"

//==========================================================================
// Class:			PlotFrame
// Function:		PlotFrame
//
// Description:		Constructor for the PlotFrame class.
//
// Input Arguments:
//		renderWindow	= RenderWindow&
//		top				= const Axis&
//		bootm			= const Axis&
//		left			= const Axis&
//		right			= const Axis&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotFrame::PlotFrame(RenderWindow &renderWindow, const Axis& top, const Axis& bottom,
	const Axis& left, const Axis& right) : Primitive(renderWindow),
	top(&top), bottom(&bottom), left(&left), right(&right)
{
	SetColor(Color::ColorCyan);// TODO:  Remove
	// Draw AFTER plot curves but BEFORE axis labels
	SetDrawOrder(2000);
}

//==========================================================================
// Class:			PlotFrame
// Function:		~PlotFrame
//
// Description:		Destructor for the PlotFrame class.
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
PlotFrame::~PlotFrame()
{
}

//==========================================================================
// Class:			PlotFrame
// Function:		InitializeVertexBuffer
//
// Description:		Initializes the vertex buffer containing this object's vertices.
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
void PlotFrame::InitializeVertexBuffer()
{
	vertexCount = 24;
	vertices = new float[vertexCount * renderWindow.GetVertexDimension()];

	glGenBuffers(1, &vertexBufferIndex);
}

//==========================================================================
// Class:			PlotFrame
// Function:		Update
//
// Description:		Updates the GL buffers associated with this object.
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
void PlotFrame::Update()
{
	// TODO:  This doesn't get updated on a re-size
	int width, height;
	renderWindow.GetSize(&width, &height);

	//FillColorBuffer();

	// Left
	vertices[0] = 0.0f;
	vertices[1] = 0.0f;

	vertices[2] = left->GetOffsetFromWindowEdge();
	vertices[3] = 0.0f;

	vertices[4] = left->GetOffsetFromWindowEdge();
	vertices[5] = height;

	vertices[6] = left->GetOffsetFromWindowEdge();
	vertices[7] = height;

	vertices[8] = 0.0f;
	vertices[9] = height;

	vertices[10] = 0.0f;
	vertices[11] = 0.0f;

	// Bottom
	vertices[12] = left->GetOffsetFromWindowEdge();
	vertices[13] = 0.0f;

	vertices[14] = width - right->GetOffsetFromWindowEdge();
	vertices[15] = 0.0f;

	vertices[16] = width - right->GetOffsetFromWindowEdge();
	vertices[17] = bottom->GetOffsetFromWindowEdge();

	vertices[18] = width - right->GetOffsetFromWindowEdge();
	vertices[19] = bottom->GetOffsetFromWindowEdge();

	vertices[20] = left->GetOffsetFromWindowEdge();
	vertices[21] = bottom->GetOffsetFromWindowEdge();

	vertices[22] = left->GetOffsetFromWindowEdge();
	vertices[23] = 0.0f;

	// Right
	vertices[24] = width - right->GetOffsetFromWindowEdge();
	vertices[25] = 0.0f;

	vertices[26] = width;
	vertices[27] = 0.0f;

	vertices[28] = width;
	vertices[29] = height;

	vertices[30] = width;
	vertices[31] = height;

	vertices[32] = width - right->GetOffsetFromWindowEdge();
	vertices[33] = height;

	vertices[34] = width - right->GetOffsetFromWindowEdge();
	vertices[35] = 0.0f;

	// Top
	vertices[36] = left->GetOffsetFromWindowEdge();
	vertices[37] = height - top->GetOffsetFromWindowEdge();

	vertices[38] = width - right->GetOffsetFromWindowEdge();
	vertices[39] = height - top->GetOffsetFromWindowEdge();

	vertices[40] = width - right->GetOffsetFromWindowEdge();
	vertices[41] = height;

	vertices[42] = width - right->GetOffsetFromWindowEdge();
	vertices[43] = height;

	vertices[44] = left->GetOffsetFromWindowEdge();
	vertices[45] = height;

	vertices[46] = left->GetOffsetFromWindowEdge();
	vertices[47] = height - top->GetOffsetFromWindowEdge();

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIndex);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * vertexCount * renderWindow.GetVertexDimension(),
		vertices, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(renderWindow.GetPositionLocation());
	glVertexAttribPointer(renderWindow.GetPositionLocation(), 2, GL_FLOAT,
		GL_FALSE, /*sizeof(GLfloat) * renderWindow.GetVertexDimension()*/0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//==========================================================================
// Class:			PlotFrame
// Function:		FillColorBuffer
//
// Description:		Writes to the color buffer.
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
void PlotFrame::FillColorBuffer()
{
	unsigned int i;
	for (i = 0; i < vertexCount; i++)
	{
		vertices[i * 8 + 4] = (float)color.GetRed();
		vertices[i * 8 + 5] = (float)color.GetGreen();
		vertices[i * 8 + 6] = (float)color.GetBlue();
		vertices[i * 8 + 7] = (float)color.GetAlpha();
	}
}

//==========================================================================
// Class:			PlotFrame
// Function:		GenerateGeometry
//
// Description:		Draws the geometry associated with this object.
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
void PlotFrame::GenerateGeometry()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIndex);
	glEnableVertexAttribArray(renderWindow.GetPositionLocation());
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glDisableVertexAttribArray(renderWindow.GetPositionLocation());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//==========================================================================
// Class:			PlotFrame
// Function:		HasValidParameters
//
// Description:		Checks to see if this object has enough information to be drawn.
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
bool PlotFrame::HasValidParameters()
{
	return true;
}