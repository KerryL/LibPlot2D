/*=============================================================================
                                   LibPlot2D
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
	/// Constructor.
	///
	/// \param renderWindow The window that owns this primitive.
	explicit Primitive(RenderWindow& renderWindow);

	/// Copy constructor.
	///
	/// \param primitive Object to copy.
	explicit Primitive(const Primitive& primitive);

	/// Move constructor.
	///
	/// \param primitive Object to absorb.
	explicit Primitive(Primitive&& primitive);

	virtual ~Primitive();

	/// Performs the drawing operations.
	void Draw();

	// Private data accessors
	void SetVisibility(const bool &isVisible);
	void SetColor(const Color &color);
	inline Color GetColor() const { return mColor; }
	void SetDrawOrder(const unsigned int &drawOrder);
	inline void SetModified() { mModified = true; }// Forces a re-draw

	inline bool GetIsVisible() const { return mIsVisible; }
	inline unsigned int GetDrawOrder() const { return mDrawOrder; }

	// Overloaded operators
	Primitive& operator=(const Primitive& primitive);
	Primitive& operator=(Primitive&& primitive);

	/// Structure containing information required to render a vertex buffer or
	/// vertex array.
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
	bool mIsVisible = true;///< Flag indicating whether or not to draw this.

	Color mColor = Color::ColorBlack;///< Color of this object.

	bool mModified = true;///< Indicates whether or not this is up-to-date.

	RenderWindow &mRenderWindow;///< The window that owns this object.

	/// Checks to determine if this object has enough information to be
	/// rendered.
	/// \returns True if this object can be rendered with its current
	///          parameters.
	virtual bool HasValidParameters() = 0;

	/// Updates the specified internal buffer required to render this object.
	///
	/// \param i Index of the buffer to update.
	virtual void Update(const unsigned int& i) = 0;

	/// Performs the actions necessary to render this object.
	virtual void GenerateGeometry() = 0;

	/// Enables alpha blending.
	void EnableAlphaBlending();

	/// Disables alpha blending.
	void DisableAlphaBlending();

	/// List of buffer information associated with this object.
	std::vector<BufferInfo> mBufferInfo;

private:
	unsigned int mDrawOrder = 1000;
};

}// namespace LibPlot2D

#endif// PRIMITIVE_H_
