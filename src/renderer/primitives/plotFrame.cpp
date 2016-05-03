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
	delete [] uiVertices;
	uiVertices = NULL;
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
	vertexCount = 48;
	uiVertices = new unsigned int[vertexCount];

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

	// Left
	uiVertices[0] = 0;											uiVertices[1] = 0;
	uiVertices[2] = left->GetOffsetFromWindowEdge();			uiVertices[3] = 0;
	uiVertices[4] = left->GetOffsetFromWindowEdge();			uiVertices[5] = height;

	uiVertices[6] = left->GetOffsetFromWindowEdge();			uiVertices[7] = height;
	uiVertices[8] = 0;											uiVertices[9] = height;
	uiVertices[10] = 0;											uiVertices[11] = 0;

	// Bottom
	uiVertices[12] = left->GetOffsetFromWindowEdge();			uiVertices[13] = 0;
	uiVertices[14] = width - right->GetOffsetFromWindowEdge();	uiVertices[15] = 0;
	uiVertices[16] = width - right->GetOffsetFromWindowEdge();	uiVertices[17] = bottom->GetOffsetFromWindowEdge();

	uiVertices[18] = width - right->GetOffsetFromWindowEdge();	uiVertices[19] = bottom->GetOffsetFromWindowEdge();
	uiVertices[20] = left->GetOffsetFromWindowEdge();			uiVertices[21] = bottom->GetOffsetFromWindowEdge();
	uiVertices[22] = left->GetOffsetFromWindowEdge();			uiVertices[23] = 0;

	// Right
	uiVertices[24] = width - right->GetOffsetFromWindowEdge();	uiVertices[25] = 0;
	uiVertices[26] = width;										uiVertices[27] = 0;
	uiVertices[28] = width;										uiVertices[29] = height;

	uiVertices[30] = width;										uiVertices[31] = height;
	uiVertices[32] = width - right->GetOffsetFromWindowEdge();	uiVertices[33] = height;
	uiVertices[34] = width - right->GetOffsetFromWindowEdge();	uiVertices[35] = 0;

	// Top
	uiVertices[36] = left->GetOffsetFromWindowEdge();			uiVertices[37] = height - top->GetOffsetFromWindowEdge();
	uiVertices[38] = width - right->GetOffsetFromWindowEdge();	uiVertices[39] = height - top->GetOffsetFromWindowEdge();
	uiVertices[40] = width - right->GetOffsetFromWindowEdge();	uiVertices[41] = height;

	uiVertices[42] = width - right->GetOffsetFromWindowEdge();	uiVertices[43] = height;
	uiVertices[44] = left->GetOffsetFromWindowEdge();			uiVertices[45] = height;
	uiVertices[46] = left->GetOffsetFromWindowEdge();			uiVertices[47] = height - top->GetOffsetFromWindowEdge();

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIndex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * vertexCount, uiVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_UNSIGNED_INT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
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