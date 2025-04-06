#pragma once

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
	void SetViewport();

private:
	HWND _hwnd;
	uint32 _width = 0;
	uint32 _height = 0;
	float _clearColor[4] = { 0.5f, 0.5f , 0.5f , 0.5f };

private:
	/* DirectX */
	// ComPtr�� ����ϸ� ref count �Ű澲�� �ʾƵ� �˾Ƽ� ������, wrl�� ���Ե�
	// ID3D11Device : ����̽� �������̽�, ���� ���˰� �ڿ� �Ҵ翡 ����, unit ����
	// ID3D11DeviceContext : ������ ����� �����ϴ� ����̽� ���ؽ�Ʈ, unit�� ���
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	// SwapChain : �������� �����͸� ��¿� ǥ���ϱ� ���� �����ϱ� ���� �ϳ� �̻��� ǥ���� ����
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	/* RenderTarget View */
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	/* Misc : ����� �͵� */
	// D3D11_VIEWPORT : ȭ���� �����ϴ� ����ü
	D3D11_VIEWPORT _viewport = {0};
};