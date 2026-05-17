#pragma once
#include "Framework.hpp"

// 1. 부모 추상 클래스
class Material {
public:
    ShaderSet shaders;
    Material(ShaderSet s) : shaders(s) {}
    virtual ~Material() {}
    virtual void Bind(ID3D11DeviceContext* context) = 0;
};

// 2. 색상 머티리얼 클래스
class ColorMaterial : public Material {
public:
    XMFLOAT4 color;
    ID3D11Buffer* pColorBuffer = nullptr;

    // 생성자가 클래스 내부({ } 안쪽)에 있어야 합니다!
    ColorMaterial(ShaderSet s, XMFLOAT4 col, ID3D11Device* dev)
        : Material(s), color(col)
    {
        D3D11_BUFFER_DESC cbd = { 0 };
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(ColorBuffer); //
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        HRESULT hr = dev->CreateBuffer(&cbd, nullptr, &pColorBuffer);
        if (FAILED(hr)) {
            OutputDebugString(L"Failed to create Color Constant Buffer\n");
        }
    }

    // 소멸자: 자원 해제
    virtual ~ColorMaterial() {
        if (pColorBuffer) pColorBuffer->Release();
    }

    void SetColor(XMFLOAT4 col) { color = col; }

    // 부모의 순수 가상 함수를 반드시 구현해야 합니다
    void Bind(ID3D11DeviceContext* context) override {
        context->IASetInputLayout(shaders.layout);
        context->VSSetShader(shaders.vs, nullptr, 0);
        context->PSSetShader(shaders.ps, nullptr, 0);

        ColorBuffer cb = { color }; //
        context->UpdateSubresource(pColorBuffer, 0, nullptr, &cb, 0, 0);
        context->PSSetConstantBuffers(1, 1, &pColorBuffer);
    }
}; 