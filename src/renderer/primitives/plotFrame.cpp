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
	vertexBuffer = new float[vertexCount * (renderWindow.GetVertexDimension() + 4)];

	glGenVertexArrays(1, &vertexArrayIndex);
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

	FillColorBuffer();

	// Left
	vertexBuffer[0] = 0.0f;
	vertexBuffer[1] = 0.0f;

	vertexBuffer[2] = left->GetOffsetFromWindowEdge();
	vertexBuffer[3] = 0.0f;

	vertexBuffer[4] = left->GetOffsetFromWindowEdge();
	vertexBuffer[5] = height;

	vertexBuffer[6] = left->GetOffsetFromWindowEdge();
	vertexBuffer[7] = height;

	vertexBuffer[8] = 0.0f;
	vertexBuffer[9] = height;

	vertexBuffer[10] = 0.0f;
	vertexBuffer[11] = 0.0f;

	// Bottom
	vertexBuffer[12] = left->GetOffsetFromWindowEdge();
	vertexBuffer[13] = 0.0f;

	vertexBuffer[14] = width - right->GetOffsetFromWindowEdge();
	vertexBuffer[15] = 0.0f;

	vertexBuffer[16] = width - right->GetOffsetFromWindowEdge();
	vertexBuffer[17] = bottom->GetOffsetFromWindowEdge();

	vertexBuffer[18] = width - right->GetOffsetFromWindowEdge();
	vertexBuffer[19] = bottom->GetOffsetFromWindowEdge();

	vertexBuffer[20] = left->GetOffsetFromWindowEdge();
	vertexBuffer[21] = bottom->GetOffsetFromWindowEdge();

	vertexBuffer[22] = left->GetOffsetFromWindowEdge();
	vertexBuffer[23] = 0.0f;

	// Right
	vertexBuffer[24] = width - right->GetOffsetFromWindowEdge();
	vertexBuffer[25] = 0.0f;

	vertexBuffer[26] = width;
	vertexBuffer[27] = 0.0f;

	vertexBuffer[28] = width;
	vertexBuffer[29] = height;

	vertexBuffer[30] = width;
	vertexBuffer[31] = height;

	vertexBuffer[32] = width - right->GetOffsetFromWindowEdge();
	vertexBuffer[33] = height;

	vertexBuffer[34] = width - right->GetOffsetFromWindowEdge();
	vertexBuffer[35] = 0.0f;

	// Top
	vertexBuffer[36] = left->GetOffsetFromWindowEdge();
	vertexBuffer[37] = height - top->GetOffsetFromWindowEdge();

	vertexBuffer[38] = width - right->GetOffsetFromWindowEdge();
	vertexBuffer[39] = height - top->GetOffsetFromWindowEdge();

	vertexBuffer[40] = width - right->GetOffsetFromWindowEdge();
	vertexBuffer[41] = height;

	vertexBuffer[42] = width - right->GetOffsetFromWindowEdge();
	vertexBuffer[43] = height;

	vertexBuffer[44] = left->GetOffsetFromWindowEdge();
	vertexBuffer[45] = height;

	vertexBuffer[46] = left->GetOffsetFromWindowEdge();
	vertexBuffer[47] = height - top->GetOffsetFromWindowEdge();

	glBindVertexArray(vertexArrayIndex);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIndex);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * vertexCount * (renderWindow.GetVertexDimension() + 4),
		vertexBuffer, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(renderWindow.GetPositionLocation());
	glVertexAttribPointer(renderWindow.GetPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(renderWindow.GetColorLocation());
	glVertexAttribPointer(renderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * renderWindow.GetVertexDimension() * vertexCount));

	glBindVertexArray(0);
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
	const unsigned int start(vertexCount * renderWindow.GetVertexDimension());
	unsigned int i;
	for (i = 0; i < vertexCount; i++)
	{
		vertexBuffer[start + i * 4] = (float)color.GetRed();
		vertexBuffer[start + i * 4 + 1] = (float)color.GetGreen();
		vertexBuffer[start + i * 4 + 2] = (float)color.GetBlue();
		vertexBuffer[start + i * 4 + 3] = (float)color.GetAlpha();
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
	glBindVertexArray(vertexArrayIndex);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
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