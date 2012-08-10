/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  primitive.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Abstract base class for creating 3D objects.
// History:

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
//		_renderWindow	= RenderWindow& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Primitive::Primitive(RenderWindow &_renderWindow) : renderWindow(_renderWindow)
{
	// Initialize private data
	isVisible = true;
	modified = true;

	// Initialize the color to black
	color = Color::ColorBlack;

	// Initialize the list index to zero
	listIndex = 0;

	// Add this object to the renderer
	renderWindow.AddActor(this);
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
	// Do the copy
	*this = primitive;
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
	// Release the glList for this object (if the list exists)
	if (listIndex != 0)
		glDeleteLists(listIndex, 1);
}

//==========================================================================
// Class:			Primitive
// Function:		Draw
//
// Description:		Calls two mandatory overloads that 1) check to see if the
//					information describing this object is valid, and if so, 2)
//					calls the GenerateGeometry() method to create the object.
//					Uses glLists if geometry has already been created and all
//					information is up-to-date.
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
void Primitive::Draw(void)
{
	if (modified || listIndex == 0)
	{
		modified = false;

		if (listIndex == 0)
			listIndex = glGenLists(1);

		glNewList(listIndex, GL_COMPILE);

		if (!HasValidParameters() || !isVisible)
		{
			glEndList();
			return;
		}

		glColor4d(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());

		// If the object is transparent, enable alpha blending
		/*if (color.GetAlpha() != 1.0)
			EnableAlphaBlending();*/

		GenerateGeometry();

		/*if (color.GetAlpha() != 1.0)
			DisableAlphaBlending();*/

		glEndList();
	}

	if (listIndex != 0)
		glCallList(listIndex);
}

//==========================================================================
// Class:			Primitive
// Function:		SetVisibility
//
// Description:		Sets the visibility flag for this object.
//
// Input Arguments:
//		_isVisible	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Primitive::SetVisibility(const bool &_isVisible)
{
	// Set the visibility flag to the argument
	isVisible = _isVisible;
	
	// Reset the modified flag
	modified = true;
}

//==========================================================================
// Class:			Primitive
// Function:		SetColor
//
// Description:		Sets the color of this object.
//
// Input Arguments:
//		_Color	= const Color&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Primitive::SetColor(const Color &_color)
{
	// Set the color to the argument
	color = _color;
	
	// Reset the modified flag
	modified = true;
}

//==========================================================================
// Class:			Primitive
// Function:		operator =
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
Primitive& Primitive::operator = (const Primitive &primitive)
{
	// Check for self-assignment
	if (this == &primitive)
		return *this;

	// Perform the assignment
	isVisible	= primitive.isVisible;
	color		= primitive.color;
	modified	= primitive.modified;

	// The list index just gets zeroed out
	listIndex = 0;

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
void Primitive::EnableAlphaBlending(void)
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
void Primitive::DisableAlphaBlending(void)
{
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}