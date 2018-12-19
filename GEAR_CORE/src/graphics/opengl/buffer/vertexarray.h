#pragma once

#include <vector>
#include <memory>
#include "GL/glew.h"
#include "vertexbuffer.h"

#define GEAR_BUFFER_POSITIONS 0
#define GEAR_BUFFER_TEXTCOORDS 1
#define GEAR_BUFFER_TEXTIDS 2
#define GEAR_BUFFER_NORMALS 3
#define GEAR_BUFFER_COLOUR 4

namespace GEAR {
namespace GRAPHICS {
namespace OPENGL {
	class VertexArray
	{
	private:
		unsigned int m_VertexArrayID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffer;

	public:
		VertexArray();
		~VertexArray();
		void Bind() const;
		void Unbind() const;

		void AddBuffer(std::shared_ptr<VertexBuffer> vbo, GLuint index);
	};
}
}
}