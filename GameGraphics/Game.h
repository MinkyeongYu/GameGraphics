#pragma once
#include "pch.h"

/* Game
 * : 게임 루프(Init, Update, Render)를 관리하고, 렌더링 파이프라인 전반을 구성하는 클래스
 */
class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();

private:
	HWND _hwnd;

	/* Graphics
	: Device, DeviceContext, SwapChain, RenderTargetView, Viewport 생성 */
	std::shared_ptr<Graphics> _graphics;

	/* Vertex Buffer
	: 정점(Vertex) 데이터를 GPU VRAM에 업로드하고, IA(Input Assembler) 단계에서 사용 */
	std::shared_ptr<VertexBuffer> _vertexBuffer;

	/* Index Buffer
	: 인덱스(Index) 데이터를 GPU VRAM에 업로드하고, IA(Input Assembler) 단계에서 사용 */
	std::shared_ptr<IndexBuffer> _indexBuffer;

	/* Input Layout
	: 정점 버퍼(Vertex Buffer)의 데이터 구조를 GPU(셰이더)에게 알려주기 위한 객체 */
	std::shared_ptr<InputLayout> _inputLayout;

	/* Geometry
	: CPU 메모리 상에서 정점(Vertex)과 인덱스(Index) 데이터를 관리하는 객체 */
	std::shared_ptr<Geometry<VertexTextureData>> _geometry;

	/* Vertex Shader
	: 정점 변환 단계 (월드/뷰/프로젝션 행렬 적용)에서 실행되는 셰이더 */
	std::shared_ptr<VertexShader> _vertexShader;

	/* Constant Buffer
	: IA(Input Assembler) 이후 VS(Vertex Shader)/PS(Pixel Shader) 단계에서 공통 데이터를 전달하는 용도 */
	std::shared_ptr<ConstantBuffer<TransformData>> _constantBuffer;

	/* Rasterizer State
	 * : 삼각형을 화면에 어떻게 그릴지(채우기, 컬링 등) 설정하는 래스터라이저 상태 객체
	 */
	std::shared_ptr<RasterizerState> _rasterizerState;

	/* Pixel Shader
	: 픽셀 색상 계산 단계 (텍스처, 라이팅 적용)에서 실행되는 셰이더 */
	std::shared_ptr<PixelShader> _pixelShader;

	/* Shader Resource View
	: 셰이더(Shader)가 GPU 리소스(텍스처, 버퍼 등)에 접근할 수 있도록 만들어주는 객체 */
	std::shared_ptr<Texture> _shaderResoureView;

	/* Sampler State
	 * : 텍스처 좌표(UV) 샘플링 방식과 경계 처리 방식을 정의하는 샘플러 상태 객체
	 */
	std::shared_ptr<SamplerState> _samplerState;

	/* Blend State
	 * : 픽셀 색상 혼합(알파 블렌딩 등) 방식을 정의하는 블렌딩 상태 객체
	 */
	std::shared_ptr<BlendState> _blendState;

private:
	/* Constant Buffer */
	TransformData _transformData;
	//ComPtr<ID3D11Buffer> _constantBuffer;

	/* SRT */
	Vec3 _localPosition = { 0.f, 0.f, 0.f };
	Vec3 _localRotation = { 0.f, 0.f, 0.f };
	Vec3 _localScale = { 1.f, 1.f, 1.f };
};