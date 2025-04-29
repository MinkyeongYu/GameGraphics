#pragma once

/* VertexBuffer
 * : 정점(Vertex) 데이터를 GPU VRAM에 업로드하고, IA(Input Assembler) 단계에서 사용하는 버퍼 클래스
 */
class VertexBuffer
{
public:
	VertexBuffer(ComPtr<ID3D11Device> device);
	~VertexBuffer();

public:
	ComPtr<ID3D11Buffer> GetComPtr() { return _vertexBuffer; }
	uint32 GetStride() { return _stride; }
	uint32 GetOffset() { return _offset; }
	uint32 GetCount() { return _count; }

	template<typename T>
	void Create(const std::vector<T>& vertices)
	{
		_stride = sizeof(T);
		_count = static_cast<uint32>(vertices.size());

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		/* GPU만 읽을 수 있음. CPU 접근 불가 */
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		/* Vertex Buffer 바인딩 용도로 사용 */
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		/* Vertex 크기 * 정점 개수 */
		desc.ByteWidth = static_cast<uint32>(_stride * _count);

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		/* 첫번째 data 시작주소, _vertices.data()로 대체 가능 */
		data.pSysMem = &vertices[0];

		// desc와 data를 기반으로 버퍼 생성 후 _vertexBuffer에 저장
		_device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
	}

private:
	ComPtr<ID3D11Device> _device;

	// GPU Buffer
	// 정점 데이터를 GPU VRAM에 전달하기 위한 버퍼 (Vertex Buffer)
	ComPtr<ID3D11Buffer> _vertexBuffer;

	uint32 _stride = 0;
	uint32 _offset = 0;
	uint32 _count = 0;

};

