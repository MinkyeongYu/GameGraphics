#pragma once

/*	Constant Buffer:
*	: CPU가 쓰고, GPU가 읽는 용도로, 자주 업데이트되는 데이터(ex: 월드행렬, 뷰행렬 등)를 GPU에 빠르게 넘기기 위해 사용하는 버퍼
*/
template <typename T>
class ConstantBuffer
{
public:
	ConstantBuffer(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
		:_device(device), _deviceContext(deviceContext)
	{
	}

	~ConstantBuffer() {}

public:
	ComPtr<ID3D11Buffer> GetComPtr() { return _constantBuffer; }

	/* Constant Buffer 생성 */
	void CreateConstantBuffer()
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		// CPU 쓰기 가능, GPU 읽기 가능 (매 프레임마다 CPU에서 업데이트 할 경우)
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(T);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		// 설정된 desc를 기반으로 GPU에 Constant Buffer 생성 후 _constantBuffer에 저장
		HRESULT hResult = _device->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
		CHECK(hResult);
	}

	/* Map, UnMap 함수, 버퍼에 Data 복사 */
	void CopyData(const T& data)
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		ZeroMemory(&subResource, sizeof(subResource));

		// GPU의 Constant Buffer를 CPU에서 쓰기 위한 접근 요청
		_deviceContext->Map(
			_constantBuffer.Get(),			// 업데이트할 Constant Buffer
			0,								// 서브리소스 인덱스 (일반적으로 0)
			D3D11_MAP_WRITE_DISCARD,		// 이전 내용은 버리고 새로 쓰기 (가장 일반적인 방식)
			0,								// Reserved (항상 0)
			&subResource					// 매핑 결과를 받을 구조체 (CPU가 접근 가능한 포인터 제공됨)
		);
		// _transformData값을 GPU메모리로 복사한 후, GPU의 Constant Buffer에 업로드
		::memcpy(subResource.pData, &data, sizeof(data));
		// 맵핑 해제 → GPU에서 읽을 수 있도록 다시 연결
		_deviceContext->Unmap(_constantBuffer.Get(), 0);
	}

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _deviceContext;

	ComPtr<ID3D11Buffer> _constantBuffer;
};