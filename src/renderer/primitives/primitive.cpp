/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  primitive.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Abstract base class for creating 3D objects.
// History:

// GLEW headers
#include <GL\glew.h>

// Local headers
#include "renderer/primitives/primitive.h"
#include "renderer/renderWindow.h"

//==========================================================================
// Class:			Primitive
// Function:		Primitive
//
// Description:		Constructor for the Primitive class.
//
// Input Arguments:
//		renderWindow	= RenderWindow& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Primitive::Primitive(RenderWindow &renderWindow) : renderWindow(renderWindow)
{
	isVisible = true;

	SetColor(Color::ColorBlack);
	drawOrder = 1000;

	renderWindow.AddActor(this);
	renderWindow.SetNeedAlphaSort();
	renderWindow.SetNeedOrderSort();

	// Add a default info block to ensure the initialize and update functions get called
	bufferInfo.push_back(BufferInfo());
	bufferInfo.back().vertexBuffer = NULL;
	bufferInfo.back().vertexCountModified = true;

	modified = true;
}

//==========================================================================
// Class:			Primitive
// Function:		Primitive
//
// Description:		Copy constructor for the Primitive class.
//
// Input Arguments:
//		primitive	= const Primitive& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Primitive::Primitive(const Primitive &primitive) : renderWindow(primitive.renderWindow)
{
	*this = primitive;

	modified = true;

	renderWindow.SetNeedAlphaSort();
	renderWindow.SetNeedOrderSort();
}

//==========================================================================
// Class:			Primitive
// Function:		~Primitive
//
// Description:		Destructor for the Primitive class.
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
Primitive::~Primitive()
{
	renderWindow.SetNeedAlphaSort();
	renderWindow.SetNeedOrderSort();

	unsigned int i;
	for (i = 0; i < bufferInfo.size(); i++)
	{
		glDeleteBuffers(1, &bufferInfo[i].vertexBufferIndex);
		glDeleteVertexArrays(1, &bufferInfo[i].vertexArrayIndex);

		delete[] bufferInfo[i].vertexBuffer;
		bufferInfo[i].vertexBuffer = NULL;
	}
}

//==========================================================================
// Class:			Primitive
// Function:		Draw
//
// Description:		Calls two mandatory overloads that 1) check to see if the
//					information describing this object is valid, and if so, 2)
//					calls the GenerateGeometry() method to create the object.
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
void Primitive::Draw()
{
	if (!HasValidParameters() || !isVisible)
		return;

	unsigned int i;
	for (i = 0; i < bufferInfo.size(); i++)
	{
		if (bufferInfo[i].vertexCountModified || modified)
			Update(i);
	}

	modified = false;
	GenerateGeometry();
}

//==========================================================================
// Class:			Primitive
// Function:		SetVisibility
//
// Description:		Sets the visibility flag for this object.
//
// Input Arguments:
//		isVisible	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Primitive::SetVisibility(const bool &isVisible)
{
	this->isVisible = isVisible;
	modified = true;
}

//==========================================================================
// Class:			Primitive
// Function:		SetColor
//
// Description:		Sets the color of this object.
//
// Input Arguments:
//		color	= const Color&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Primitive::SetColor(const Color &color)
{
	this->color = color;
	renderWindow.SetNeedAlphaSort();
	modified = true;
}

//==========================================================================
// Class:			Primitive
// Function:		SetDrawOrder
//
// Description:		Sets the draw order for the object.
//
// Input Arguments:
//		drawOrder	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Primitive::SetDrawOrder(const unsigned int& drawOrder)
{
	this->drawOrder = drawOrder;
	renderWindow.SetNeedOrderSort();
}

//==========================================================================
// Class:			Primitive
// Function:		operator=
//
// Description:		Assignment operator for Primitive class.
//
// Input Arguments:
//		Primitive	= const Primitive& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive&, reference to this object
//
//==========================================================================
Primitive& Primitive::operator=(const Primitive &primitive)
{
	// Check for self-assignment
	if (this == &primitive)
		return *this;

	// Perform the assignment
	isVisible	= primitive.isVisible;
	color		= primitive.color;
	modified	= primitive.modified;
	drawOrder	= primitive.drawOrder;

	// TODO:  Need to go over handling of openGL stuff here
	assert(false);

	return *this;
}

//==========================================================================
// Class:			Primitive
// Function:		EnableAlphaBlending
//
// Description:		Assignment operator for Primitive class.
//
// Input Arguments:
//		Primitive	= const Primitive& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive&, reference to this object
//
//==========================================================================
void Primitive::EnableAlphaBlending()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Also disable the Z-buffer
	glDepthMask(GL_FALSE);
}

//==========================================================================
// Class:			Primitive
// Function:		DisableAlphaBlending
//
// Description:		Assignment operator for Primitive class.
//
// Input Arguments:
//		Primitive	= const Primitive& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive&, reference to this object
//
//==========================================================================
void Primitive::DisableAlphaBlending()
{
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}
