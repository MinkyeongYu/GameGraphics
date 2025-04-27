#include "pch.h"
#include "Game.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;

	// [1] DirectX 핵심 객체들 생성
	// [2] 백버퍼 기반의 렌더 타겟 뷰 생성
	// [3] 화면에 출력할 뷰포트 크기 설정
	//_graphics = make_shared<Graphics>(hwnd);
	_graphics = std::make_shared<Graphics>(hwnd);

	{
		// Create Geometry
		_geometry = std::make_shared<Geometry<VertexTextureData>>();
		// Create Vertex Buffer
		_vertexBuffer = std::make_shared<VertexBuffer>(_graphics->GetDevice());
		// Create Index Buffer
		_indexBuffer = std::make_shared<IndexBuffer>(_graphics->GetDevice());
		// Create Input Layout
		_inputLayout = std::make_shared<InputLayout>(_graphics->GetDevice());
		// Create Vertex Shader
		_vertexShader = std::make_shared<VertexShader>(_graphics->GetDevice());
		// Create Constant Buffer
		_constantBuffer = std::make_shared<ConstantBuffer<TransformData>>(_graphics->GetDevice(), _graphics->GetDeviceContext());
		// Create Rasterizer State
		_rasterizerState = std::make_shared<RasterizerState>(_graphics->GetDevice());
		// Create Pixel Shader
		_pixelShader = std::make_shared<PixelShader>(_graphics->GetDevice());
		// Create Shader Resource View
		_shaderResoureView = std::make_shared<Texture>(_graphics->GetDevice());
		// Create Sampler State
		_samplerState = std::make_shared<SamplerState>(_graphics->GetDevice());
		// Create Blend State
		_blendState = std::make_shared<BlendState>(_graphics->GetDevice());
	}


	// [4] 정점 데이터 정의 및 GPU 버퍼에 업로드, vertexBuffer 생성
	/* Create Vertex Data & Index Data */				
	{
		/* Index Data */
		// 삼각형 1 : _vertices[0], _vertices[1], _vertices[2]
		// 삼각형 2 : _vertices[2], _vertices[1], _vertices[3]
		GeometryHelper::CreateRectangle(_geometry);
	}
	/* Vertex Buffer 생성 */
	{
		_vertexBuffer->Create(_geometry->GetVertices());
	}
	/* Index Buffer 생성 - 정점을 재사용하여 메모리 사용을 줄이고, 렌더링 성능을 최적화하기 위함 */
	{
		_indexBuffer->Create(_geometry->GetIndices());
	}
	
	// [5] 정점 셰이더 컴파일 및 생성
	/* vertex shader load 후 _vertexBlob에 결과 저장 */
	_vertexShader->Create(L"DefaultVertexShader.hlsl", "VS_main", "vs_5_0");  
	// [6] 정점 구조 정의 (셰이더와 버퍼 연결), CreateInputLayout()
	_inputLayout->Create(VertexTextureData::descs, _vertexShader->GetBlob());	
	// [7] 래스터라이저 상태 생성 (삼각형 그리는 방식, 컬링, 채우기 모드 등 설정)
	_rasterizerState->CreateRasterizerState();														
	
	// [8] 픽셀 셰이더 컴파일 및 생성
	/* pixel shader load 후 _pixelBlob에 결과 저장 */
	_pixelShader->Create(L"DefaultVertexShader.hlsl", "PS", "ps_5_0");	
	// [9] 샘플러 상태 생성
	_samplerState->CreateSamplerState();															

	// [10] 쉐이더 리소스 뷰 생성
	_shaderResoureView->CreateShaderResourceView(L"hamster_latte.png");
	// [11] 상수 버퍼 생성
	_constantBuffer->CreateConstantBuffer();
	// [12] 블렌딩 상태 생성
	_blendState->CreateBlendState();																
}

void Game::Update()
{
	_localPosition.x += 0.001f;

	// Create SRT
	Matrix scaleMatrix			= Matrix::CreateScale(_localScale / 3);
	Matrix rotationMatrix		= Matrix::CreateRotationX(_localRotation.x);
	rotationMatrix				*= Matrix::CreateRotationY(_localRotation.y);
	rotationMatrix				*= Matrix::CreateRotationZ(_localRotation.z);
	Matrix translationMatrix	= Matrix::CreateTranslation(_localPosition);

	// Create WorldMatrix
	Matrix worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	_transformData.worldMatrix = worldMatrix;

	// 버퍼에 데이터 복사
	_constantBuffer->CopyData(_transformData);
}

void Game::Render()
{
	/* 랜더링 시작 */
	_graphics->RenderBegin();

	/* IA - VS - RS - PS - OM */
	{
		// IA (Input Assembler) : 정점의 정보 전달
		uint32 stride = sizeof(VertexTextureData);					// 정점 1개 크기 (28바이트)
		uint32 offset = 0;											// 버퍼의 시작 위치 오프셋

		/* GPU에게 정점 버퍼의 크기와 위치(stride, offset) 전달, vertices 사용함을 GPU에 알려줌 */
		_graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
		/* 32-bit(4Byte) uint 인덱스 버퍼 GPU에 바인딩 */
		_graphics->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
		/* GPU에게 정점 데이터의 구조 전달 */
		_graphics->GetDeviceContext()->IASetInputLayout(_inputLayout->GetComPtr().Get());
		/* 각 정점을 어떻게 이어줄지 전달, 삼각형으로 이어주도록 설정 */
		_graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS (Vertex Shader) : 정점의 위치/색상 등 가공
		_graphics->GetDeviceContext()->VSSetShader(_vertexShader->GetComPtr().Get(), nullptr, 0);
		_graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, _constantBuffer->GetComPtr().GetAddressOf());

		// RS (Rasterizer) : 정점 → 픽셀로 삼각형 그리기
		/* Draw()호출 시 내부적으로 GPU가 자동으로 Rasterizer 실행함 */
		_graphics->GetDeviceContext()->RSSetState(_rasterizerState->GetComPtr().Get());

		// PS (Pixel Shader) : 픽셀 단위 색상 처리
		_graphics->GetDeviceContext()->PSSetShader(_pixelShader->GetComPtr().Get(), nullptr, 0);
		_graphics->GetDeviceContext()->PSSetShaderResources(0, 1, _shaderResoureView->GetComPtr().GetAddressOf());
		_graphics->GetDeviceContext()->PSSetSamplers(0, 1, _samplerState->GetComPtr().GetAddressOf());

		// OM (Output Merger) : 최종 픽셀을 렌더 타겟에 출력
		_graphics->GetDeviceContext()->OMSetBlendState(_blendState->GetComPtr().Get(), _blendState->GetBlendFactor(), _blendState->GetSampleMask());
		/* 실제 삼각형 그리기 (정점 개수 3, 시작 offset 0) */
		//_deviceContext->Draw(_vertices.size(), 0);
		_graphics->GetDeviceContext()->DrawIndexed(_geometry->GetIndexCount(), 0, 0);
	}

	/* 랜더링 끝, 화면에 출력 */
	_graphics->RenderEnd();
}