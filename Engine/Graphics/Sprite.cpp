#include "Sprite.h"

#include "Core/TextureLoader.h"
#include "Core/SamplerManager.h"

std::shared_ptr<Texture> Texture::Load(const std::filesystem::path& path) {
	// privateコンストラクタをmake_sharedで呼ぶためのヘルパー
	struct Helper : Texture {
		Helper() : Texture() {}
	};
	std::shared_ptr<Texture> texture = std::make_shared<Helper>();
	texture->resource_->Create(path);
	return texture;
}

const DescriptorHandle& Texture::GetSampler() const {
	switch (interpolation_) {
	case Texture::Interpolation::Linear:
		switch (extension_) {
		case Texture::Extension::Wrap:
			return SamplerManager::LinearWrap;
		case Texture::Extension::Clamp:
			return SamplerManager::LinearClamp;
		}
		break;
	case Texture::Interpolation::Point:
		switch (extension_) {
		case Texture::Extension::Wrap:
			return SamplerManager::PointWrap;
		case Texture::Extension::Clamp:
			return SamplerManager::PointClamp;
		}
		break;
	}
    return SamplerManager::AnisotropicWrap;
}

std::list<Sprite*> Sprite::instanceList_;

Sprite::Sprite() {
	instanceList_.emplace_back(this);
}

Sprite::~Sprite() {
	instanceList_.remove(this);
}
