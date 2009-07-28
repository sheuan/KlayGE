#ifndef _MODEL_HPP
#define _MODEL_HPP

#include <KlayGE/PreDeclare.hpp>
#include <string>
#include <KlayGE/Mesh.hpp>

class DetailedSkinnedMesh;

class DetailedSkinnedModel : public KlayGE::SkinnedModel
{
	friend class DetailedSkinnedMesh;

public:
	DetailedSkinnedModel(std::wstring const & name);

	void SetTime(float time);
	void SetLightPos(KlayGE::float3 const & light_pos);
	void SetEyePos(KlayGE::float3 const & eye_pos);

	void VisualizeLighting();
	void VisualizeVertex(KlayGE::VertexElementUsage usage, KlayGE::uint8_t usage_index);
	void VisualizeTexture(int slot);

	void LineMode(bool line_mode);

	KlayGE::TexturePtr const & EmptyNormalMap() const
	{
		return empty_normal_map_;
	}

private:
	KlayGE::TexturePtr empty_normal_map_;
};

class DetailedSkinnedMesh : public KlayGE::SkinnedMesh
{
public:
	DetailedSkinnedMesh(KlayGE::RenderModelPtr const & model, std::wstring const & name);

	void OnRenderBegin();

	void SetWorld(KlayGE::float4x4 const & mat);
	void SetLightPos(KlayGE::float3 const & light_pos);
	void SetEyePos(KlayGE::float3 const & eye_pos);

	void BuildMeshInfo();

	void VisualizeLighting();
	void VisualizeVertex(KlayGE::VertexElementUsage usage, KlayGE::uint8_t usage_index);
	void VisualizeTexture(int slot);

	void LineMode(bool line_mode);

	bool HasOpacityMap() const
	{
		return has_opacity_map_;
	}

private:
	void UpdateTech();

private:
	KlayGE::float4x4 world_;
	KlayGE::float4x4 inv_world_;
	KlayGE::RenderEffectPtr effect_;
	KlayGE::float3 light_pos_;

	bool line_mode_;
	std::string visualize_;
	bool has_opacity_map_;
};

#endif		// _MODEL_HPP