#include "Speaker.h"

#include <cassert>

#include "AudioDevice.h"

Speaker::~Speaker() {
    Destroy();
}

Speaker& Speaker::operator=(const std::shared_ptr<Sound>& sound) {
    if (sound_ != sound) {
        sound_ = sound;
        Create();
    }
    return *this;
}

void Speaker::Play(bool loop) {
    HRESULT hr = S_FALSE;
    XAUDIO2_BUFFER buffer{};
    buffer.pAudioData = sound_->GetMediaData().data();
    buffer.AudioBytes = sizeof(BYTE) * sound_->GetMediaData().size();
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    if (loop) {
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    }
    hr = sourceVoice_->SubmitSourceBuffer(&buffer);
    assert(SUCCEEDED(hr));
    hr = sourceVoice_->Start();
    assert(SUCCEEDED(hr));
}

void Speaker::Stop() {
    assert(sourceVoice_);
    HRESULT hr = S_FALSE;
    hr = sourceVoice_->Stop();
    assert(SUCCEEDED(hr));
}

void Speaker::SetVolume(float volume) {
    assert(sourceVoice_);
    HRESULT hr = S_FALSE;
    hr = sourceVoice_->SetVolume(volume);
    assert(SUCCEEDED(hr));
}

void Speaker::SetPitch(float pitch) {
    assert(sourceVoice_);
    HRESULT hr = S_FALSE;
    hr = sourceVoice_->SetFrequencyRatio(pitch);
    assert(SUCCEEDED(hr));
}

bool Speaker::IsPlaying() const {
    if (!sound_ || !sourceVoice_) { return false; }
    XAUDIO2_VOICE_STATE state{};
    sourceVoice_->GetState(&state);
    return state.BuffersQueued > 0;
}

void Speaker::Create() {
    assert(sound_);
    Destroy();
    HRESULT hr = S_FALSE;
    hr = AudioDevice::GetInstance()->GetXAudio2()->CreateSourceVoice(&sourceVoice_, sound_->GetWaveFormat());
    assert(SUCCEEDED(hr));

}

void Speaker::Destroy() {
    if (sourceVoice_) {
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
    }
}
