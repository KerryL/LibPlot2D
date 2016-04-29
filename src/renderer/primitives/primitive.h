/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  primitive.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Base class for creating 3D objects.
// History:

#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

// Local headers
#include "renderer/color.h"

// Forward declarations
class RenderWindow;

class Primitive
{
public:
	Primitive(RenderWindow &renderWindow);
	Primitive(const Primitive &primitive);

	virtual ~Primitive();

	// Performs the drawing operations
	void Draw();

	// Called when something is modified to re-create this object with
	// all of the latest information
	virtual void UpdateColor();
	virtual void Update() = 0;
	virtual void GenerateGeometry() = 0;

	// Checks to see if this object's parameters are valid and allow drawing
	virtual bool HasValidParameters() = 0;

	// Private data accessors
	void SetVisibility(const bool &isVisible);
	void SetColor(const Color &color);
	inline Color GetColor() const { return color; }
	void SetDrawOrder(const unsigned int &drawOrder);
	inline void SetModified() { modified = true; }// Forces a re-draw

	inline bool GetIsVisible() const { return isVisible; }
	inline unsigned int GetDrawOrder() const { return drawOrder; }

	// Overloaded operators
	Primitive& operator=(const Primitive &primitive);

protected:
	bool isVisible;

	Color color;

	bool modified;

	RenderWindow &renderWindow;

	void EnableAlphaBlending();
	void DisableAlphaBlending();

	unsigned int vertexShaderIndex;
	unsigned int fragmentShaderIndex;

	virtual void InitializeColorBuffer();
	virtual void InitializeVertexBuffer() = 0;

	GLfloat *vertices;

private:
	unsigned int drawOrder;

	GLuint positionBufferIndex;
	GLuint colorBufferIndex;
	GLfloat colorBuffer[4];
};

#endif// PRIMITIVE_H_