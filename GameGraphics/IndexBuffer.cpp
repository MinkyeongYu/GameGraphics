#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(ComPtr<ID3D11Device> device)
	: _device(device)
{

}

IndexBuffer::~IndexBuffer()
{

}

void IndexBuffer::Create(std::vector<uint32> indices)
{
	_stride = sizeof(uint32);
	_count = static_cast<uint32>(indices.size());

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	/* GPU만 읽을 수 있음. CPU 접근 불가 */
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	/* Index Buffer 바인딩 용도로 사용 */
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	/* uint32 크기 * 인덱스 개수 */
	desc.ByteWidth = static_cast<uint32>(_stride * _count);

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	/* 첫번째 data 시작주소, _indices.data()로 대체 가능 */
	data.pSysMem = &indices[0];

	// desc와 data를 기반으로 버퍼 생성 후 _vertexBuffer에 저장
	HRESULT hResult = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
	CHECK(hResult);
}