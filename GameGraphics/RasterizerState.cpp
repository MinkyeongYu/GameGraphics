#include "pch.h"
#include "RasterizerState.h"

RasterizerState::RasterizerState(ComPtr<ID3D11Device> device)
	:_device(device)
{

}

RasterizerState::~RasterizerState()
{

}

void RasterizerState::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	// 픽셀을 채우는 방식: SOLID(채우기) 또는 WIREFRAME(선만 그리기)
	desc.FillMode = D3D11_FILL_SOLID;
	// 컬링 방식: BACK(뒤쪽 면 제거), FRONT(앞면 제거), NONE(제거 안 함)
	desc.CullMode = D3D11_CULL_BACK;
	// 삼각형의 앞면 시계방향/반시계방향 여부 (false = 시계방향이 앞면)
	desc.FrontCounterClockwise = false;

	HRESULT hResult = _device->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
	CHECK(hResult);
}