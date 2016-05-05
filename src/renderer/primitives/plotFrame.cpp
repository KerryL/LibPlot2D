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
//		bottom			= const Axis&
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
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotFrame::InitializeVertexBuffer(const unsigned int& i)
{
	bufferInfo[i].vertexCount = 24;
	bufferInfo[i].vertexBuffer = new float[bufferInfo[i].vertexCount * (renderWindow.GetVertexDimension() + 4)];
	assert(renderWindow.GetVertexDimension() == 2);
}

//==========================================================================
// Class:			PlotFrame
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
void PlotFrame::Update(const unsigned int& i)
{
	// TODO:  This doesn't get updated when axes window offsets change...
	int width, height;
	renderWindow.GetSize(&width, &height);

	FillColorBuffer();

	// Left
	bufferInfo[i].vertexBuffer[0] = 0.0f;
	bufferInfo[i].vertexBuffer[1] = 0.0f;

	bufferInfo[i].vertexBuffer[2] = left->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[3] = 0.0f;

	bufferInfo[i].vertexBuffer[4] = left->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[5] = height;

	bufferInfo[i].vertexBuffer[6] = left->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[7] = height;

	bufferInfo[i].vertexBuffer[8] = 0.0f;
	bufferInfo[i].vertexBuffer[9] = height;

	bufferInfo[i].vertexBuffer[10] = 0.0f;
	bufferInfo[i].vertexBuffer[11] = 0.0f;

	// Bottom
	bufferInfo[i].vertexBuffer[12] = left->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[13] = 0.0f;

	bufferInfo[i].vertexBuffer[14] = width - right->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[15] = 0.0f;

	bufferInfo[i].vertexBuffer[16] = width - right->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[17] = bottom->GetOffsetFromWindowEdge();

	bufferInfo[i].vertexBuffer[18] = width - right->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[19] = bottom->GetOffsetFromWindowEdge();

	bufferInfo[i].vertexBuffer[20] = left->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[21] = bottom->GetOffsetFromWindowEdge();

	bufferInfo[i].vertexBuffer[22] = left->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[23] = 0.0f;

	// Right
	bufferInfo[i].vertexBuffer[24] = width - right->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[25] = 0.0f;

	bufferInfo[i].vertexBuffer[26] = width;
	bufferInfo[i].vertexBuffer[27] = 0.0f;

	bufferInfo[i].vertexBuffer[28] = width;
	bufferInfo[i].vertexBuffer[29] = height;

	bufferInfo[i].vertexBuffer[30] = width;
	bufferInfo[i].vertexBuffer[31] = height;

	bufferInfo[i].vertexBuffer[32] = width - right->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[33] = height;

	bufferInfo[i].vertexBuffer[34] = width - right->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[35] = 0.0f;

	// Top
	bufferInfo[i].vertexBuffer[36] = left->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[37] = height - top->GetOffsetFromWindowEdge();

	bufferInfo[i].vertexBuffer[38] = width - right->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[39] = height - top->GetOffsetFromWindowEdge();

	bufferInfo[i].vertexBuffer[40] = width - right->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[41] = height;

	bufferInfo[i].vertexBuffer[42] = width - right->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[43] = height;

	bufferInfo[i].vertexBuffer[44] = left->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[45] = height;

	bufferInfo[i].vertexBuffer[46] = left->GetOffsetFromWindowEdge();
	bufferInfo[i].vertexBuffer[47] = height - top->GetOffsetFromWindowEdge();

	glBindVertexArray(bufferInfo[i].vertexArrayIndex);

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo[i].vertexBufferIndex);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * bufferInfo[i].vertexCount * (renderWindow.GetVertexDimension() + 4),
		bufferInfo[i].vertexBuffer, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(renderWindow.GetPositionLocation());
	glVertexAttribPointer(renderWindow.GetPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(renderWindow.GetColorLocation());
	glVertexAttribPointer(renderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * renderWindow.GetVertexDimension() * bufferInfo[i].vertexCount));

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
	const unsigned int start(bufferInfo[0].vertexCount * renderWindow.GetVertexDimension());
	unsigned int i;
	for (i = 0; i < bufferInfo[0].vertexCount; i++)
	{
		bufferInfo[0].vertexBuffer[start + i * 4] = (float)color.GetRed();
		bufferInfo[0].vertexBuffer[start + i * 4 + 1] = (float)color.GetGreen();
		bufferInfo[0].vertexBuffer[start + i * 4 + 2] = (float)color.GetBlue();
		bufferInfo[0].vertexBuffer[start + i * 4 + 3] = (float)color.GetAlpha();
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
	if (bufferInfo.size() == 0)
		return;

	glBindVertexArray(bufferInfo[0].vertexArrayIndex);
	glDrawArrays(GL_TRIANGLES, 0, bufferInfo[0].vertexCount);
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