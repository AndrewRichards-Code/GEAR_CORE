#pragma once

#include "gear_core_common.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/RenderPipeline.h"
#include "Objects/Camera.h"
#include "Objects/Light.h"
#include "Objects/Skybox.h"
#include "Objects/Model.h"

namespace gear 
{
namespace graphics 
{
	class Renderer
	{
	private:
		//Context and Device
		void* m_Device;
		Ref<miru::crossplatform::Context> m_Context;

		//Cmd Pools and CmdBuffers
		Ref<miru::crossplatform::CommandPool> m_CmdPool;
		miru::crossplatform::CommandPool::CreateInfo m_CmdPoolCI;
		Ref<miru::crossplatform::CommandBuffer> m_CmdBuffer;
		miru::crossplatform::CommandBuffer::CreateInfo m_CmdBufferCI;

		Ref<miru::crossplatform::CommandPool> m_TransCmdPool;
		miru::crossplatform::CommandPool::CreateInfo m_TransCmdPoolCI;
		Ref<miru::crossplatform::CommandBuffer> m_TransCmdBuffer;
		miru::crossplatform::CommandBuffer::CreateInfo m_TransCmdBufferCI;

		//Descriptor Pool and Sets
		Ref<miru::crossplatform::DescriptorPool> m_DescPool;
		miru::crossplatform::DescriptorPool::CreateInfo m_DescPoolCI;

		std::map<Ref<graphics::RenderPipeline>, Ref<miru::crossplatform::DescriptorSet>> m_DescSetPerView;
		std::map<Ref<objects::Model>, Ref<miru::crossplatform::DescriptorSet>> m_DescSetPerModel;
		std::map<Ref<objects::Material>, Ref<miru::crossplatform::DescriptorSet>> m_DescSetPerMaterial;

		bool m_BuiltDescPoolsAndSets = false;
		bool m_ReloadTextures = false;

		//Renderering Objects
		std::map<std::string, Ref<graphics::RenderPipeline>> m_RenderPipelines;
		const Ref<miru::crossplatform::Framebuffer>* m_Framebuffers;
		Ref<objects::Camera> m_Camera;
		Ref<objects::Camera> m_FontCamera;
		std::vector<Ref<objects::Light>> m_Lights;
		Ref<objects::Skybox> m_Skybox;
		std::vector<Ref<objects::Model>> m_RenderQueue;

		//Present Synchronisation Primitives
		std::vector<Ref<miru::crossplatform::Fence>> m_DrawFences;
		miru::crossplatform::Fence::CreateInfo m_DrawFenceCI;
		std::vector<Ref<miru::crossplatform::Semaphore>>m_AcquireSemaphores;
		miru::crossplatform::Semaphore::CreateInfo m_AcquireSemaphoreCI;
		std::vector<Ref<miru::crossplatform::Semaphore>>m_SubmitSemaphores;
		miru::crossplatform::Semaphore::CreateInfo m_SubmitSemaphoreCI;

		uint32_t m_FrameIndex = 0;
		uint32_t m_FrameCount = 0;

	public:
		Renderer(const Ref<miru::crossplatform::Context>& context);
		virtual ~Renderer();

		void InitialiseRenderPipelines(const std::vector<std::string>& filepaths, float viewportWidth, float viewportHeight, miru::crossplatform::Image::SampleCountBit samples, const Ref<miru::crossplatform::RenderPass>& renderPass);
		
		void SubmitFramebuffer(const Ref<miru::crossplatform::Framebuffer>* framebuffers);
		void SubmitCamera(const Ref<objects::Camera>& camera);
		void SubmitFontCamera(const Ref<objects::Camera>& fontCamera);
		void SubmitLights(const std::vector<Ref<objects::Light>>& lights);
		void SubmitSkybox(const Ref<objects::Skybox>& skybox);
		void SubmitModel(const Ref<objects::Model>& obj);

		void Upload(bool forceUploadCamera, bool forceUploadLights, bool forceUploadSkybox, bool forceUploadMeshes);
		void Flush();
		void Present(const Ref<miru::crossplatform::Swapchain>& swapchain, bool& windowResize);

		void DrawCoordinateAxes();

		void ResizeRenderPipelineViewports(uint32_t width, uint32_t height);
		void RecompileRenderPipelineShaders();
		void ReloadTextures();

		inline std::vector<Ref<objects::Model>>& GetRenderQueue() { return m_RenderQueue; };
		inline const Ref<miru::crossplatform::CommandBuffer>& GetCmdBuffer() { return m_CmdBuffer; };
		inline const std::map<std::string, Ref<graphics::RenderPipeline>>& GetRenderPipelines() const { return m_RenderPipelines; }

		inline const uint32_t& GetFrameIndex() const { return m_FrameIndex; }
		inline const uint32_t& GetFrameCount() const { return m_FrameCount; }
	};
}
}