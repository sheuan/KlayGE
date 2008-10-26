// D3D10Texture1D.cpp
// KlayGE D3D10 1D������ ʵ���ļ�
// Ver 3.8.0
// ��Ȩ����(C) ������, 2008
// Homepage: http://klayge.sourceforge.net
//
// 3.8.0
// ���ν��� (2008.9.21)
//
// �޸ļ�¼
/////////////////////////////////////////////////////////////////////////////////

#include <KlayGE/KlayGE.hpp>
#include <KlayGE/Util.hpp>
#include <KlayGE/COMPtr.hpp>
#include <KlayGE/ThrowErr.hpp>
#include <KlayGE/Math.hpp>
#include <KlayGE/Context.hpp>
#include <KlayGE/RenderEngine.hpp>
#include <KlayGE/RenderFactory.hpp>
#include <KlayGE/Math.hpp>
#include <KlayGE/Texture.hpp>

#include <cstring>

#include <KlayGE/D3D10/D3D10MinGWDefs.hpp>
#include <d3d10.h>
#include <d3dx10.h>

#include <KlayGE/D3D10/D3D10Typedefs.hpp>
#include <KlayGE/D3D10/D3D10RenderEngine.hpp>
#include <KlayGE/D3D10/D3D10Mapping.hpp>
#include <KlayGE/D3D10/D3D10Texture.hpp>

#ifdef KLAYGE_COMPILER_MSVC
#ifdef KLAYGE_DEBUG
	#pragma comment(lib, "d3dx10d.lib")
#else
	#pragma comment(lib, "d3dx10.lib")
#endif
#endif

namespace KlayGE
{
	D3D10Texture1D::D3D10Texture1D(uint32_t width, uint16_t numMipMaps, ElementFormat format, uint32_t access_hint, ElementInitData* init_data)
					: D3D10Texture(TT_1D, access_hint)
	{
		D3D10RenderEngine& renderEngine(*checked_cast<D3D10RenderEngine*>(&Context::Instance().RenderFactoryInstance().RenderEngineInstance()));
		d3d_device_ = renderEngine.D3DDevice();

		numMipMaps_ = numMipMaps;
		format_		= format;
		widthes_.assign(1, width);

		bpp_ = NumFormatBits(format);

		desc_.Width = width;
		desc_.MipLevels = numMipMaps_;
		desc_.ArraySize = 1;
		desc_.Format = D3D10Mapping::MappingFormat(format_);

		this->GetD3DFlags(desc_.Usage, desc_.BindFlags, desc_.CPUAccessFlags, desc_.MiscFlags);

		D3D10_SUBRESOURCE_DATA subres_data;
		if (init_data != NULL)
		{
			subres_data.pSysMem = &init_data->data[0];
			subres_data.SysMemPitch = init_data->row_pitch;
			subres_data.SysMemSlicePitch = init_data->slice_pitch;
		}

		ID3D10Texture1D* d3d_tex;
		TIF(d3d_device_->CreateTexture1D(&desc_, (init_data != NULL) ? &subres_data : NULL, &d3d_tex));
		d3dTexture1D_ = MakeCOMPtr(d3d_tex);

		if (access_hint_ & EAH_GPU_Read)
		{
			ID3D10ShaderResourceView* d3d_sr_view;
			d3d_device_->CreateShaderResourceView(d3dTexture1D_.get(), NULL, &d3d_sr_view);
			d3d_sr_view_ = MakeCOMPtr(d3d_sr_view);
		}

		this->UpdateParams();
	}

	uint32_t D3D10Texture1D::Width(int level) const
	{
		BOOST_ASSERT(level < numMipMaps_);

		return widthes_[level];
	}

	void D3D10Texture1D::CopyToTexture(Texture& target)
	{
		BOOST_ASSERT(type_ == target.Type());

		D3D10Texture1D& other(*checked_cast<D3D10Texture1D*>(&target));

		if ((this->Width(0) == target.Width(0)) && (this->Format() == target.Format()) && (this->NumMipMaps() == target.NumMipMaps()))
		{
			d3d_device_->CopyResource(other.D3DTexture().get(), d3dTexture1D_.get());
		}
		else
		{
			D3DX10_TEXTURE_LOAD_INFO info;
			info.pSrcBox = NULL;
			info.pDstBox = NULL;
			info.SrcFirstMip = D3D10CalcSubresource(0, 0, 1);
			info.DstFirstMip = D3D10CalcSubresource(0, 0, 1);
			info.NumMips = std::min(this->NumMipMaps(), target.NumMipMaps());
			info.SrcFirstElement = 0;
			info.DstFirstElement = 0;
			info.NumElements = 0;
			info.Filter = D3DX10_FILTER_LINEAR;
			info.MipFilter = D3DX10_FILTER_LINEAR;
			if (IsSRGB(format_))
			{
				info.Filter |= D3DX10_FILTER_SRGB_IN;
				info.MipFilter |= D3DX10_FILTER_SRGB_IN;
			}
			if (IsSRGB(target.Format()))
			{
				info.Filter |= D3DX10_FILTER_SRGB_OUT;
				info.MipFilter |= D3DX10_FILTER_SRGB_OUT;
			}

			D3DX10LoadTextureFromTexture(d3dTexture1D_.get(), &info, other.D3DTexture().get());
		}
	}

	void D3D10Texture1D::CopyToTexture1D(Texture& target, int level,
			uint32_t dst_width, uint32_t dst_xOffset, uint32_t src_width, uint32_t src_xOffset)
	{
		BOOST_ASSERT(type_ == target.Type());

		D3D10Texture1D& other(*checked_cast<D3D10Texture1D*>(&target));

		if ((src_width == dst_width) && (this->Format() == target.Format()))
		{
			D3D10_BOX src_box;
			src_box.left = src_xOffset;
			src_box.top = 0;
			src_box.front = 0;
			src_box.right = src_xOffset + src_width;
			src_box.bottom = 1;
			src_box.back = 1;

			d3d_device_->CopySubresourceRegion(other.D3DTexture().get(), D3D10CalcSubresource(level, 0, 1),
				dst_xOffset, 0, 0, d3dTexture1D_.get(), D3D10CalcSubresource(level, 0, 1), &src_box);
		}
		else
		{
			D3D10_BOX src_box, dst_box;

			src_box.left = src_xOffset;
			src_box.top = 0;
			src_box.front = 0;
			src_box.right = src_xOffset + src_width;
			src_box.bottom = 1;
			src_box.back = 1;

			dst_box.left = dst_xOffset;
			dst_box.top = 0;
			dst_box.front = 0;
			dst_box.right = dst_xOffset + dst_width;
			dst_box.bottom = 1;
			dst_box.back = 1;

			D3DX10_TEXTURE_LOAD_INFO info;
			info.pSrcBox = &src_box;
			info.pDstBox = &dst_box;
			info.SrcFirstMip = D3D10CalcSubresource(level, 0, 1);
			info.DstFirstMip = D3D10CalcSubresource(level, 0, 1);
			info.NumMips = 1;
			info.SrcFirstElement = 0;
			info.DstFirstElement = 0;
			info.NumElements = 0;
			info.Filter = D3DX10_FILTER_LINEAR;
			info.MipFilter = D3DX10_FILTER_LINEAR;
			if (IsSRGB(format_))
			{
				info.Filter |= D3DX10_FILTER_SRGB_IN;
				info.MipFilter |= D3DX10_FILTER_SRGB_IN;
			}
			if (IsSRGB(target.Format()))
			{
				info.Filter |= D3DX10_FILTER_SRGB_OUT;
				info.MipFilter |= D3DX10_FILTER_SRGB_OUT;
			}

			D3DX10LoadTextureFromTexture(d3dTexture1D_.get(), &info, other.D3DTexture().get());
		}
	}

	void D3D10Texture1D::Map1D(int level, TextureMapAccess tma,
			uint32_t x_offset, uint32_t /*width*/,
			void*& data)
	{
		uint8_t* p;
		TIF(d3dTexture1D_->Map(D3D10CalcSubresource(level, 0, 1), D3D10Mapping::Mapping(tma, type_, access_hint_, numMipMaps_), 0, reinterpret_cast<void**>(&p)));
		data = p + x_offset * NumFormatBytes(format_);
	}

	void D3D10Texture1D::Unmap1D(int level)
	{
		d3dTexture1D_->Unmap(D3D10CalcSubresource(level, 0, 1));
	}

	void D3D10Texture1D::BuildMipSubLevels()
	{
		if (d3d_sr_view_)
		{
			if (!(desc_.MiscFlags & D3D10_RESOURCE_MISC_GENERATE_MIPS))
			{
				desc_.MiscFlags |= D3D10_RESOURCE_MISC_GENERATE_MIPS;

				ID3D10Texture1D* d3d_tex;
				TIF(d3d_device_->CreateTexture1D(&desc_, NULL, &d3d_tex));

				d3d_device_->CopyResource(d3d_tex, d3dTexture1D_.get());

				d3dTexture1D_ = MakeCOMPtr(d3d_tex);

				ID3D10ShaderResourceView* d3d_sr_view;
				d3d_device_->CreateShaderResourceView(d3dTexture1D_.get(), NULL, &d3d_sr_view);
				d3d_sr_view_ = MakeCOMPtr(d3d_sr_view);
			}

			d3d_device_->GenerateMips(d3d_sr_view_.get());
		}
		else
		{
			D3DX10FilterTexture(d3dTexture1D_.get(), 0, D3DX10_FILTER_LINEAR);
		}
	}

	void D3D10Texture1D::UpdateParams()
	{
		d3dTexture1D_->GetDesc(&desc_);

		numMipMaps_ = static_cast<uint16_t>(desc_.MipLevels);
		BOOST_ASSERT(numMipMaps_ != 0);

		widthes_.resize(numMipMaps_);
		widthes_[0] = desc_.Width;
		for (uint16_t level = 1; level < numMipMaps_; ++ level)
		{
			widthes_[level] = widthes_[level - 1] / 2;
		}

		format_ = D3D10Mapping::MappingFormat(desc_.Format);
		bpp_	= NumFormatBits(format_);
	}
}