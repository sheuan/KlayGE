// D3D11Query.hpp
// KlayGE D3D11��ѯ�� ʵ���ļ�
// Ver 3.8.0
// ��Ȩ����(C) ������, 2009
// Homepage: http://klayge.sourceforge.net
//
// 3.8.0
// ���ν��� (2009.1.30)
//
// �޸ļ�¼
/////////////////////////////////////////////////////////////////////////////////

#ifndef _D3D11OCCLUSIONQUERY_HPP
#define _D3D11OCCLUSIONQUERY_HPP

#include <KlayGE/Query.hpp>

namespace KlayGE
{
	class D3D11OcclusionQuery : public OcclusionQuery
	{
	public:
		D3D11OcclusionQuery();

		void Begin();
		void End();

		uint64_t SamplesPassed();

	private:
		ID3D11QueryPtr query_;
	};

	class D3D11ConditionalRender : public ConditionalRender
	{
	public:
		D3D11ConditionalRender();

		void Begin();
		void End();

		void BeginConditionalRender();
		void EndConditionalRender();

	private:
		ID3D11PredicatePtr predicate_;
	};
}

#endif		// _D3D11QUERY_HPP