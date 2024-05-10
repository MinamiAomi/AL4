#include "Graphics.h"

#include <dxgi1_6.h>

#include <cassert>
#include <format>

#include "Helper.h"
#include "SamplerManager.h"
#include "TextureLoader.h"
#include "LinearAllocator.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

using namespace Microsoft::WRL;

#define DEBUG_DIRECTX
#define ENABLED_DEBUG_LAYER 1
#define ENABLED_GPU_BASED_DEBUGGER 0
#define ENABLED_DEBUG_DRED 0

#ifdef DEBUG_DIRECTX

#include <dxgidebug.h>

namespace {
    struct LeakChecker {
        ~LeakChecker() {
            ComPtr<IDXGIDebug1> debug;
            if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
                debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
                debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
                debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
            }
        }
    } leakChecker;

    std::string ToString(D3D12_AUTO_BREADCRUMB_OP op) {



        switch (op)
        {
        case D3D12_AUTO_BREADCRUMB_OP_SETMARKER:
            return "Setmarker";
        case D3D12_AUTO_BREADCRUMB_OP_BEGINEVENT:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_ENDEVENT:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_DRAWINSTANCED:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_DRAWINDEXEDINSTANCED:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_EXECUTEINDIRECT:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_DISPATCH:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_COPYBUFFERREGION:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_COPYTEXTUREREGION:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_COPYRESOURCE:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_COPYTILES:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCE:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_CLEARRENDERTARGETVIEW:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_CLEARUNORDEREDACCESSVIEW:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_CLEARDEPTHSTENCILVIEW:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_RESOURCEBARRIER:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_EXECUTEBUNDLE:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_PRESENT:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_RESOLVEQUERYDATA:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_BEGINSUBMISSION:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_ENDSUBMISSION:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT64:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCEREGION:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_WRITEBUFFERIMMEDIATE:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME1:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_SETPROTECTEDRESOURCESESSION:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME2:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES1:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_BUILDRAYTRACINGACCELERATIONSTRUCTURE:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_EMITRAYTRACINGACCELERATIONSTRUCTUREPOSTBUILDINFO:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_COPYRAYTRACINGACCELERATIONSTRUCTURE:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_DISPATCHRAYS:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_INITIALIZEMETACOMMAND:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_EXECUTEMETACOMMAND:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_ESTIMATEMOTION:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_RESOLVEMOTIONVECTORHEAP:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_SETPIPELINESTATE1:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_INITIALIZEEXTENSIONCOMMAND:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_EXECUTEEXTENSIONCOMMAND:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_DISPATCHMESH:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_ENCODEFRAME:
            break;
        case D3D12_AUTO_BREADCRUMB_OP_RESOLVEENCODEROUTPUTMETADATA:
            break;
        default:
            break;
        }
    }

}
#endif // DEBUG_DIRECTX

Graphics* Graphics::GetInstance() {
    static Graphics instance;
    return &instance;
}

void Graphics::Initialize() {
    CreateDevice();

    D3D12_FEATURE_DATA_SHADER_MODEL featureShaderModel{};
    if (SUCCEEDED(device_->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &featureShaderModel, sizeof(featureShaderModel)))) {
        if (featureShaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_6) {
            // HLSL 6.6に対応してない
            assert(false);
        }

    }

    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5{};
    if (SUCCEEDED(device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5)))) {
        if (options5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
            ASSERT_IF_FAILED(device_.As(&dxrDevice_));
            OutputDebugStringA("DXR supported!!\n");
        }
    }

    directCommandSet_.queue.Create();
    computeCommandSet_.queue.Create();
    copyCommandSet_.queue.Create();

    uint32_t numDescriptorsTable[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    numDescriptorsTable[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = kNumRTVs;
    numDescriptorsTable[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = kNumDSVs;
    numDescriptorsTable[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = kNumSRVs;
    numDescriptorsTable[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = kNumSamplers;

    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
        descriptorHeaps_[i] = std::make_shared<DescriptorHeap>();
        descriptorHeaps_[i]->Create(D3D12_DESCRIPTOR_HEAP_TYPE(i), numDescriptorsTable[i]);
    }

    for (int i = 0; i < LinearAllocatorType::Count; ++i) {
        linearAllocatorPagePools_[i].Initialize((LinearAllocatorType::Type)i);
    }

    SamplerManager::Initialize();
    CreateDynamicResourcesRootSignature();
}

void Graphics::Finalize() {
    directCommandSet_.queue.WaitForIdle();
    computeCommandSet_.queue.WaitForIdle();
    copyCommandSet_.queue.WaitForIdle();
    TextureLoader::ReleaseAll();
    for (int i = 0; i < LinearAllocatorType::Count; ++i) {
        linearAllocatorPagePools_[i].Finalize();
    }
    releasedObjectTracker_.AllRelease();
}

DescriptorHandle Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type) {
    return descriptorHeaps_[type]->Allocate();
}

//void Graphics::CheckDRED(HRESULT presentReturnValue) {
//    static  const char* AutoBreadcrumbsOp[] = {
//                "SETMARKER",
//                "BEGINEVENT",
//                "ENDEVENT ",
//                "DRAWINSTANCED",
//                "DRAWINDEXEDINSTANCED",
//                "EXECUTEINDIRECT",
//                "DISPATCH",
//                "COPYBUFFERREGION",
//                "COPYTEXTUREREGION",
//                "COPYRESOURCE",
//                "COPYTILES",
//                "RESOLVESUBRESOURCE",
//                "CLEARRENDERTARGETVIEW",
//                "CLEARUNORDEREDACCESSVIEW",
//                "CLEARDEPTHSTENCILVIEW",
//                "RESOURCEBARRIER",
//                "EXECUTEBUNDLE",
//                "PRESENT",
//                "RESOLVEQUERYDATA",
//                "BEGINSUBMISSION",
//                "ENDSUBMISSION",
//                "DECODEFRAME",
//                "PROCESSFRAMES",
//                "ATOMICCOPYBUFFERUINT",
//                "ATOMICCOPYBUFFERUINT64",
//                "RESOLVESUBRESOURCEREGION",
//                "WRITEBUFFERIMMEDIATE",
//                "DECODEFRAME1",
//                "SETPROTECTEDRESOURCESESSION",
//                "DECODEFRAME2",
//                "PROCESSFRAMES1",
//                "BUILDRAYTRACINGACCELERATIONSTRUCTURE",
//                "EMITRAYTRACINGACCELERATIONSTRUCTUREPOSTBUILDINFO",
//                "COPYRAYTRACINGACCELERATIONSTRUCTURE",
//                "DISPATCHRAYS",
//                "INITIALIZEMETACOMMAND",
//                "EXECUTEMETACOMMAND",
//                "ESTIMATEMOTION",
//                "RESOLVEMOTIONVECTORHEAP",
//                "SETPIPELINESTATE1",
//                "INITIALIZEEXTENSIONCOMMAND",
//                "EXECUTEEXTENSIONCOMMAND",
//                "DISPATCHMESH",
//                "ENCODEFRAME",
//                "RESOLVEENCODEROUTPUTMETADATA"
//    };
//
//    static const char* DredAllocationType[] = {
//        "COMMAND_QUEUE",
//        "COMMAND_ALLOCATOR",
//        "PIPELINE_STATE",
//        "COMMAND_LIST",
//        "FENCE",
//        "DESCRIPTOR_HEAP",
//        "HEAP",
//        "QUERY_HEAP",
//        "COMMAND_SIGNATURE",
//        "PIPELINE_LIBRARY",
//        "VIDEO_DECODER",
//        "VIDEO_PROCESSOR",
//        "RESOURCE",
//        "PASS",
//        "CRYPTOSESSION",
//        "CRYPTOSESSIONPOLICY",
//        "PROTECTEDRESOURCESESSION",
//        "VIDEO_DECODER_HEAP",
//        "COMMAND_POOL",
//        "COMMAND_RECORDER",
//        "STATE_OBJECT",
//        "METACOMMAND",
//        "SCHEDULINGGROUP",
//        "VIDEO_MOTION_ESTIMATOR",
//        "VIDEO_MOTION_VECTOR_HEAP",
//        "VIDEO_EXTENSION_COMMAND",
//        "VIDEO_ENCODER",
//        "VIDEO_ENCODER_HEAP",
//        "INVALID"
//    };
//
//
//    if (presentReturnValue == DXGI_ERROR_DEVICE_REMOVED) {
//        ComPtr<ID3D12DeviceRemovedExtendedData> dred;
//        ASSERT_IF_FAILED(device_.As(&dred));
//
//        D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT autoBreadcrumbsOutput = {};
//        ASSERT_IF_FAILED(dred->GetAutoBreadcrumbsOutput(&autoBreadcrumbsOutput));
//
//        auto node = autoBreadcrumbsOutput.pHeadAutoBreadcrumbNode;
//        while (node != nullptr) {
//            auto error = std::format(
//                "Node :\n"
//                "CommandList  Name = {}, Address = {}\n"
//                "CommandQueue Name = {}, Address = {}\n"
//                "Command Count = {}\n"
//                "Completed Commands = {}\n",
//                node->pCommandListDebugNameA,
//                node->pCommandList,
//                node->pCommandQueueDebugNameA,
//                node->pCommandQueue,
//                node->BreadcrumbCount,
//                *(node->pLastBreadcrumbValue));
//            OutputDebugStringA(error.c_str());
//
//            for (uint32_t i = 0; i < node->BreadcrumbCount; ++i) {
//                auto cmd = node->pCommandHistory[i];
//                error = std::format("Command[{}] : {}\n", i, AutoBreadcrumbsOp[cmd]);
//                OutputDebugStringA(error.c_str());
//            }
//            node = node->pNext;
//        }
//
//        D3D12_DRED_PAGE_FAULT_OUTPUT pageFaultOutput{};
//        ASSERT_IF_FAILED(dred->GetPageFaultAllocationOutput(&pageFaultOutput));
//
//        OutputDebugStringA(std::format("Page Fault Virtual Adress = {}\n", pageFaultOutput.PageFaultVA).c_str());
//
//        auto allocationNode = pageFaultOutput.pHeadExistingAllocationNode;
//        while (allocationNode != nullptr) {
//            auto error = std::format("Existing Allocation Node Name = {}, Type = {}\n", allocationNode->ObjectNameA, DredAllocationType[allocationNode->AllocationType]);
//            OutputDebugStringA(error.c_str());
//            allocationNode = allocationNode->pNext;
//        }
//
//        allocationNode = pageFaultOutput.pHeadRecentFreedAllocationNode;
//        while (allocationNode != nullptr) {
//            auto error = std::format("Existing Allocation Node Name = {}, Type = {}\n", allocationNode->ObjectNameA, DredAllocationType[allocationNode->AllocationType]);
//            OutputDebugStringA(error.c_str());
//            allocationNode = allocationNode->pNext;
//        }
//    }
//}

Graphics::Graphics() :
    directCommandSet_(D3D12_COMMAND_LIST_TYPE_DIRECT),
    computeCommandSet_(D3D12_COMMAND_LIST_TYPE_COMPUTE),
    copyCommandSet_(D3D12_COMMAND_LIST_TYPE_COPY) {
}

void Graphics::CreateDevice() {
#ifdef DEBUG_DIRECTX
#if ENABLED_DEBUG_LAYER || ENABLED_GPU_BASED_DEBUGGER
    ComPtr<ID3D12Debug1> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
#if ENABLED_DEBUG_LAYER 
        debugController->EnableDebugLayer();
        OutputDebugStringA("Enable DebugLayer!\n");
#endif
#if ENABLED_GPU_BASED_DEBUGGER
        debugController->SetEnableGPUBasedValidation(TRUE);
        OutputDebugStringA("Enable GPU-based validation!\n");
#endif
    }
#endif
#if ENABLED_DEBUG_DRED
    ComPtr<ID3D12DeviceRemovedExtendedDataSettings> dredSettings;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(dredSettings.GetAddressOf())))) {
        dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
        dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
    }
#endif
#endif // DEBUG_DIRECTX

    ComPtr<IDXGIFactory7> factory;
    ASSERT_IF_FAILED(CreateDXGIFactory(IID_PPV_ARGS(factory.GetAddressOf())));

    ComPtr<IDXGIAdapter4> adapter;
    for (uint32_t i = 0;
        factory->EnumAdapterByGpuPreference(
            i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(adapter.GetAddressOf())) != DXGI_ERROR_NOT_FOUND;
        ++i) {
        // アダプター情報を取得
        DXGI_ADAPTER_DESC3 adapterDesc{};
        ASSERT_IF_FAILED(adapter->GetDesc3(&adapterDesc));
        // ソフトウェアアダプタでなければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 採用したアダプタ情報を出力
            OutputDebugStringW(std::format(L"Use Adapter:{}\n", adapterDesc.Description).c_str());
            break;
        }
        adapter.Reset(); // ソフトウェアアダプタは見なかったことにする
    }
    assert(adapter);

    // 機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
    };
    const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
    // 高い順に生成できるか試していく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        // 採用したアダプターデバイスを生成
        // 指定した機能レベルでデバイスが生成できたかを確認
        if (SUCCEEDED(D3D12CreateDevice(
            adapter.Get(), featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf())))) {
            // 生成できたのでログ出力を行ってループを抜ける
            OutputDebugStringA(std::format("FeatureLevel : {}\n", featureLevelStrings[i]).c_str());
            break;
        }
    }
    assert(device_);

#ifdef DEBUG_DIRECTX
    // デバッグ時のみ
    ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())))) {
        // やばいエラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // エラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);
    }
#endif // DEBUG_DIRECTX
}

void Graphics::CreateDynamicResourcesRootSignature() {
    D3D12_ROOT_SIGNATURE_DESC dynamicResourcesRootSignatureDesc{};
    dynamicResourcesRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    dynamicResourcesRootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    dynamicResourcesRootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
    dynamicResourcesRootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

    CD3DX12_ROOT_PARAMETER rootParameters[1]{};
    rootParameters[0].InitAsConstantBufferView(0);
    dynamicResourcesRootSignatureDesc.pParameters = rootParameters;
    dynamicResourcesRootSignatureDesc.NumParameters = _countof(rootParameters);
    dynamicResourcesRootSignature_.Create(L"RootSignature DynamicResources", dynamicResourcesRootSignatureDesc);
}
