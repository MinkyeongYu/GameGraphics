#pragma once

/* Geometry
 * : 정점(Vertex)과 인덱스(Index) 데이터를 CPU 메모리 상에서 관리하는 템플릿 클래스
 */
template<typename T>
class Geometry
{
public:
	Geometry() {}
	~Geometry() {}

public:
	// ------ Vertex -------
	uint32 GetVertexCount() { return static_cast<uint32>(_vertices.size()); }

	void* GetVertexData() { return _vertices.data(); }
	const std::vector<T>& GetVertices() { return _vertices; }

	void AddVertex(const T& vertex) { _vertices.push_back(vertex); }
	void AddVertices(const std::vector<T> vertices) { _vertices.insert(_vertices.end(), vertices.begin(), vertices.end()); }
	void SetVertices(const std::vector<T> vertices) { _vertices = vertices; }

	// ------ Index -------
	uint32 GetIndexCount() { return static_cast<uint32>(_indices.size()); }

	void* GetIndexData() { return _indices.data(); }
	const std::vector<uint32>& GetIndices() { return _indices; }

	void AddIndex(uint32 index) { _indices.push_back(index); }
	void AddIndices(const std::vector<uint32>& indices) { _indices.insert(_indices.end(), indices.begin(), indices.end()); }
	void SetIndices(const std::vector<uint32>& indices) { _indices = indices; }

private:
	std::vector<T> _vertices;
	std::vector<uint32> _indices;
};