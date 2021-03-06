#pragma once

#include "gear_core_common.h"
#include "Graphics/Texture.h"

namespace gear 
{
namespace graphics 
{
	class Framebuffer
	{
	public:
		struct CreateInfo
		{
			std::string									debugName;
			void*										device;
			uint32_t									width;
			uint32_t									height;
			miru::crossplatform::Image::SampleCountBit	samples;
			Ref<miru::crossplatform::RenderPass>	renderPass;
			bool										cubemap;
		};

	private:
		Ref<miru::crossplatform::Framebuffer> m_Framebuffer;
		miru::crossplatform::Framebuffer::CreateInfo m_FramebufferCI;

		std::array<Ref<Texture>, 8> m_ColourTextures;
		Ref<Texture> m_DepthTexture;

		CreateInfo m_CI;

	public:
		Framebuffer(CreateInfo* pCreateInfo);
		~Framebuffer();

		const CreateInfo& GetCreateInfo() { return m_CI; }

		void UpdateFrameBufferSize(uint32_t width, uint32_t height);

		void AddColourTextureAttachment(size_t attachment = 0);
		void AddDepthTextureAttachment(size_t attachment = 0);

		void FinaliseFramebuffer();

		inline Ref<Texture> GetColourTexture(size_t slot = 0, bool getResolvedFBO = true) const
		{
			return m_ColourTextures[slot]; 
		}
		inline Ref<Texture> GetDepthTexture(bool getResolvedFBO = true) const
		{
			return m_DepthTexture; 
		}
		inline miru::crossplatform::Image::SampleCountBit GetMultisampleValue() const { return m_CI.samples; }
		inline const Ref<miru::crossplatform::Framebuffer>& GetFramebuffer() { return m_Framebuffer; }

	private:
		void CheckColourTextureAttachments(size_t attachment);
	};
}
}