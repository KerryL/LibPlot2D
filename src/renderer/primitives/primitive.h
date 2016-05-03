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

	bool vertexCountModified;
	bool modified;

	RenderWindow &renderWindow;

	virtual bool HasValidParameters() = 0;
	virtual void Update() = 0;
	virtual void GenerateGeometry() = 0;

	void EnableAlphaBlending();
	void DisableAlphaBlending();

	unsigned int vertexShaderIndex;
	unsigned int fragmentShaderIndex;

	virtual void InitializeVertexBuffer() = 0;
	unsigned int vertexCount;

	float *vertices;
	unsigned int vertexBufferIndex;

private:
	unsigned int drawOrder;
};

#endif// PRIMITIVE_H_