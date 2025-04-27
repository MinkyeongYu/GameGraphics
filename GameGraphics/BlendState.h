#pragma once

/*	Blend State 객체
	: 픽셀 색상 혼합 방식 설정: 알파 블렌딩 등 */
class BlendState
{
public:
	BlendState(ComPtr<ID3D11Device> device);
	~BlendState();

public:
	ComPtr<ID3D11BlendState> GetComPtr() { return _blendState; }

	const float* GetBlendFactor() { return &_blendFactor; }
	uint32 GetSampleMask() { return _sampleMask; }

	/* Blend State 생성
   - 픽셀이 렌더 타겟에 출력될 때, 기존 픽셀과 새 픽셀을 어떻게 섞을지 설정
	 (투명도, 알파 블렌딩 등 처리 방식) */
	void CreateBlendState(D3D11_RENDER_TARGET_BLEND_DESC blendDesc =
		{
			true,
			D3D11_BLEND_SRC_ALPHA,
			D3D11_BLEND_INV_SRC_ALPHA,
			D3D11_BLEND_OP_ADD,
			D3D11_BLEND_ONE,
			D3D11_BLEND_ZERO,
			D3D11_BLEND_OP_ADD,
			D3D11_COLOR_WRITE_ENABLE_ALL
		}, float factor = 0.f);

private:
	ComPtr<ID3D11Device> _device;

	ComPtr<ID3D11BlendState> _blendState;

	float _blendFactor = 0.f;
	uint32 _sampleMask = 0xFFFFFFFF;
};