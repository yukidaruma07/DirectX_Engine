#include "AudioManager.h"
#include <filesystem>

namespace AudioManager {
    inline IXAudio2* audio = nullptr;
    inline IXAudio2MasteringVoice* masteringVoice = nullptr;
    inline std::vector<AudioData> audioList;
    std::vector<MediaData> mediaList;
}

void AudioManager::InitManager() {
    HRESULT result = {};
    result = XAudio2Create(&audio);
    result = audio->CreateMasteringVoice(&masteringVoice);

    result = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
}

int AudioManager::Load(std::string fileName, bool loop) {
    for (int n = 0; n < audioList.size(); n++) {
        if (audioList[n].fileName == fileName) { //リストと同じ名前（=すでに使っている）
            return n;
        }
    }
    HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD byteSize = 0;
    Chunk riffChunk = {};
    ReadFile(hFile, &riffChunk.id, 4, &byteSize, NULL);
    ReadFile(hFile, &riffChunk.size, 4, &byteSize, NULL);

    char wave[4] = "";
    ReadFile(hFile, &wave, 4, &byteSize, NULL);

    Chunk formatChunk = {};
    while (formatChunk.id[0] != 'f') {
        ReadFile(hFile, &formatChunk.id, 4, &byteSize, NULL);
    }
    ReadFile(hFile, &formatChunk.size, 4, &byteSize, NULL);

    WAVEFORMATEX format = {};
    ReadFile(hFile, &format.wFormatTag, 2, &byteSize, NULL);
    ReadFile(hFile, &format.nChannels, 2, &byteSize, NULL);
    ReadFile(hFile, &format.nSamplesPerSec, 4, &byteSize, NULL);
    ReadFile(hFile, &format.nAvgBytesPerSec, 4, &byteSize, NULL);
    ReadFile(hFile, &format.nBlockAlign, 2, &byteSize, NULL);
    ReadFile(hFile, &format.wBitsPerSample, 2, &byteSize, NULL);

    Chunk data = {};

    while (true) {

        ReadFile(hFile, &data.id, 4, &byteSize, NULL);
        if (strcmp(data.id, "data") == 0) {
            break;
        }
        else {
            ReadFile(hFile, &data.size, 4, &byteSize, NULL);
            char* buf = new char[data.size];
            ReadFile(hFile, buf, data.size, &byteSize, NULL);
        }
    }

    ReadFile(hFile, &data.size, 4, &byteSize, NULL);
    char* buf = new char[data.size];
    ReadFile(hFile, buf, data.size, &byteSize, NULL);
    CloseHandle(hFile);

    AudioData audioData = {};
    audioData.fileName = fileName;
    audioData.buffer.AudioBytes = data.size;
    audioData.buffer.pAudioData = (BYTE*)buf;
    audioData.buffer.Flags = XAUDIO2_END_OF_STREAM;
    audioData.sourceVoice = new IXAudio2SourceVoice * [1];
    
    if (loop) {
        audioData.buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    }
    audio->CreateSourceVoice(&audioData.sourceVoice[0], &format);
    audioList.push_back(audioData);

    return (int)audioList.size() - 1;
}

int AudioManager::LoadMP3(std::string fileName) {
    if (!std::filesystem::exists(fileName)) {
        MessageBox(NULL, "ファイルが見つかりませんでした", NULL, MB_OK);
        return -1;
    }
    HRESULT result = {};

    MediaData mediaData = {};
    std::wstring wFileName = std::wstring(fileName.begin(), fileName.end());
    result = MFCreateSourceReaderFromURL(wFileName.c_str(), NULL, &mediaData.mfSourceReader);
    result = MFCreateMediaType(&mediaData.mfMediaType);
    mediaData.mfMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    mediaData.mfMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    mediaData.mfSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, mediaData.mfMediaType);
    mediaData.mfMediaType->Release();
    mediaData.mfSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &mediaData.mfMediaType);

    WAVEFORMATEX* waveFormat = nullptr;
    MFCreateWaveFormatExFromMFMediaType(mediaData.mfMediaType, &waveFormat, nullptr);

    
    while (true) {
        IMFSample* mfSample = nullptr;
        DWORD dwStreamFlag = {};
        mediaData.mfSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlag, nullptr, &mfSample);

        if (dwStreamFlag & MF_SOURCE_READERF_ENDOFSTREAM) {
            break;
        }

        IMFMediaBuffer* mfMediaBuffer = nullptr;
        mfSample->ConvertToContiguousBuffer(&mfMediaBuffer);

        BYTE* buffer = nullptr;
        DWORD currentLength = {};
        mfMediaBuffer->Lock(&buffer, nullptr, &currentLength);

        mediaData.mediaBuffer.resize(mediaData.mediaBuffer.size() + currentLength);
        memcpy_s(mediaData.mediaBuffer.data() + mediaData.mediaBuffer.size() - currentLength, currentLength, buffer, currentLength);

        mfMediaBuffer->Unlock();
        mfSample->Release();

    }

    audio->CreateSourceVoice(&mediaData.sourceVoice, waveFormat);

    mediaList.push_back(mediaData);
    return mediaList.size() - 1;
}

void AudioManager::Play(int id) {
    XAUDIO2_VOICE_STATE state = {};
    audioList[id].sourceVoice[0]->GetState(&state);
    if (state.BuffersQueued == 0) {
        audioList[id].sourceVoice[0]->SubmitSourceBuffer(&audioList[id].buffer);
        audioList[id].sourceVoice[0]->Start();
    }
}

void AudioManager::PlayMP3(int id) {
    XAUDIO2_BUFFER buffer = {};
    buffer.pAudioData = mediaList[id].mediaBuffer.data();
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.AudioBytes = sizeof(BYTE) * (UINT32)mediaList[id].mediaBuffer.size();
    mediaList[id].sourceVoice->SubmitSourceBuffer(&buffer);
    mediaList[id].sourceVoice->Start();
}

void AudioManager::Stop(int id) {
    audioList[id].sourceVoice[0]->Stop();                               // 再生を止めるだけ
    audioList[id].sourceVoice[0]->FlushSourceBuffers(); // キューから完全に削除する
}

void AudioManager::ChangeVolume(int id, int volume) {
    audioList[id].sourceVoice[0]->SetVolume(volume);
}
