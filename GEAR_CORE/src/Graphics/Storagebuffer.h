#pragma once

#include "gear_core_common.h"

namespace gear 
{
namespace graphics 
{
	template<typename T>
	class Storagebuffer : public T
	{
	public:
		struct CreateInfo
		{
			std::string debugName;
			void*		device;
			void*		data;
		};

	private:
		Ref<miru::crossplatform::Buffer> m_ShaderStorageBuffer, m_ShaderStorageBufferUpload;
		miru::crossplatform::Buffer::CreateInfo m_ShaderStorageBufferCI, m_ShaderStorageBufferUploadCI;

		Ref<miru::crossplatform::BufferView> m_ShaderStorageBufferView;
		miru::crossplatform::BufferView::CreateInfo m_ShaderStorageBufferViewCI;

		CreateInfo m_CI;

	public:
		Storagebuffer(CreateInfo* pCreateInfo) 
		{
			m_CI = *pCreateInfo;

			m_ShaderStorageBufferUploadCI.debugName = "GEAR_CORE_ShaderStorageBufferUpload: " + m_CI.debugName;
			m_ShaderStorageBufferUploadCI.device = m_CI.device;
			m_ShaderStorageBufferUploadCI.usage = miru::crossplatform::Buffer::UsageBit::TRANSFER_SRC_BIT | miru::crossplatform::Buffer::UsageBit::TRANSFER_DST_BIT;
			m_ShaderStorageBufferUploadCI.size = GetSize();
			m_ShaderStorageBufferUploadCI.data = m_CI.data;
			m_ShaderStorageBufferUploadCI.pAllocator = AllocatorManager::GetAllocator(AllocatorManager::AllocatorType::CPU);
			m_ShaderStorageBufferUpload = miru::crossplatform::Buffer::Create(&m_ShaderStorageBufferUploadCI);

			m_ShaderStorageBufferCI.debugName = "GEAR_CORE_ShaderStorageBuffer: " + m_CI.debugName;
			m_ShaderStorageBufferCI.device = m_CI.device;
			m_ShaderStorageBufferCI.usage = miru::crossplatform::Buffer::UsageBit::TRANSFER_SRC_BIT | miru::crossplatform::Buffer::UsageBit::TRANSFER_DST_BIT | miru::crossplatform::Buffer::UsageBit::STORAGE_BIT;
			m_ShaderStorageBufferCI.size = GetSize();
			m_ShaderStorageBufferCI.data = nullptr;
			m_ShaderStorageBufferCI.pAllocator = AllocatorManager::GetAllocator(AllocatorManager::AllocatorType::GPU);
			m_ShaderStorageBuffer = miru::crossplatform::Buffer::Create(&m_ShaderStorageBufferCI);

			m_ShaderStorageBufferViewCI.debugName = "GEAR_CORE_ShaderStorageViewUsage: " + m_CI.debugName;
			m_ShaderStorageBufferViewCI.device = m_CI.device;
			m_ShaderStorageBufferViewCI.type = miru::crossplatform::BufferView::Type::UNIFORM;
			m_ShaderStorageBufferViewCI.pBuffer = m_ShaderStorageBuffer;
			m_ShaderStorageBufferViewCI.offset = 0;
			m_ShaderStorageBufferViewCI.size = GetSize();
			m_ShaderStorageBufferViewCI.stride = 0;
			m_ShaderStorageBufferView = miru::crossplatform::BufferView::Create(&m_ShaderStorageBufferViewCI);
		}
		~Storagebuffer() {}

		const CreateInfo& GetCreateInfo() { return m_CI; }

		void SubmitData(const void* data, size_t  size) const
		{
			m_ShaderStorageBufferUploadCI.pAllocator->SubmitData(m_ShaderStorageBufferUpload->GetAllocation(), (size_t)size, (void*)data);
		}
		void AccessData(void* data, size_t size) const 
		{
			m_ShaderStorageBufferUploadCI.pAllocator->AccessData(m_ShaderStorageBufferUpload->GetAllocation(), size, data);
		}
		void Upload(const Ref<miru::crossplatform::CommandBuffer>& cmdBuffer, uint32_t cmdBufferIndex = 0)
		{
			cmdBuffer->CopyBuffer(cmdBufferIndex, m_ShaderStorageBufferUpload, m_ShaderStorageBuffer, { {0, 0, GetSize()} });
		}
		void Download(const Ref<miru::crossplatform::CommandBuffer>& cmdBuffer, uint32_t cmdBufferIndex = 0)
		{
			cmdBuffer->CopyBuffer(cmdBufferIndex, m_ShaderStorageBuffer, m_ShaderStorageBufferUpload, { {0, 0, GetSize()} });
		}

		inline const Ref<miru::crossplatform::Buffer>& GetBuffer() const { return m_ShaderStorageBuffer; };
		inline const Ref<miru::crossplatform::BufferView>& GetBufferView() const { return m_ShaderStorageBufferView; };

		inline size_t GetSize() { return sizeof(T); }
	};
}
}