#pragma once

namespace JR {

class Buffer {
public:
	bool Create(D3D11_BUFFER_DESC bufferDesc);

	void Bind(uint32_t stride, uint32_t offset);
	void Bind(uint32_t slot);

	template <typename T>
	void SetData(const std::vector<T>& data) {
		SetData(static_cast<const void*>(data.data()), static_cast<uint32_t>(sizeof(T) * data.size()));
	}

	void SetData(const void* data, uint32_t bytes);

	template <typename T>
	void SetData(const T& data) {
		SetData(&data, sizeof(T));
	}

private:
	ComPtr<ID3D11Buffer> mBuffer;
};

}  // namespace JR