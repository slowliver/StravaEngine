#pragma once

#include <d3d12.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Graphics/Type.h>

namespace StravaEngine::Graphics::D3D12::Translator
{
namespace ToD3D12
{
STRAVA_FORCE_INLINE D3D12_PRIMITIVE_TOPOLOGY ToPrimitiveTopology(const PrimitiveTopology& in)
{
	D3D12_PRIMITIVE_TOPOLOGY out;
	switch (in)
	{
	case PrimitiveTopology::PointList:
		out = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	case PrimitiveTopology::LineList:
		out = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case PrimitiveTopology::LineStrip:
		out = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;
	case PrimitiveTopology::TriangleList:
		out = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	case PrimitiveTopology::TriangleStrip:
		out = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	default:
		STRAVA_ASSERT("Not translatable!");
		out = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		break;
	}
	return out;
}

STRAVA_FORCE_INLINE D3D12_PRIMITIVE_TOPOLOGY_TYPE ToPrimitiveTopologyType(const PrimitiveTopology& in)
{
	D3D12_PRIMITIVE_TOPOLOGY_TYPE out;
	switch (in)
	{
	case PrimitiveTopology::PointList:
		out = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		break;
	case PrimitiveTopology::LineList:
		out = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		break;
	case PrimitiveTopology::LineStrip:
	case PrimitiveTopology::TriangleList:
	case PrimitiveTopology::TriangleStrip:
		out = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		break;
	default:
		STRAVA_ASSERT("Not translatable!");
		out = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		break;
	}
	return out;
}

STRAVA_FORCE_INLINE D3D12_VIEWPORT ToViewport(const Viewport& in)
{
	D3D12_VIEWPORT out = CD3DX12_VIEWPORT
	(
		in.m_left,
		in.m_top,
		in.m_left + in.m_width,
		in.m_top + in.m_height,
		in.m_minDepth,
		in.m_maxDepth
	);
	return out;
}

STRAVA_FORCE_INLINE D3D12_RECT ToRect(const Core::Int32Rect& in)
{
	D3D12_RECT out = CD3DX12_RECT
	(
		in.m_left,
		in.m_top,
		in.m_right,
		in.m_bottom
	);
	return out;
}
}
}
