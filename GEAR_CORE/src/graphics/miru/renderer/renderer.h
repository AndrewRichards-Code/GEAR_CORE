#pragma once

#include "gear_core_common.h"
#include "mars.h"
#include "graphics/miru/buffer/framebuffer.h"
#include "graphics/miru/buffer/vertexbuffer.h"
#include "graphics/miru/buffer/indexbuffer.h"
#include "graphics/miru/pipeline.h"
#include "objects/object.h"
#include "objects/camera.h"

namespace GEAR {
namespace GRAPHICS {
class Renderer
{
private:
	void* m_Device;

	miru::Ref<miru::crossplatform::CommandPool> m_CmdPool;
	miru::crossplatform::CommandPool::CreateInfo m_CmdPoolCI;
	miru::Ref<miru::crossplatform::CommandBuffer> m_CmdBuffer;
	miru::crossplatform::CommandBuffer::CreateInfo m_CmdBufferCI;

	miru::Ref<miru::crossplatform::CommandPool> m_TransCmdPool;
	miru::crossplatform::CommandPool::CreateInfo m_TransCmdPoolCI;
	miru::Ref<miru::crossplatform::CommandBuffer> m_TransCmdBuffer;
	miru::crossplatform::CommandBuffer::CreateInfo m_TransCmdBufferCI;

	miru::Ref<miru::crossplatform::DescriptorPool> m_DescPool;
	miru::crossplatform::DescriptorPool::CreateInfo m_DescPoolCI;
	miru::Ref<miru::crossplatform::DescriptorSet> m_DescSetCamera;
	std::vector<miru::Ref<miru::crossplatform::DescriptorSet>> m_DescSetObj;
	miru::crossplatform::DescriptorSet::CreateInfo m_DescSetCI;

	miru::Ref<miru::crossplatform::Framebuffer>* m_Framebuffers;
	std::deque<OBJECTS::Object*> m_RenderQueue;
	std::shared_ptr<OBJECTS::Camera> m_Camera;

public:
	Renderer(miru::Ref<miru::crossplatform::Context> context);
	virtual ~Renderer();

	virtual void SubmitFramebuffer(miru::Ref<miru::crossplatform::Framebuffer>* framebuffers) { m_Framebuffers = framebuffers; };
	virtual void SubmitCamera(std::shared_ptr<OBJECTS::Camera> camera) { m_Camera = camera; };
	virtual void Submit(OBJECTS::Object* obj);
	virtual void Flush();

	inline std::deque<OBJECTS::Object*>& GetRenderQueue() { return m_RenderQueue; };
};
}
}