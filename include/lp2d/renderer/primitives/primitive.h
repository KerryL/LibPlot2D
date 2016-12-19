/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  primitive.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Base class for creating 3D objects.

#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

// Standard C++ headers
#include <vector>
#include <memory>
#include <limits>

// Local headers
#include "lp2d/renderer/color.h"

namespace LibPlot2D
{

// Forward declarations
class RenderWindow;

class Primitive
{
public:
	explicit Primitive(RenderWindow& renderWindow);
	Primitive(const Primitive& primitive);
	Primitive(Primitive&& primitive);

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
	Primitive& operator=(const Primitive& primitive);
	Primitive& operator=(Primitive&& primitive);

	struct BufferInfo
	{
		BufferInfo() = default;
		BufferInfo(const BufferInfo&) = delete;
		BufferInfo(BufferInfo&& b);
		~BufferInfo();

		BufferInfo& operator=(const BufferInfo&) = delete;
		BufferInfo& operator=(BufferInfo&& b);

		unsigned int vertexCount = 0;
		std::vector<float> vertexBuffer;
		std::vector<unsigned int> indexBuffer;
		bool vertexCountModified = true;

		void GetOpenGLIndices(const bool& needIndexObject = false);
		void FreeOpenGLObjects();

		const unsigned int& GetVertexBufferIndex() const { return vertexBufferIndex; }
		const unsigned int& GetVertexArrayIndex() const { return vertexArrayIndex; }
		const unsigned int& GetIndexBufferIndex() const { return indexBufferIndex; }

	private:
		bool glVertexBufferExists = false;
		bool glIndexBufferExists = false;

		unsigned int vertexBufferIndex = std::numeric_limits<unsigned int>::max();
		unsigned int vertexArrayIndex = std::numeric_limits<unsigned int>::max();
		unsigned int indexBufferIndex = std::numeric_limits<unsigned int>::max();
	};

protected:
	bool isVisible;

	Color color;

	bool modified;

	RenderWindow &renderWindow;

	virtual bool HasValidParameters() = 0;
	virtual void Update(const unsigned int& i) = 0;
	virtual void GenerateGeometry() = 0;

	void EnableAlphaBlending();
	void DisableAlphaBlending();

	std::vector<BufferInfo> bufferInfo;

private:
	unsigned int drawOrder;
};

}// namespace LibPlot2D

#endif// PRIMITIVE_H_
