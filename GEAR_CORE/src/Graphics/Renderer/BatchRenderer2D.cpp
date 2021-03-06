#include "batchrenderer2d.h"

using namespace GEAR;
using namespace GRAPHICS;
using namespace OPENGL;
using namespace mars;
using namespace OBJECTS;

BatchRenderer2D::BatchRenderer2D()
{
	Init();
}

BatchRenderer2D::~BatchRenderer2D()
{
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);
}

void BatchRenderer2D::Init()
{
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, GEAR_BATCH_RENDERER_2D_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glEnableVertexAttribArray((GLuint)VertexArray::BufferType::GEAR_BUFFER_POSITIONS);
	glEnableVertexAttribArray((GLuint)VertexArray::BufferType::GEAR_BUFFER_TEXCOORDS);
	glEnableVertexAttribArray((GLuint)VertexArray::BufferType::GEAR_BUFFER_TEXIDS);
	glEnableVertexAttribArray((GLuint)VertexArray::BufferType::GEAR_BUFFER_NORMALS);
	glEnableVertexAttribArray((GLuint)VertexArray::BufferType::GEAR_BUFFER_COLOURS);

	glVertexAttribPointer((GLuint)VertexArray::BufferType::GEAR_BUFFER_POSITIONS, 3, GL_FLOAT, GL_FALSE, GEAR_BATCH_RENDERER_2D_VERTEX_SIZE, (const GLvoid*)offsetof(Object::VertexData, m_Vertex));
	glVertexAttribPointer((GLuint)VertexArray::BufferType::GEAR_BUFFER_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, GEAR_BATCH_RENDERER_2D_VERTEX_SIZE, (const GLvoid*)offsetof(Object::VertexData, m_TexCoord));
	glVertexAttribPointer((GLuint)VertexArray::BufferType::GEAR_BUFFER_TEXIDS, 1, GL_FLOAT, GL_FALSE, GEAR_BATCH_RENDERER_2D_VERTEX_SIZE, (const GLvoid*)offsetof(Object::VertexData, m_TexId));
	glVertexAttribPointer((GLuint)VertexArray::BufferType::GEAR_BUFFER_NORMALS, 3, GL_FLOAT, GL_FALSE, GEAR_BATCH_RENDERER_2D_VERTEX_SIZE, (const GLvoid*)offsetof(Object::VertexData, m_Normal));
	glVertexAttribPointer((GLuint)VertexArray::BufferType::GEAR_BUFFER_COLOURS, 4, GL_FLOAT, GL_FALSE, GEAR_BATCH_RENDERER_2D_VERTEX_SIZE, (const GLvoid*)offsetof(Object::VertexData, m_Colour));
	
	GLuint indicies[GEAR_BATCH_RENDERER_2D_INDICIES_SIZE];
	int offset = 0;
	for (int i = 0; i < GEAR_BATCH_RENDERER_2D_INDICIES_SIZE; i += 6)
	{
		indicies[i + 0] = offset + 0;
		indicies[i + 1] = offset + 1;
		indicies[i + 2] = offset + 2;
		indicies[i + 3] = offset + 2;
		indicies[i + 4] = offset + 3;
		indicies[i + 5] = offset + 0;
		offset += 4;
	}

	m_IBO = std::make_unique<IndexBuffer>(indicies, GEAR_BATCH_RENDERER_2D_INDICIES_SIZE);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BatchRenderer2D::OpenMapBuffer() 
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	m_BatchBuffer = (Object::VertexData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void BatchRenderer2D::CloseMapBuffer() 
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BatchRenderer2D::Submit(Object* obj)
{
	m_Object = obj;
	const char* file = obj->GetObjFileName();
	bool test = obj->b_ForBatchRenderer2D;

	int textureSlot = 0;
	unsigned int textureID = obj->GetTextureID();

	if (test == true && file == "res/obj/quad.obj")
	{
		bool found = false;
		for (int i = 0; i < (int)m_TextureSlots.size(); i++)
		{
			if (m_TextureSlots[i] == textureID)
			{
				textureSlot = i + 1;
				found = true;
				break;
			}
		}
		if (!found)
		{
			m_TextureSlots.push_back(textureID);
			if (m_TextureSlots.size() >= GEAR_BATCH_RENDERER_2D_MAX_TEXTURE_SLOTS)
			{
				CloseMapBuffer();
				Flush();
				OpenMapBuffer();
			}
			textureSlot = static_cast<int>(m_TextureSlots.size());
		}

		Object::VertexData temp[4];

		int j = 0, k = 0, l = 0;
		for (int i = 0; i < 4; i++)
		{
			temp[i].m_Vertex.x = obj->GetVertices()[j + 0];
			temp[i].m_Vertex.y = obj->GetVertices()[j + 1];
			temp[i].m_Vertex.z = obj->GetVertices()[j + 2];

			temp[i].m_TexCoord.x = obj->GetTextCoords()[k + 0];
			temp[i].m_TexCoord.y = obj->GetTextCoords()[k + 1];

			temp[i].m_TexId = static_cast<float>(textureSlot);
			//std::cout << "ObjectPtr V: S, I" << std::endl;
			//std::cout << obj << ", " << i << ": " << textureSlot << ", " << textureID << std::endl << std::endl;
			temp[i].m_Normal.x = obj->GetNormals()[j + 0];
			temp[i].m_Normal.y = obj->GetNormals()[j + 1];
			temp[i].m_Normal.z = obj->GetNormals()[j + 2];

			temp[i].m_Colour.r = obj->GetColours()[l + 0];
			temp[i].m_Colour.g = obj->GetColours()[l + 1];
			temp[i].m_Colour.b = obj->GetColours()[l + 2];
			temp[i].m_Colour.a = obj->GetColours()[l + 3];

			j += 3;
			k += 2;
			l += 4;

			*m_BatchBuffer = temp[i];
			m_BatchBuffer++;
		}

		m_IndexCount += 6;
	}
	else
	{
		std::cout << "ERROR: GEAR::GRAPHICS::OPENGL::BatchRender2D: Submitted object did not use the correct constructor or 'res/obj/quad.obj' as its mesh data!" << std::endl << "BatchRender2D only supports quads!" << std::endl;
	}
}

void BatchRenderer2D::Flush()
{
	for (int i = 0; i < (int)m_TextureSlots.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_TextureSlots[i]);

		if (i > GEAR_BATCH_RENDERER_2D_MAX_TEXTURE_SLOTS - 1)
		{
			m_TextureSlots.erase(m_TextureSlots.begin(), m_TextureSlots.begin() + GEAR_BATCH_RENDERER_2D_MAX_TEXTURE_SLOTS - 1);
			break;
		}
	}

	Object::SetTextureArray(m_Object->GetShader());
	m_Object->SetUniformModlMatrix();
	m_Object->GetShader().Enable();
	glBindVertexArray(m_VAO);
	m_IBO->Bind();
	glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
	m_IBO->Unbind();
	glBindVertexArray(0);
	m_Object->GetShader().Disable();
	
	//DepthRender
	/*for (int i = 0; i < (signed int)m_Lights.size(); i++)
	{
		m_Lights[i]->GetDepthShader().Enable();
		m_Lights[i]->CalculateLightMVP();
		glBindVertexArray(m_VAO);
		m_IBO->Bind();
		glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
		m_IBO->Unbind();
		glBindVertexArray(0);
		m_Lights[i]->GetDepthShader().Disable();
	}*/

	m_IndexCount = 0;
}