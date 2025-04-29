#include "pch.h"
#include "SamplerState.h"

SamplerState::SamplerState(ComPtr<ID3D11Device> device)
	:_device(device)
{
}

SamplerState::~SamplerState()
{
}

void SamplerState::CreateSamplerState()
{
	// 샘플러 상태(Sampler State) 설정 구조체 초기화
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// 텍스처 좌표 범위를 벗어났을 때 테두리 색상(Border Color) 사용 (U, V, W 각각에 적용)
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	// 테두리(Border) 색상 설정 R,G,B,A (빨강: 1,0,0,1)
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;

	// 비교 함수 (사용하지 않음 → 항상 통과)
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	// 필터 설정 (텍스처 축소/확대/밉맵 전부 선형 보간)
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	// 이방성 필터링 최대치 (일반적으로 16 사용)
	desc.MaxAnisotropy = 16;

	// 사용할 밉맵 LOD 범위 설정
	desc.MinLOD = FLT_MIN;
	desc.MaxLOD = FLT_MAX;

	// 밉맵 LOD 오프셋 (기본값 0)
	desc.MipLODBias = 0.f;

	// 위 설정을 기반으로 샘플러 상태 객체 생성
	HRESULT hResult = _device->CreateSamplerState(&desc, _samplerState.GetAddressOf());
	CHECK(hResult);
}