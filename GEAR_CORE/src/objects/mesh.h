#pragma once

#include "gear_core_common.h"
#include "graphics/vertexbuffer.h"
#include "graphics/indexbuffer.h"
#include "utils/fileutils.h"

namespace gear {
namespace objects {

	class Mesh
	{
	public:
		struct CreateInfo
		{
			const char* debugName;
			void*		device;
			std::string filepath;
		};

		enum class VertexBufferContents : size_t
		{
			POSITION,
			TEXTURE_COORD,
			TEXTURE_ID,
			NORMAL,
			COLOUR,
			BI_NORMAL,
			TANGENT,
		};

	private:
		std::string m_DebugName;

		CreateInfo m_CI;
		FileUtils::ObjData m_Data;

		std::map<VertexBufferContents, gear::Ref<graphics::VertexBuffer>> m_VBs;
		gear::Ref<graphics::IndexBuffer> m_IB;

	public:
		Mesh(CreateInfo* pCreateInfo);
		~Mesh();

		void AddVertexBuffer(VertexBufferContents type, gear::Ref<graphics::VertexBuffer> vertexBuffer);
		void RemoveVertexBuffer(VertexBufferContents type);

		inline const std::map<VertexBufferContents, gear::Ref<graphics::VertexBuffer>>& GetVertexBuffers() const { return m_VBs; }
		inline const gear::Ref<graphics::IndexBuffer> GetIndexBuffer() const { return m_IB; }
		inline const FileUtils::ObjData& GetObjData() const { return m_Data; }
	};
}
}
