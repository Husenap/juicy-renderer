#include "Buffer.h"

#include "Framework.h"

namespace JR {

bool Buffer::Create(D3D11_BUFFER_DESC bufferDesc) {
	HRESULT hr = MM::Get<Framework>().Device()->CreateBuffer(&bufferDesc, nullptr, &mBuffer);
	if (FAILED(hr)) {
		LOG_ERROR("Failed to Create Buffer");
		return false;
	}

	return true;
}

void Buffer::Bind(uint32_t stride, uint32_t offset) {
	MM::Get<Framework>().Context()->IASetVertexBuffers(0, 1, mBuffer.GetAddressOf(), &stride, &offset);
}

void Buffer::Bind(uint32_t slot) {
	auto& context = MM::Get<Framework>().Context();

	context->VSSetConstantBuffers(slot, 1, mBuffer.GetAddressOf());
	context->GSSetConstantBuffers(slot, 1, mBuffer.GetAddressOf());
	context->PSSetConstantBuffers(slot, 1, mBuffer.GetAddressOf());
}

void Buffer::SetData(const void* data, uint32_t bytes) {
	auto& context = MM::Get<Framework>().Context();

	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	std::memcpy(ms.pData, data, bytes);
	context->Unmap(mBuffer.Get(), NULL);
}

}  // namespace JR