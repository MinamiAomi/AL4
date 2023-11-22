#include "TextureResource.h"

#include "Externals/DirectXTex/Include/DirectXTex.h"

#include "Helper.h"
#include "Graphics.h"
#include "CommandContext.h"

void TextureResource::CreateFromWICFile(const std::wstring& path) {


    // 中間リソースをコピーする
    CommandContext commandContext;
    commandContext.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
    CreateFromWICFile(commandContext, path);
    commandContext.Finish(true);
}

void TextureResource::Create(UINT width, UINT height, DXGI_FORMAT format, void* dataBegin) {

    // 中間リソースをコピーする
    CommandContext commandContext;
    commandContext.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);

    Create(commandContext, width, height, format, dataBegin);
    commandContext.Finish(true);

}


void TextureResource::CreateFromWICFile(CommandContext& commandContext, const std::wstring& path) {
    auto graphics = Graphics::GetInstance();
    auto device = graphics->GetDevice();

    auto& wname = path;

    // ファイルを読み込む
    DirectX::ScratchImage image{};
    ASSERT_IF_FAILED(DirectX::LoadFromWICFile(wname.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image));

    // ミップマップを生成
    DirectX::ScratchImage mipImages{};
    ASSERT_IF_FAILED(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages));

    // リソースを生成
    auto& metadata = mipImages.GetMetadata();
    desc_.Width = UINT(metadata.width);
    desc_.Height = UINT(metadata.height);
    desc_.MipLevels = UINT16(metadata.mipLevels);
    desc_.DepthOrArraySize = UINT16(metadata.arraySize);
    desc_.Format = metadata.format;
    desc_.SampleDesc.Count = 1;
    desc_.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);
    D3D12_HEAP_PROPERTIES heapPropeteies = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &heapPropeteies,
        D3D12_HEAP_FLAG_NONE,
        &desc_,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(resource_.GetAddressOf())));
    state_ = D3D12_RESOURCE_STATE_COPY_DEST;

    // 中間リソースを読み込む
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(resource_.Get(), 0, UINT(subresources.size()));

    D3D12_RESOURCE_DESC intermediateResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(intermediateSize);
    heapPropeteies = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &heapPropeteies,
        D3D12_HEAP_FLAG_NONE,
        &intermediateResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(intermediateResource.GetAddressOf())));
    UpdateSubresources(commandContext, resource_.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

    commandContext.TrackingObject(intermediateResource);
    commandContext.TrackingObject(resource_);
    commandContext.TransitionResource(*this, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    // ビューを生成
    CreateView();
}

void TextureResource::Create(CommandContext& commandContext, UINT width, UINT height, DXGI_FORMAT format, void* dataBegin) {
    auto graphics = Graphics::GetInstance();
    auto device = graphics->GetDevice();

    size_t pixelSize = Helper::GetBytePerPixel(format);
    size_t bufferSize = pixelSize * width * height;

    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto intermediateDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &intermediateDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(intermediateResource.GetAddressOf())));
    UINT8* uploadDataBegin = nullptr;
    ASSERT_IF_FAILED(intermediateResource->Map(0, nullptr, reinterpret_cast<void**>(&uploadDataBegin)));

    D3D12_SUBRESOURCE_FOOTPRINT pitchedDesc{};
    pitchedDesc.Format = format;
    pitchedDesc.Width = width;
    pitchedDesc.Height = height;
    pitchedDesc.Depth = 1;
    pitchedDesc.RowPitch = UINT(Helper::AlignUp(width * pixelSize, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));

    UINT8* uploadDataDest = reinterpret_cast<UINT8*>(Helper::AlignUp(reinterpret_cast<size_t>(uploadDataBegin), D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT));

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D{};
    placedTexture2D.Offset = uploadDataDest - uploadDataBegin;
    placedTexture2D.Footprint = pitchedDesc;

    for (UINT y = 0; y < height; ++y) {
        UINT8* scan = uploadDataBegin + placedTexture2D.Offset + y * pitchedDesc.RowPitch;
        memcpy(scan, &(reinterpret_cast<UINT8*>(dataBegin)[y * width]), pixelSize * width);
    }

    heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    desc_ = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 1);
    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &desc_,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(resource_.GetAddressOf())));
    state_ = D3D12_RESOURCE_STATE_COPY_DEST;

    ID3D12GraphicsCommandList* commandList = commandContext;

    auto destLocation = CD3DX12_TEXTURE_COPY_LOCATION(resource_.Get(), 0);
    auto srcLocation = CD3DX12_TEXTURE_COPY_LOCATION(intermediateResource.Get(), placedTexture2D);
    commandList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);
    commandContext.TrackingObject(intermediateResource);
    commandContext.TrackingObject(resource_);
    commandContext.TransitionResource(*this, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    CreateView();
}

void TextureResource::CreateView() {
    auto graphics = Graphics::GetInstance();
    auto device = graphics->GetDevice();

    if (srvHandle_.IsNull()) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = desc_.Format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc_.MipLevels;
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
}
