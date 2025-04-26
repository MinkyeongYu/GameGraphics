#include "pch.h"
#include "VertexData.h"


std::vector<D3D11_INPUT_ELEMENT_DESC> VertexTextureData::descs = 
{
	/* 정점 위치(POSITION): float 3개 (X, Y, Z), 오프셋 0부터 시작 */
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	/*  UV : float 2개 (R, G), 오프셋 12부터 시작 (POSITION 뒤, D3D11_APPEND_ALIGNED_ELEMENT가 오프셋을 알아서 계산해줌) */
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

std::vector<D3D11_INPUT_ELEMENT_DESC> VertexColorData::descs =
{
	/* 정점 위치(POSITION): float 3개 (X, Y, Z), 오프셋 0부터 시작 */
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	/*  UV : float 4개 (R, G, B, A), 오프셋 12부터 시작 (POSITION 뒤, D3D11_APPEND_ALIGNED_ELEMENT가 오프셋을 알아서 계산해줌) */
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
};