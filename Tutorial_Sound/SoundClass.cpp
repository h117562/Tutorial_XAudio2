#include "SoundClass.h"

SoundClass::SoundClass()
{
	m_xAudio2 = 0;
	m_xAudio2MasteringVoice = 0;
	m_xAudio2SourceVoice = 0;
}

SoundClass::SoundClass(const SoundClass& other)
{
}

SoundClass::~SoundClass()
{
}

bool SoundClass::Initialize(HWND hwnd, const char* filePath)
{
	bool result;

	//XAudio 초기화
	result = InitializeXAudio(hwnd);
	if (!result)
	{
		return false;
	}
	
	//wav 파일 로드
	result = LoadSoundFile(filePath);
	if (!result)
	{
		return false;
	}

	return true;
}

bool SoundClass::InitializeXAudio(HWND hwnd)
{
	HRESULT result;

	//COM 초기화
	result = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(result))
	{
		return false;
	}

	//XAudio2 생성
	result = ::XAudio2Create(&m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(result))
	{
		return false;
	}

	//마스터링 보이스 생성
	result = m_xAudio2->CreateMasteringVoice(&m_xAudio2MasteringVoice);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool SoundClass::LoadSoundFile(const char* filePath)
{
	HRESULT result;

	WAVEFORMATEXTENSIBLE wfx = { 0 };
	XAUDIO2_BUFFER buffer = { 0 };

	//CreateFile을 사용하여 오디오 파일을 엽니다.
	HANDLE hFile = CreateFileA(
		filePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return false;
	}
		
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
	{
		return false;
	}


	DWORD dwChunkSize;
	DWORD dwChunkPosition;

	//오디오 파일에서 'RIFF' 청크를 찾아 파일 형식을 확인합니다.
	//check the file type, should be fourccWAVE or 'XWMA'
	result = FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	if (FAILED(result))
	{
		return false;
	}

	//RIFF 청크를 읽어 파일 타입을 ㅓㅈ장
	DWORD filetype;
	ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	if (FAILED(result))
	{
		return false;
	}

	//Wav 확장자가 아닐 경우 리턴
	if (filetype != fourccWAVE)
	{
		return false;
	}


	//'fmt' 청크를 찾아 해당 내용을 WAVEFORMATEXTENSIBLE 구조체로 저장합니다.
	result = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	if (FAILED(result))
	{
		return false;
	}

	result = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	if (FAILED(result))
	{
		return false;
	}
		

	//'data' 청크를 찾아 해당 내용을 버퍼로 읽습니다.
	//fill out the audio data buffer with the contents of the fourccDATA chunk
	result = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	if (FAILED(result))
	{
		return false;
	}

	//읽은 데이터를 XAudio 버퍼에 저장
	BYTE* pDataBuffer = new BYTE[dwChunkSize];

	result = ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);
	if (FAILED(result))
	{
		return false;
	}

	buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
	buffer.pAudioData = pDataBuffer;  //buffer containing audio data
	buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer


	//소스 보이스 생성
	result = m_xAudio2->CreateSourceVoice(&m_xAudio2SourceVoice, (WAVEFORMATEX*)&wfx);
	if (FAILED(result))
	{
		return false;
	}

	result = m_xAudio2SourceVoice->SubmitSourceBuffer(&buffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool SoundClass::PlayAudio()
{
	HRESULT result;
	
	//오디오 재생
	result = m_xAudio2SourceVoice->Start(0);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

HRESULT SoundClass::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		DWORD dwRead;
		if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
				return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;

	}

	return S_OK;
}

HRESULT SoundClass::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT result;

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
		
	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
	{
		result = HRESULT_FROM_WIN32(GetLastError());
	}
		
	return S_OK;
}

void SoundClass::Shutdown()
{

	if (m_xAudio2SourceVoice)
	{
		m_xAudio2SourceVoice->DestroyVoice();
		m_xAudio2SourceVoice = 0;
	}


	if (m_xAudio2MasteringVoice)
	{
		m_xAudio2MasteringVoice->DestroyVoice();
		m_xAudio2MasteringVoice = 0;
	}

	if (m_xAudio2)
	{
		m_xAudio2->Release();
		m_xAudio2 = 0;
	}

	return;
}