// OGLFrameBuffer.hpp
// KlayGE OpenGL渲染到纹理类 头文件
// Ver 3.3.0
// 版权所有(C) 龚敏敏, 2006
// Homepage: http://www.klayge.org
//
// 3.3.0
// 改为FrameBuffer (2006.5.30)
//
// 2.8.0
// 初次建立 (2005.8.1)
//
// 修改记录
/////////////////////////////////////////////////////////////////////////////////

#ifndef _OGLFRAMEBUFFER_HPP
#define _OGLFRAMEBUFFER_HPP

#pragma once

#include <KlayGE/FrameBuffer.hpp>
#include <KlayGE/RenderView.hpp>

namespace KlayGE
{
	class OGLFrameBuffer : public FrameBuffer
	{
	public:
		explicit OGLFrameBuffer(bool off_screen);
		virtual ~OGLFrameBuffer();

		virtual std::wstring const & Description() const;

		void OnBind();

		void Clear(uint32_t flags, Color const & clr, float depth, int32_t stencil);
		virtual void Discard(uint32_t flags) KLAYGE_OVERRIDE;

		GLuint OGLFbo() const
		{
			return fbo_;
		}

	protected:
		GLuint fbo_;
	};

	typedef shared_ptr<OGLFrameBuffer> OGLFrameBufferPtr;
}

#endif			// _OGLFRAMEBUFFER_HPP
