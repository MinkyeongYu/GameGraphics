#pragma once
#include "pch.h"


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
	/* 기하학적 도형 그리기 */
	void CreateGeometry();
	/* GPU가 정점 데이터를 올바르게 해석할 수 있도록 입력 레이아웃(Input Layout)을 생성 */
	void CreateInputLayout();

	void CreateVertexShader();
	void CreatePixelShader();

	/* RasterizerState 생성
	   - 삼각형을 화면에 어떻게 그릴지 설정 (채우기 모드, 컬링, 앞면 방향 등) */
	void CreateRasterizerState();

	/* Sampler State 생성
	   - 픽셀 셰이더에서 텍스처 좌표(UV)를 어떻게 샘플링할지 설정
		 (보간 방식, 경계 처리 방식 등) */
	void CreateSamplerState();

	/* Blend State 생성
	   - 픽셀이 렌더 타겟에 출력될 때, 기존 픽셀과 새 픽셀을 어떻게 섞을지 설정
		 (투명도, 알파 블렌딩 등 처리 방식) */
	void CreateBlendState();

	/* Shader Resource View 생성 */
	void CreateShaderResourceView();

	/* Constant Buffer 생성 */
	void CreateConstantBuffer();

	/* DefaultVertexShader.hlsl에서 shader 로드하고 컴파일하여 결과를 ID3DBlob에 저장 */
	void LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version, ComPtr<ID3DBlob>& blob);

private:
	HWND _hwnd;

	// Device, DeviceContext, SwapChain, RenderTargetView, Viewport 생성
	std::shared_ptr<Graphics> _graphics;
	// 정점 데이터를 GPU VRAM에 전달하기 위한 버퍼 (Vertex Buffer) 생성
	std::shared_ptr<VertexBuffer> _vertexBuffer;
	// 인덱스 데이터를 GPU VRAM에 전달하기 위한 버퍼 (Index Buffer) 생성
	std::shared_ptr<IndexBuffer> _indexBuffer;
	// 정점 버퍼의 데이터 구조를 GPU에게 알려주는 입력 레이아웃
	std::shared_ptr<InputLayout> _inputLayout;
	// 정점 데이터, 인덱스 데이터 구조 생성
	std::shared_ptr<Geometry<VertexTextureData>> _geometry;

private:
	/* Geometry */
	// 정점 리스트 (CPU 메모리)
	//std::vector<Vertex> _vertices;
	// 인덱스 리스트 (CPU 메모리)
	//std::vector<uint32> _indices;
	
	// Input Assembler - GPU Buffers
	// Vertex Buffer
	// Index Buffer
	// Input Layout


	/* Vertex Shader */
	// 정점 셰이더 객체 (GPU에서 정점 위치/속성 처리)
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
	// 정점 셰이더의 바이트 코드 (InputLayout 생성에도 사용)
	ComPtr<ID3DBlob> _vertexBlob = nullptr;

	/* Rasterizer State */
	ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr;

	/* Pixel Shader */
	// 픽셀 셰이더 객체 (GPU에서 픽셀 색상 처리)
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
	// 픽셀 셰이더의 바이트 코드
	ComPtr<ID3DBlob> _pixelBlob = nullptr;

	/* Shader Resource View 
		: 셰이더(Shader)가 GPU 리소스(텍스처, 버퍼 등)에 접근할 수 있도록 만들어주는 객체 */
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView = nullptr;

	/* Sampler State 객체
   - 텍스처 샘플링 시 보간 방식(Filter), 경계 처리(AddressMode) 등을 정의한 상태 객체
   - 픽셀 셰이더에서 텍스처 좌표(UV)를 어떻게 해석할지 GPU에게 알려줌 */
	ComPtr<ID3D11SamplerState> _samplerState = nullptr;

	/*	Blend State 객체 
		: 픽셀 색상 혼합 방식 설정: 알파 블렌딩 등 */
	ComPtr<ID3D11BlendState> _blendState = nullptr;
private:
	/* Constant Buffer */
	TransformData _transformData;
	ComPtr<ID3D11Buffer> _constantBuffer;

	/* SRT */
	Vec3 _localPosition = { 0.f, 0.f, 0.f };
	Vec3 _localRotation = { 0.f, 0.f, 0.f };
	Vec3 _localScale = { 1.f, 1.f, 1.f };
};