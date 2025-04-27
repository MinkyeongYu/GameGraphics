#pragma once

/* RasterizerState
 * : 삼각형을 화면에 렌더링할 때, 채우기 모드(Fill Mode), 컬링(Culling), 앞면/뒷면 정의 등
 *   래스터화(Rasterization) 과정의 상태를 설정하는 클래스
 */
class RasterizerState
{
public:
	RasterizerState(ComPtr<ID3D11Device> device);
	~RasterizerState();

public:
	ComPtr<ID3D11RasterizerState> GetComPtr() { return _rasterizerState; }

	/* RasterizerState 생성
   - 삼각형을 화면에 어떻게 그릴지 설정 (채우기 모드, 컬링, 앞면 방향 등) */
	void CreateRasterizerState();

private:
	ComPtr<ID3D11Device> _device;

	/* Rasterizer State */
	ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr;
};