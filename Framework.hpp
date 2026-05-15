#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <vector>
#include <string>
#include <chrono>
#include <functional>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

struct Vertex { XMFLOAT3 pos; XMFLOAT4 col; };
struct ConstantBuffer { XMMATRIX matWorld; };
struct ColorBuffer { XMFLOAT4 tintColor; };

struct ShaderSet {
    ID3D11VertexShader* vs = nullptr;
    ID3D11PixelShader* ps = nullptr;
    ID3D11InputLayout* layout = nullptr;
    void Release() {
        if (vs) vs->Release(); if (ps) ps->Release(); if (layout) layout->Release();
    }
};