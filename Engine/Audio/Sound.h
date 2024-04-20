#pragma once

#include <xaudio2.h>

#include <memory>
#include <filesystem>
#include <vector>

class Sound {
public:
    static std::shared_ptr<Sound> Load(const std::filesystem::path& path);

    const WAVEFORMATEX* GetWaveFormat() const { return waveFormat_; }
    const std::vector<BYTE>& GetMediaData() const { return mediaData_; }

private:
    Sound();
    ~Sound();

    WAVEFORMATEX* waveFormat_;
    std::vector<BYTE> mediaData_;
};