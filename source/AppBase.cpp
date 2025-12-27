#include "AppBase.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <dxgi.h>
#include <dxgi1_4.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);
namespace hlab {

    using namespace std;

    AppBase *g_appBase = nullptr;

    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        return g_appBase->MsgProc(hWnd, msg, wParam, lParam);
    }

    AppBase::AppBase()
        : m_screenWidth(1280), m_screenHeight(960), m_mainWindow(0),
        m_screenViewport(D3D11_VIEWPORT()) {
        g_appBase = this;
    }

    AppBase::~AppBase() { 
        g_appBase = nullptr;

         if (m_imguiDx11Inited)
            ImGui_ImplDX11_Shutdown();
        if (m_imguiWin32Inited)
            ImGui_ImplWin32_Shutdown();
        if (m_imguiContextCreated)
            ImGui::DestroyContext();

        if (m_mainWindow)
            DestroyWindow(m_mainWindow);
    }

    float AppBase::GetAspectRatio() const {
        return float(m_screenWidth - m_guiWidth) / m_screenHeight;
    }

    int AppBase::Run() {
    
        MSG msg = {0};
        while (WM_QUIT != msg.message) {
        
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        } else {
                 ImGui_ImplDX11_NewFrame();
                 ImGui_ImplWin32_NewFrame();

                 ImGui::NewFrame();
                 ImGui::Begin("Scene Control");

                 UpdateGUI();

                 ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

                 m_guiWidth = int(ImGui::GetWindowWidth());

                 ImGui::End();
                 ImGui::Render();

                 Update(ImGui::GetIO().DeltaTime);

                 Render();

                 ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

                 m_swapChain->Present(1, 0);
            }
        }

        return 0;
    }

    bool AppBase::Initialize() { 

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        if (!InitMainWindow())
            return false;

        if (!InitDirect3D())
            return false;

        if (!InitGUI())
            return false;

        return true;
    }

    LRESULT AppBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
            return true;

        switch (msg) { 
        case WM_SIZE:
            if (m_swapChain) {
                m_screenWidth = int(LOWORD(lParam));
                m_screenHeight = int(HIWORD(lParam));
                m_guiWidth = 0;

                m_renderTargetView.Reset();
                m_swapChain->ResizeBuffers(0,
                    (UINT)LOWORD(lParam),
                    (UINT)HIWORD(lParam),
                    DXGI_FORMAT_UNKNOWN,
                    0);
                CreateRenderTargetView();
                CreateDepthBuffer();
                SetViewport();
            }

            break;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        case WM_MOUSEMOVE:
            cout << "Mouse " << LOWORD(lParam) << " " << HIWORD(lParam) <<
            endl;
            break;
        case WM_LBUTTONUP:
            cout << "WM_LBUTTONUP Left mouse button" << endl;
            break;
        case WM_RBUTTONUP:
            cout << "WM_RBUTTONUP Right mouse button" << endl;
            break;
        case WM_KEYDOWN:
            cout << "WM_KEYDOWN " << (int)wParam << endl;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }

        return ::DefWindowProc(hwnd, msg, wParam, lParam);
    }

    void AppBase::ShutdownGui() {
        if (m_imguiDx11Inited) {
            ImGui_ImplDX11_Shutdown();
            m_imguiDx11Inited = false;
        }
        if (m_imguiWin32Inited) {
            ImGui_ImplWin32_Shutdown();
            m_imguiWin32Inited = false;
        }
        ImGui::DestroyContext();
    }

    bool AppBase::InitMainWindow() {
        
        WNDCLASSEX wc = {sizeof(WNDCLASSEX), 
            CS_CLASSDC, 
            WndProc, 
            0L, 0L,
            GetModuleHandle(NULL),
            NULL, NULL, NULL, NULL,
            L"Modeling",
            NULL
        };

        if (!RegisterClassEx(&wc)) {
            cout << "RegisterClassEx() failed." << endl;
            return false;
        }

        RECT wr = {0, 0, m_screenWidth, m_screenHeight};

        AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

        m_mainWindow = CreateWindow(wc.lpszClassName, L"ModelExample", 
            WS_OVERLAPPEDWINDOW,
            100,
            100,
            wr.right - wr.left,
            wr.bottom - wr.top,
            NULL,NULL,
            wc.hInstance,
            NULL);
        if (!m_mainWindow) {
            cout << "CreateWindow() failed." << endl;
            return false;
        }

        ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
        UpdateWindow(m_mainWindow);

        return true;
    }

    bool AppBase::InitDirect3D() {

        const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

        UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;

        const D3D_FEATURE_LEVEL featureLevels[2] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_9_3};
        D3D_FEATURE_LEVEL featureLevel;

        if (FAILED(D3D11CreateDevice(nullptr, driverType, 0, createDeviceFlags,
                                     featureLevels, ARRAYSIZE(featureLevels),
                                     D3D11_SDK_VERSION, device.GetAddressOf(),
                                     &featureLevel, context.GetAddressOf()))) {
            cout << "D3D11 CreateDevice() failed." << endl;
            return false;
        }

        if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
            cout << "D3D Feature Level 11 not support." << endl;
            return false;
        }

        device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4,
                                              &numQualityLevels);
        if (numQualityLevels <= 0) {
            cout << "4x MSAA not support." << endl;
        }

        if (FAILED(device.As(&m_device))) {
            cout << "device.As()failed." << endl;
            return false;
        }

        if (FAILED(context.As(&m_context))) {
            cout << "context.As() failed." << endl;
            return false;
        }

        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferDesc.Width = m_screenWidth;
        sd.BufferDesc.Height = m_screenHeight;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferCount = 2;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = m_mainWindow;
        sd.Windowed = TRUE;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        if (numQualityLevels > 0) {
            sd.SampleDesc.Count = 4;
            sd.SampleDesc.Quality = numQualityLevels - 1;
        } else {

            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
        }

     /*   if (FAILED(D3D11CreateDeviceAndSwapChain(
            0,
            driverType,
            0,
            createDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &sd,
            m_swapChain.GetAddressOf(), m_device.GetAddressOf(),
            &featureLevel, m_context.GetAddressOf()))) {
        cout << "D3D11CreateDeviceAndSwapChain() failed." << endl;
        return false;
        }*/

        UINT numFeatureLevels = _countof(featureLevels);

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr, driverType, nullptr, createDeviceFlags, featureLevels,
            numFeatureLevels, D3D11_SDK_VERSION, &sd,
            m_swapChain.GetAddressOf(), m_device.GetAddressOf(), &featureLevel,
            m_context.GetAddressOf());
        if (FAILED(hr)) {
            std::cout << "D3D11CreateDeviceAndSwapChain failed. hr=0x"
                      << std::hex << hr << std::dec << std::endl;
            return false;
        }

        CreateRenderTargetView();

        SetViewport();

        D3D11_RASTERIZER_DESC rastDesc;
        ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
        rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
        rastDesc.FrontCounterClockwise = false;
        rastDesc.DepthClipEnable = true;

        m_device->CreateRasterizerState(&rastDesc,
            m_solidRasterizerState.GetAddressOf());

        rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;

        m_device->CreateRasterizerState(&rastDesc,
            m_wireRasterizerState.GetAddressOf());

        CreateDepthBuffer();

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask =
            D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc =
            D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
        if (FAILED(m_device->CreateDepthStencilState(
            &depthStencilDesc, m_depthStencilState.GetAddressOf()))) {
        cout << "CreateDepthStencilState() failed." << endl;
        }

        return true;
    }

    bool AppBase::InitGUI() {
    
     /*   IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.DisplaySize = ImVec2(float(m_screenWidth), float(m_screenHeight));
        ImGui::StyleColorsLight();

        if (!ImGui_ImplDX11_Init(m_device.Get(), m_context.Get())) {
            return false;
        }

        if (!ImGui_ImplWin32_Init(m_mainWindow)) {
            return false;
        }

        return true;*/

         IMGUI_CHECKVERSION();
         ImGui::CreateContext();
         m_imguiContextCreated = true;

         ImGuiIO &io = ImGui::GetIO();
         (void)io;
         ImGui::StyleColorsDark();

         m_imguiWin32Inited = ImGui_ImplWin32_Init(m_mainWindow);
         if (!m_imguiWin32Inited)
             return false;

         m_imguiDx11Inited = ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());
         if (!m_imguiDx11Inited)
             return false;

         return true;
    }

    void AppBase::SetViewport() { 
        static int previousGuiWidth = m_guiWidth;

        if (previousGuiWidth != m_guiWidth) {
        
            previousGuiWidth = m_guiWidth;

            ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
            m_screenViewport.TopLeftX = float(m_guiWidth);
            m_screenViewport.TopLeftY = 0;
            m_screenViewport.Width = float(m_screenWidth - m_guiWidth);
            m_screenViewport.Height = float(m_screenHeight);
            m_screenViewport.MinDepth = 0.0f;
            m_screenViewport.MaxDepth = 1.0f;

            m_context->RSSetViewports(1, &m_screenViewport);
        }
    }

    bool AppBase::CreateRenderTargetView() {
        ComPtr<ID3D11Texture2D> backBuffer;
        m_swapChain->GetBuffer(0,IID_PPV_ARGS(backBuffer.GetAddressOf()));
        if (backBuffer) {
            m_device->CreateRenderTargetView(backBuffer.Get(), NULL,
                                             m_renderTargetView.GetAddressOf());
        } else {
            std::cout << "CreateRenderTargetView Failed." << std::endl;
            return false;
        }
        return true;
    }

    bool AppBase::CreateDepthBuffer() {
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        depthStencilBufferDesc.Width = m_screenWidth;
        depthStencilBufferDesc.Height = m_screenHeight;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        if (numQualityLevels > 0) {
            depthStencilBufferDesc.SampleDesc.Count = 4;
            depthStencilBufferDesc.SampleDesc.Quality = numQualityLevels - 1;
        } else {
            depthStencilBufferDesc.SampleDesc.Count = 1;
            depthStencilBufferDesc.SampleDesc.Quality = 0;
        }
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilBufferDesc.CPUAccessFlags = 0;
        depthStencilBufferDesc.MiscFlags = 0;

        if (FAILED(m_device->CreateTexture2D(
                &depthStencilBufferDesc, 0,
                m_depthStencilBuffer.GetAddressOf()))) {
            std::cout << "CreateTexture2D failed." << std::endl;
        }

        if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(),
                                                    0, &m_depthStencilView))) {
            std::cout << "CreateDepthStencilView failed." << std::endl;
        }
        return true;
    }

    void CheckResult(HRESULT hr, ID3DBlob *errorBlob) { 
        if (FAILED(hr)) {
            if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
                cout << "File not found." << endl;
            }

            if (errorBlob) {
                cout << "Shader compile error \n"
                     << (char *)errorBlob->GetBufferPointer() << endl;
            }
        }
    }

    void AppBase::CreateVertexShaderAndInputLayout(
        const wstring &filename,
        const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
        ComPtr<ID3D11VertexShader> &vertexShader,
        ComPtr<ID3D11InputLayout> &inputLayout) {

        ComPtr<ID3DBlob> shaderBlob;
        ComPtr<ID3DBlob> errorBlob;

        UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = D3DCompileFromFile(
            filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
            "vs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

        CheckResult(hr, errorBlob.Get());

        m_device->CreateVertexShader(shaderBlob->GetBufferPointer(),
                                     shaderBlob->GetBufferSize(), NULL,
                                     &vertexShader);

        m_device->CreateInputLayout(inputElements.data(),
                                    UINT(inputElements.size()),
                                    shaderBlob->GetBufferPointer(),
                                    shaderBlob->GetBufferSize(), &inputLayout);
    }

        void AppBase::CreatePixelShader(const wstring &filename, 
            ComPtr<ID3D11PixelShader> &pixelShader) {

            ComPtr<ID3DBlob> shaderBlob;
            ComPtr<ID3DBlob> errorBlob;

            UINT compileFlags = 0;

    #if defined(DEBUG) || defined(_DEBUG)
         compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    #endif

            HRESULT hr = D3DCompileFromFile(
                filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
                "ps_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

            CheckResult(hr, errorBlob.Get());

            m_device->CreatePixelShader(shaderBlob->GetBufferPointer(),
                                        shaderBlob->GetBufferSize(), NULL,
                                        &pixelShader);
        }

        void AppBase::CreateIndexBuffer(const std::vector<uint32_t> &indices,
                                        ComPtr<ID3D11Buffer> &indexBuffer) {
            D3D11_BUFFER_DESC bufferDesc = {};
            bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
            bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            bufferDesc.CPUAccessFlags = 0;
            bufferDesc.StructureByteStride = sizeof(uint32_t); 

            D3D11_SUBRESOURCE_DATA indexBufferData = {0};
            indexBufferData.pSysMem = indices.data();
            indexBufferData.SysMemPitch = 0;
            indexBufferData.SysMemSlicePitch = 0;

            m_device->CreateBuffer(&bufferDesc, &indexBufferData,
                                   indexBuffer.GetAddressOf());
        }
        ///

        void AppBase::CreateTexture(const std::string filename,
                              ComPtr<ID3D11Texture2D> &texture,
                              ComPtr<ID3D11ShaderResourceView> &textureResourceView,
                              bool useSRGB) {
            
            if (!m_device) {
                std::cout << "[Texture] m_device is NULL! file=" << filename
                          << "\n";
                return;
            }

            int width = 0, height = 0, channelsInFile = 0;

            unsigned char *img = stbi_load(filename.c_str(), &width, &height,
                                           &channelsInFile, 4);
            if (!img) {
                std::cout << "[Texture] stbi_load FAIL: " << filename << "\n";
                std::cout << "[Texture] reason: " << stbi_failure_reason()
                          << "\n";
                return;
            }

            texture.Reset();
            textureResourceView.Reset();

            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = (UINT)width;
            desc.Height = (UINT)height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = useSRGB ? 
                DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
                : DXGI_FORMAT_R8G8B8A8_UNORM;  
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            D3D11_SUBRESOURCE_DATA init = {};
            init.pSysMem = img;
            init.SysMemPitch = (UINT)(width * 4); // RGBA

            HRESULT hr =
                m_device->CreateTexture2D(&desc, &init, texture.GetAddressOf());
            if (FAILED(hr)) {
                std::cout << "[Texture] CreateTexture2D FAIL hr=0x" << std::hex
                          << hr << std::dec << " file=" << filename << "\n";
                stbi_image_free(img);
                return;
            }

            hr = m_device->CreateShaderResourceView(
                texture.Get(), nullptr, textureResourceView.GetAddressOf());
            if (FAILED(hr)) {
                std::cout << "[Texture] CreateSRV FAIL hr=0x" << std::hex << hr
                          << std::dec << " file=" << filename << "\n";
                stbi_image_free(img);
                return;
            }

            stbi_image_free(img);
        }
      
    }