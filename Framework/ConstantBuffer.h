#pragma once
#include <wrl/client.h>
#include <cstring>
#include <type_traits>



template<typename T>
class ConstantBuffer {
    static_assert(std::is_trivially_copyable_v<T>, "ConstantBuffer<T>: T must be trivially copyable");

public:
    ConstantBuffer() = default;

    // ������
    bool Initialize(ID3D11Device* device) {
        // �萔�o�b�t�@�p�̃o�b�t�@�I�u�W�F�N�g���쐬����
        D3D11_BUFFER_DESC bufferDesc{};
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(T));	
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      
        return SUCCEEDED(device->CreateBuffer(&bufferDesc, nullptr, buffer.ReleaseAndGetAddressOf()));
    }

    // �l���ύX���ꂽ�ꍇ�̂ݍX�V
    void UpdateIfNeeded(ID3D11DeviceContext* context, const T& data) {
        if (!buffer) return;  // ���S�`�F�b�N

        if (std::memcmp(&cachedData, &data, sizeof(T)) != 0)
        {
            D3D11_MAPPED_SUBRESOURCE mapped{};
            HRESULT hr = context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
            if (SUCCEEDED(hr) && mapped.pData) 
            {
                std::memcpy(mapped.pData, &data, sizeof(T));
                context->Unmap(buffer.Get(), 0);
                cachedData = data;
            }
            else 
            {
                OutputDebugStringA("Map failed or pData is null\n");
            }
        }
    }

    // �����X�V
    void Update(ID3D11DeviceContext* context, const T& data) 
    {
        // GPU���g�p���郊�\�[�X�̃�������CPU����A�N�Z�X����ۂɗ��p����\����
        D3D11_MAPPED_SUBRESOURCE mappedResource{};
        // �萔�o�b�t�@���}�b�v����
        context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        std::memcpy(mappedResource.pData, &data, sizeof(T));
       
        // �}�b�v����
        context->Unmap(buffer.Get(), 0);

        cachedData = data;
    }

    ID3D11Buffer* GetBuffer() const noexcept {
        return buffer.Get();
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    T cachedData{};  // �O�񑗐M�������e
};