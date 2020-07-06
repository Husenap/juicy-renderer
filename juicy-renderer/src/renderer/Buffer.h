#pragma once

namespace JR {

class Buffer {
public:
	bool Create(D3D11_BUFFER_DESC bufferDesc);

	void Bind(uint32_t stride, uint32_t offset);

	template<typename T>
	void SetData(const std::vector<T>& data) {
		SetData(data.data(), sizeof(T) * data.size());
	}

	void SetData(const void* data, uint32_t bytes);

private:

	ComPtr<ID3D11Buffer> mBuffer;
};

}  // namespace JR