/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  primitive.cpp
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Abstract base class for creating 3D objects.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "lp2d/renderer/primitives/primitive.h"
#include "lp2d/renderer/renderWindow.h"

namespace LibPlot2D
{

//=============================================================================
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
//=============================================================================
Primitive::Primitive(RenderWindow &renderWindow) : mRenderWindow(renderWindow)
{
	mRenderWindow.AddActor(std::unique_ptr<Primitive>(this));
	mRenderWindow.SetNeedAlphaSort();
	mRenderWindow.SetNeedOrderSort();

	// Add a default info block to ensure the initialize and update functions get called
	mBufferInfo.push_back(BufferInfo());
}

//=============================================================================
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
//=============================================================================
Primitive::Primitive(const Primitive &primitive) : mRenderWindow(primitive.mRenderWindow)
{
	*this = primitive;
}

//=============================================================================
// Class:			Primitive
// Function:		Primitive
//
// Description:		Move constructor for the Primitive class.
//
// Input Arguments:
//		primitive	= Primitive&& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
Primitive::Primitive(Primitive&& primitive) : mRenderWindow(primitive.mRenderWindow)
{
	*this = std::move(primitive);
}

//=============================================================================
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
//=============================================================================
Primitive::~Primitive()
{
	mRenderWindow.SetNeedAlphaSort();
	mRenderWindow.SetNeedOrderSort();
}

//=============================================================================
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
//=============================================================================
void Primitive::Draw()
{
	if (!HasValidParameters() || !mIsVisible)
		return;

	unsigned int i;
	for (i = 0; i < mBufferInfo.size(); ++i)
	{
		if (mBufferInfo[i].vertexCountModified || mModified)
			Update(i);
		assert(!RenderWindow::GLHasError());
	}

	mModified = false;
	GenerateGeometry();

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
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
//=============================================================================
void Primitive::SetVisibility(const bool &isVisible)
{
	mIsVisible = isVisible;
	mModified = true;
}

//=============================================================================
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
//=============================================================================
void Primitive::SetColor(const Color &color)
{
	mColor = color;
	mRenderWindow.SetNeedAlphaSort();
	mModified = true;
}

//=============================================================================
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
//=============================================================================
void Primitive::SetDrawOrder(const unsigned int& drawOrder)
{
	mDrawOrder = drawOrder;
	mRenderWindow.SetNeedOrderSort();
}

//=============================================================================
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
//=============================================================================
Primitive& Primitive::operator=(const Primitive &primitive)
{
	// Check for self-assignment
	if (this == &primitive)
		return *this;

	// Perform the assignment
	mIsVisible	= primitive.mIsVisible;
	mColor		= primitive.mColor;
	mModified	= true;
	mDrawOrder	= primitive.mDrawOrder;

	mRenderWindow.SetNeedAlphaSort();
	mRenderWindow.SetNeedOrderSort();

	return *this;
}

//=============================================================================
// Class:			Primitive
// Function:		operator=
//
// Description:		Move assignment operator for Primitive class.
//
// Input Arguments:
//		Primitive	= Primitive&& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive&, reference to this object
//
//=============================================================================
Primitive& Primitive::operator=(Primitive &&primitive)
{
	// Check for self-assignment
	if (this == &primitive)
		return *this;

	// Perform the assignment
	mIsVisible	= std::move(primitive.mIsVisible);
	mColor		= std::move(primitive.mColor);
	mModified	= true;
	mDrawOrder	= std::move(primitive.mDrawOrder);
	mBufferInfo	= std::move(primitive.mBufferInfo);

	mRenderWindow.SetNeedAlphaSort();
	mRenderWindow.SetNeedOrderSort();

	return *this;
}

//=============================================================================
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
//=============================================================================
void Primitive::EnableAlphaBlending()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Also disable the Z-buffer
	glDepthMask(GL_FALSE);
}

//=============================================================================
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
//=============================================================================
void Primitive::DisableAlphaBlending()
{
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

//=============================================================================
// Class:			Primitive::BufferInfo
// Function:		BufferInfo
//
// Description:		Move constructor for BufferInfo class.
//
// Input Arguments:
//		b	= BufferInfo&&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
Primitive::BufferInfo::BufferInfo(BufferInfo&& b)
{
	*this = std::move(b);
}

//=============================================================================
// Class:			Primitive::BufferInfo
// Function:		~BufferInfo
//
// Description:		Destructor for BufferInfo class.
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
//=============================================================================
Primitive::BufferInfo::~BufferInfo()
{
	FreeOpenGLObjects();
}

//=============================================================================
// Class:			Primitive::BufferInfo
// Function:		operator=
//
// Description:		Move assignment operator for BufferInfo class.
//
// Input Arguments:
//		b	= BufferInfo&&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
Primitive::BufferInfo& Primitive::BufferInfo::operator=(BufferInfo&& b)
{
	if (this == &b)
		return *this;

	vertexCount = std::move(b.vertexCount);
	vertexBuffer = std::move(b.vertexBuffer);
	indexBuffer = std::move(b.indexBuffer);
	vertexCountModified = std::move(b.vertexCountModified);

	vertexBufferIndex = std::move(b.vertexBufferIndex);
	vertexArrayIndex = std::move(b.vertexArrayIndex);
	indexBufferIndex = std::move(b.indexBufferIndex);

	glVertexBufferExists = b.glVertexBufferExists;
	glIndexBufferExists = b.glIndexBufferExists;

	b.glVertexBufferExists = false;
	b.glIndexBufferExists = false;

	return *this;
}

//=============================================================================
// Class:			Primitive::BufferInfo
// Function:		GetOpenGLIndices
//
// Description:		Method for safely initializing this object.
//
// Input Arguments:
//		needIndexObject	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Primitive::BufferInfo::GetOpenGLIndices(const bool& needIndexObject)
{
	if (!glVertexBufferExists)
	{
		glGenVertexArrays(1, &vertexArrayIndex);
		glGenBuffers(1, &vertexBufferIndex);
		glVertexBufferExists = true;
	}

	if (needIndexObject && !glIndexBufferExists)
	{
		glGenBuffers(1, &indexBufferIndex);
		glIndexBufferExists = true;
	}

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
// Class:			Primitive::BufferInfo
// Function:		FreeOpenGLObjects
//
// Description:		Frees OpenGL resources.
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
//=============================================================================
void Primitive::BufferInfo::FreeOpenGLObjects()
{
	if (glVertexBufferExists)
	{
		glDeleteVertexArrays(1, &vertexArrayIndex);
		glDeleteBuffers(1, &vertexBufferIndex);
		glVertexBufferExists = false;
	}

	if (glIndexBufferExists)
	{
		glDeleteBuffers(1, &indexBufferIndex);
		glIndexBufferExists = false;
	}

	//assert(!RenderWindow::GLHasError());
}

}// namespace LibPlot2D
