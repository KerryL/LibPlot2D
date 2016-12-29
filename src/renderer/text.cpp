/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  text.cpp
// Date:  5/12/2016
// Auth:  K. Loux
// Desc:  Helper object for building BufferInfo objects for text rendering.

// Standard C++ headers
#include <cassert>
#include <algorithm>

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "lp2d/renderer/text.h"
#include "lp2d/renderer/renderWindow.h"

// Freetype headers
#include FT_MODULE_H

namespace LibPlot2D
{

//=============================================================================
// Class:			Text
// Function:		Constant declarations
//
// Description:		Constant declarations for Text class.
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
unsigned int Text::program;
unsigned int Text::colorLocation;
unsigned int Text::vertexLocation;
unsigned int Text::indexLocation;
unsigned int Text::modelviewMatrixLocation;
bool Text::initialized;
FT_Library Text::ft;
unsigned int Text::ftReferenceCount(0);

//=============================================================================
// Class:			Text
// Function:		vertexShader
//
// Description:		Text vertex shader.
//
// Input Arguments:
//		0	= vertex
//		1	= texIndex
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
const std::string Text::vertexShader(
	"#version 300 es\n"
	"\n"
	"uniform mat4 projectionMatrix;\n"
	"uniform mat4 modelviewMatrix;\n"
	"\n"
	"layout(location = 0) in highp vec4 vertex;// <vec2 pos, vec2 tex>\n"
	"layout(location = 1) in uint texIndex;\n"
	"\n"
	"out highp vec2 texCoords;\n"
	"flat out uint index;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_Position = projectionMatrix * modelviewMatrix * vec4(vertex.xy, 0.0, 1.0);\n"
	"    texCoords = vertex.zw;\n"
	"    index = texIndex;\n"
	"}\n"
);

//=============================================================================
// Class:			Text
// Function:		fragmentShader
//
// Description:		Text fragment shader.
//
// Input Arguments:
//		0	= texCoords
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
const std::string Text::fragmentShader(
	"#version 300 es\n"
	"\n"
	"uniform highp sampler2DArray text;\n"
	"uniform highp vec3 textColor;\n"
	"\n"
	"in highp vec2 texCoords;\n"
	"flat in uint index;\n"
	"\n"
	"out highp vec4 color;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    highp vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, vec3(texCoords, index)).r);\n"
	"    color = vec4(textColor, 1.0) * sampled;\n"
	"}\n"
);

//=============================================================================
// Class:			Text
// Function:		Text
//
// Description:		Constructor for Text object.
//
// Input Arguments:
//		renderer	= RenderWindow&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
Text::Text(RenderWindow& renderer) : renderer(renderer)
{
	color = Color::ColorBlack;
	scale = 1.0;
	initialized = false;
	glyphsGenerated = false;
	face = nullptr;
	isOK = true;

	modelview.Resize(4, 4);
	SetOrientation(0.0);

	Initialize();
}

//=============================================================================
// Class:			Text
// Function:		~Text
//
// Description:		Destructor for Text object.
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
Text::~Text()
{
	FreeFTResources();

	if (glIsTexture(textureId))
		glDeleteTextures(1, &textureId);
}

//=============================================================================
// Class:			Text
// Function:		Initialize
//
// Description:		Initializes the Freetype back-end of this object.  Must
//					be called prior to any other post-creation method.
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
void Text::Initialize()
{
	assert(!glyphsGenerated);
	assert(isOK);

	if (ftReferenceCount == 0)
	{
		if (FT_Init_FreeType(&ft))
			isOK = false;
	}

	++ftReferenceCount;
}

//=============================================================================
// Class:			Text
// Function:		SetFace
//
// Description:		Initializes the Freetype back-end of this object.  Must
//					be called prior to any other post-creation method.
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
bool Text::SetFace(const std::string& fontFileName)
{
	assert(!glyphsGenerated);
	if (FT_New_Face(ft, fontFileName.c_str(), 0, &face))
		return false;

	return true;
}

//=============================================================================
// Class:			Text
// Function:		SetSize
//
// Description:		Sets the width and height to use for the font.  Width is
//					calculated to maintain proper aspect ratio.
//
// Input Arguments:
//		height	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Text::SetSize(const double& height)
{
	SetSize(0, height);
}

//=============================================================================
// Class:			Text
// Function:		SetSize
//
// Description:		Sets the width and height to use for the font.
//
// Input Arguments:
//		width	= const double&
//		height	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Text::SetSize(const double& width, const double& height)
{
	assert(!glyphsGenerated);
	FT_Set_Pixel_Sizes(face, width, height);
}

//=============================================================================
// Class:			Text
// Function:		GenerateGlyphs
//
// Description:		Generates the set of glyphs.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
bool Text::GenerateGlyphs()
{
	assert(!RenderWindow::GLHasError());
	assert(!glyphsGenerated);// Doing this twice could leak memory via OpenGL

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	Glyph g;
	GLubyte c;

	maxXSize = 0;
	maxYSize = 0;

	// First loop determines max required image size
	const unsigned int glyphCount(128);
	for (c = 0; c < glyphCount; ++c)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			return false;

		maxXSize = std::max(maxXSize, face->glyph->bitmap.width);
		maxYSize = std::max(maxYSize, face->glyph->bitmap.rows);
	}

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, maxXSize, maxYSize, glyphCount,
		0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);

	// Second loop actually builds and stores the textures
	for (c = 0; c < glyphCount; ++c)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			return false;

		int w, h, d;
		glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &w);
		glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &h);
		glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &d);

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, c,
			face->glyph->bitmap.width, face->glyph->bitmap.rows, 1,
			GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);// TODO:  Memory leak here

		g.index = c;
		g.xSize = face->glyph->bitmap.width;
		g.ySize = face->glyph->bitmap.rows;
		g.xBearing = face->glyph->bitmap_left;
		g.yBearing = face->glyph->bitmap_top;
		g.advance = face->glyph->advance.x;

		glyphs.insert(std::make_pair(c, g));
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glyphsGenerated = true;

	assert(!RenderWindow::GLHasError());
	return true;
}

//=============================================================================
// Class:			Text
// Function:		FreeFTResources
//
// Description:		Frees previously allocated Freetype resources (with
//					reference counting).
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
void Text::FreeFTResources()
{
	FT_Done_Face(face);

	assert(ftReferenceCount > 0);
	--ftReferenceCount;
	if (ftReferenceCount == 0)
		FT_Done_FreeType(ft);
}

//=============================================================================
// Class:			Text
// Function:		BuildText
//
// Description:		Generates the vertex buffer information for this object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive::BufferInfo
//
//=============================================================================
Primitive::BufferInfo Text::BuildText()
{
	if (bufferVector.size() > 0)
		return AssembleBuffers();

	Primitive::BufferInfo bufferInfo(BuildLocalText());
	ConfigureVertexArray(bufferInfo);

	return bufferInfo;
}

//=============================================================================
// Class:			Text
// Function:		BuildText
//
// Description:		Generates the vertex buffer information for this object.
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
void Text::RenderBufferedGlyph(const unsigned int& vertexCount)
{
	assert(vertexCount > 0);

	glUseProgram(program);

	// TODO:  Really, we don't want to access state here that isn't contained within BufferInfo
	// Are we making an exception for color and orientation?  Assume that this object will
	// be used only to render text having the same color and orientation (and size?).
	// Maybe put some assertions in the Set() methods then to ensure it hasn't yet been initialized?
	glUniform3f(colorLocation, color.GetRed(), color.GetGreen(), color.GetBlue());
	RenderWindow::SendUniformMatrix(modelview, modelviewMatrixLocation);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	renderer.UseDefaultProgram();
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
// Class:			Text
// Function:		GetBoundingBox
//
// Description:		Returns the bounding box for the specified text string
//					given this object's font and size settings.
//
// Input Arguments:
//		s	= const std::string&
//
// Output Arguments:
//		None
//
// Return Value:
//		Bounding Box
//
//=============================================================================
Text::BoundingBox Text::GetBoundingBox(const std::string& s)
{
	DoInternalInitialization();

	BoundingBox b;
	b.xLeft = 0;
	b.xRight = 0;
	b.yUp = 0;
	b.yDown = 0;

	for (const auto& c : s)
	{
		Glyph g = glyphs.find(c)->second;

		//b.xLeft += 0;
		b.xRight += g.advance >> 6;
		b.yUp = std::max(b.yUp, g.yBearing);
		b.yDown = std::min(b.yDown, g.yBearing - g.ySize);
	}

	b.xLeft *= scale;
	b.xRight *= scale;
	b.yUp *= scale;
	b.yDown *= scale;

	return b;
}

//=============================================================================
// Class:			Text
// Function:		DoInternalInitialization
//
// Description:		Performs necessary static-state initialization.
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
void Text::DoInternalInitialization()
{
	if (!glyphsGenerated)
	{
		if (!GenerateGlyphs())
		{
			isOK = false;
			return;
		}
	}

	if (!initialized)
	{
		std::vector<GLuint> shaderList;
		shaderList.push_back(renderer.CreateShader(GL_VERTEX_SHADER, vertexShader));
		shaderList.push_back(renderer.CreateShader(GL_FRAGMENT_SHADER, fragmentShader));

		program = renderer.CreateProgram(shaderList);
		colorLocation = glGetUniformLocation(program, "textColor");

		RenderWindow::ShaderInfo s;
		s.programId = program;
		s.needsModelview = false;
		s.needsProjection = true;
		s.projectionLocation = glGetUniformLocation(program, "projectionMatrix");
		modelviewMatrixLocation = glGetUniformLocation(program, "modelviewMatrix");
		renderer.AddShader(s);

		vertexLocation = glGetAttribLocation(program, "vertex");
		indexLocation = glGetAttribLocation(program, "texIndex");

		initialized = true;
	}
}

//=============================================================================
// Class:			Text
// Function:		SetOrientation
//
// Description:		Alters the modelview matrix according to the text rotation angle.
//
// Input Arguments:
//		angle	= const double& [rad]
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Text::SetOrientation(const double& angle)
{
	modelview.MakeIdentity();
	renderer.Rotate(modelview, angle, 0.0, 0.0, 1.0);
}

//=============================================================================
// Class:			Text
// Function:		AppendText
//
// Description:		Appends text to the render buffer.  Use this instead of
//					SetText() when multiple text values must be rendered with
//					different positions, scales, etc.  Text will be rendered
//					with current settings (i.e. this must be the last call
//					for this string in the update loop).
//
// Input Arguments:
//		text	= const std::string&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Text::AppendText(const std::string& text)
{
	SetText(text);
	bufferVector.push_back(BuildLocalText());
}

//=============================================================================
// Class:			Text
// Function:		AssembleBuffers
//
// Description:		Assembles
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
Primitive::BufferInfo Text::AssembleBuffers()
{
	Primitive::BufferInfo bufferInfo;

	for (const auto& buffer : bufferVector)
	{
		bufferInfo.indexBuffer.insert(bufferInfo.indexBuffer.end(),
			buffer.indexBuffer.begin(),
			buffer.indexBuffer.end());
		bufferInfo.vertexBuffer.insert(bufferInfo.vertexBuffer.end(),
			buffer.vertexBuffer.begin(),
			buffer.vertexBuffer.end());
		bufferInfo.vertexCount += buffer.vertexCount;
	}

	ConfigureVertexArray(bufferInfo);
	bufferVector.clear();

	return bufferInfo;
}

//=============================================================================
// Class:			Text
// Function:		BuildLocalText
//
// Description:		Allocates buffers and populates local (CPU-side) memory
//					to render text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive::BufferInfo
//
//=============================================================================
Primitive::BufferInfo Text::BuildLocalText()
{
	DoInternalInitialization();

	assert(sizeof(GLfloat) == sizeof(float));
	assert(sizeof(GLuint) == sizeof(unsigned int));

	Primitive::BufferInfo bufferInfo;
	bufferInfo.vertexCount = 6 * text.length();
	bufferInfo.vertexBuffer.resize(bufferInfo.vertexCount * 4);
	bufferInfo.indexBuffer.resize(bufferInfo.vertexCount);

	double xStart(x);

	unsigned int i(0), texI(0);
	for (const auto &c : text)
	{
		Glyph g = glyphs[c];

		GLfloat xpos = xStart + g.xBearing * scale;
		GLfloat ypos = y - (g.ySize - g.yBearing) * scale;

		GLfloat w = g.xSize * scale;
		GLfloat h = g.ySize * scale;

		bufferInfo.indexBuffer[texI++] = g.index;
		bufferInfo.indexBuffer[texI++] = g.index;
		bufferInfo.indexBuffer[texI++] = g.index;
		bufferInfo.indexBuffer[texI++] = g.index;
		bufferInfo.indexBuffer[texI++] = g.index;
		bufferInfo.indexBuffer[texI++] = g.index;

		bufferInfo.vertexBuffer[i++] = xpos;
		bufferInfo.vertexBuffer[i++] = ypos;
		bufferInfo.vertexBuffer[i++] = 0.0;
		bufferInfo.vertexBuffer[i++] = float(g.ySize) / float(maxYSize);

		bufferInfo.vertexBuffer[i++] = xpos;
		bufferInfo.vertexBuffer[i++] = ypos + h;
		bufferInfo.vertexBuffer[i++] = 0.0;
		bufferInfo.vertexBuffer[i++] = 0.0;

		bufferInfo.vertexBuffer[i++] = xpos + w;
		bufferInfo.vertexBuffer[i++] = ypos + h;
		bufferInfo.vertexBuffer[i++] = float(g.xSize) / float(maxXSize);
		bufferInfo.vertexBuffer[i++] = 0.0;

		bufferInfo.vertexBuffer[i++] = xpos + w;
		bufferInfo.vertexBuffer[i++] = ypos + h;
		bufferInfo.vertexBuffer[i++] = float(g.xSize) / float(maxXSize);
		bufferInfo.vertexBuffer[i++] = 0.0;

		bufferInfo.vertexBuffer[i++] = xpos + w;
		bufferInfo.vertexBuffer[i++] = ypos;
		bufferInfo.vertexBuffer[i++] = float(g.xSize) / float(maxXSize);
		bufferInfo.vertexBuffer[i++] = float(g.ySize) / float(maxYSize);

		bufferInfo.vertexBuffer[i++] = xpos;
		bufferInfo.vertexBuffer[i++] = ypos;
		bufferInfo.vertexBuffer[i++] = 0.0;
		bufferInfo.vertexBuffer[i++] = float(g.ySize) / float(maxYSize);

		xStart += (g.advance >> 6) * scale;// Bitshift by 6 to get value in pixels (2^6 = 64)
    }

	return bufferInfo;
}

//=============================================================================
// Class:			Text
// Function:		ConfigureVertexArray
//
// Description:		Handles configuration of OpenGL vertex array object.
//
// Input Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Output Arguments:
//		None
//
// Return Value:
//		Primitive::BufferInfo
//
//=============================================================================
void Text::ConfigureVertexArray(Primitive::BufferInfo& bufferInfo) const
{
	bufferInfo.GetOpenGLIndices(true);
	glBindVertexArray(bufferInfo.GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.GetVertexBufferIndex());
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * bufferInfo.vertexCount,
		bufferInfo.vertexBuffer.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.GetIndexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * bufferInfo.indexBuffer.size(),
		bufferInfo.indexBuffer.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(indexLocation);
	glVertexAttribIPointer(indexLocation, 1, GL_UNSIGNED_INT, 0, 0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

}// namespace LibPlot2D
