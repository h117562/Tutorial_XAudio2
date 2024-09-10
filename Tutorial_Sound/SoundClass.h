#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_


#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

#pragma comment(lib, "xaudio2.lib")

#include <xaudio2.h>
#include <windows.h>

class SoundClass
{
public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	bool Initialize(HWND, const char*);
	bool PlayAudio();
	void Shutdown();

private:
	bool InitializeXAudio(HWND);
	bool LoadSoundFile(const char*);
	HRESULT FindChunk(HANDLE, DWORD, DWORD&, DWORD&);
	HRESULT ReadChunkData(HANDLE, void*, DWORD, DWORD);

private:
	IXAudio2* m_xAudio2;
	IXAudio2MasteringVoice* m_xAudio2MasteringVoice;
	IXAudio2SourceVoice* m_xAudio2SourceVoice;
};

#endif