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
	void RenderBegin();
	void RenderEnd();

private:
	/* ������� �׸� �� ���� ���� */
	void CreateDeviceAndSwapChain();
	/* �� �۹��� ���� ������, GPU�� ��ǥ ���ۿ� ������ �� �ֵ��� ���ִ� ����Ÿ�� �� ��ü ���� */
	void CreateRenderTargetView();
	/* viewport ����*/
	void SetViewport();

private:
	/* �������� ���� �׸��� */
	void CreateGeometry();
	/* GPU�� ���� �����͸� �ùٸ��� �ؼ��� �� �ֵ��� �Է� ���̾ƿ�(Input Layout)�� ���� */
	void CreateInputLayout();

	void CreateVertexShader();
	void CreatePixelShader();

	/* Shader Resource View ���� */
	void CreateShaderResourceView();

	/* Constant Buffer ���� */
	void CreateConstantBuffer();

	/* DefaultVertexShader.hlsl���� shader �ε��ϰ� �������Ͽ� ����� ID3DBlob�� ���� */
	void LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version, ComPtr<ID3DBlob>& blob);

private:
	HWND _hwnd;
	uint32 _width = 0;
	uint32 _height = 0;

private:
	/* DirectX */
	// ComPtr�� ����ϸ� ref count �Ű澲�� �ʾƵ� �˾Ƽ� ������, wrl�� ���Ե�
	// ID3D11Device : ����̽� �������̽�, ���� ���˰� �ڿ� �Ҵ翡 ����, unit ����
	// ID3D11DeviceContext : ������ ����� �����ϴ� ����̽� ���ؽ�Ʈ, unit�� ���
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	// SwapChain : �������� �����͸� ��¿� ǥ���ϱ� ���� �����ϱ� ���� �ϳ� �̻��� ǥ���� ����
	ComPtr<IDXGISwapChain> _swapChain = nullptr;
	/* RenderTarget View
		: GPU�� �� ���۸� ������ ���(Target)���� �ν��ϰ�,
		  �ش� ���ۿ� �׸��� �׸� �� �ֵ��� �������ִ� ��(View) ��ü */
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	/* Misc : ����� �͵� */
	// D3D11_VIEWPORT : ȭ���� �����ϴ� ����ü
	D3D11_VIEWPORT _viewport = {0};
	float _clearColor[4] = { 1.0f, 0.576f, 0.674f, 0.5f };

private:
	/* Geometry */
	// ���� ����Ʈ (CPU �޸�)
	std::vector<Vertex> _vertices;
	// �ε��� ����Ʈ (CPU �޸�)
	std::vector<uint32> _indices;
	// GPU Buffers
	// ���� �����͸� GPU VRAM�� �����ϱ� ���� ���� (Vertex Buffer)
	ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;
	// �ε��� �����͸� GPU VRAM�� �����ϱ� ���� ���� (Index Buffer) 
	ComPtr<ID3D11Buffer> _indexBuffer = nullptr;

	/* Input Layout */
	// ���� ������ ������ ������ GPU���� �˷��ִ� �Է� ���̾ƿ�
	ComPtr<ID3D11InputLayout> _inputLayout = nullptr;

	/* Vertex Shader */
	// ���� ���̴� ��ü (GPU���� ���� ��ġ/�Ӽ� ó��)
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
	// ���� ���̴��� ����Ʈ �ڵ� (InputLayout �������� ���)
	ComPtr<ID3DBlob> _vertexBlob = nullptr;

	/* Pixel Shader */
	// �ȼ� ���̴� ��ü (GPU���� �ȼ� ���� ó��)
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
	// �ȼ� ���̴��� ����Ʈ �ڵ�
	ComPtr<ID3DBlob> _pixelBlob = nullptr;

	/* Shader Resource View 
		: ���̴�(Shader)�� GPU ���ҽ�(�ؽ�ó, ���� ��)�� ������ �� �ֵ��� ������ִ� ��ü */
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView = nullptr;

private:
	/* Constant Buffer */
	TransformData _transformData;
	ComPtr<ID3D11Buffer> _constantBuffer;
};