#include "pch.h"
#include "BlendState.h"

BlendState::BlendState(ComPtr<ID3D11Device> device)
	:_device(device)
{

}

BlendState::~BlendState()
{

}

void BlendState::CreateBlendState(D3D11_RENDER_TARGET_BLEND_DESC blendDesc, float factor)
{
	_blendFactor = factor;

	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// 멀티 샘플링 시 알파 커버리지 사용 여부 (일반적으로 false)
	desc.AlphaToCoverageEnable = false;

	// MRT(Multiple Render Targets) 각각에 다른 블렌드 설정을 할지 여부 (false = 동일 설정 사용)
	desc.IndependentBlendEnable = false;

	// 렌더 타겟 0번 블렌딩 설정
	desc.RenderTarget[0] = blendDesc;								

	// 위 설정을 기반으로 BlendState 생성
	HRESULT hResult = _device->CreateBlendState(&desc, _blendState.GetAddressOf());
	CHECK(hResult);
}