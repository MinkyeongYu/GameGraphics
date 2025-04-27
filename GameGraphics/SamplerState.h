#pragma once

/* Sampler State 객체
- 텍스처 샘플링 시 보간 방식(Filter), 경계 처리(AddressMode) 등을 정의한 상태 객체
- 픽셀 셰이더에서 텍스처 좌표(UV)를 어떻게 해석할지 GPU에게 알려줌 */
class SamplerState
{
public:
	SamplerState(ComPtr<ID3D11Device> device);
	~SamplerState();

public:
	ComPtr<ID3D11SamplerState> GetComPtr() { return _samplerState; }

	/* Sampler State 생성
   - 픽셀 셰이더에서 텍스처 좌표(UV)를 어떻게 샘플링할지 설정
	 (보간 방식, 경계 처리 방식 등) */
	void CreateSamplerState();

private:
	ComPtr<ID3D11Device> _device;

	ComPtr<ID3D11SamplerState> _samplerState;
};