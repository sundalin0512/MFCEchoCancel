#pragma once
// EchoCancelWithCaptureAndRender.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <vector>
#include "../AEC/delayEstimation/delayEstimation.h"
#include "../AEC/delayEstimation/delayEstimation_emxAPI.h"
#include <iostream>

using namespace std;

namespace MyAudioClient
{
#define EXIT_ON_ERROR(hres)  \
			  if (FAILED(hres)) { exit(-1); }
#define SAFE_RELEASE(punk)  \
			  if ((punk) != NULL)  \
				{ (punk)->Release(); (punk) = NULL; }


	template <class T> void SafeRelease(T **ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}


	const int ciFrameSize = 480;
	const int REFTIMES_PER_SEC = 10000000;
	const int REFTIMES_PER_MILLISEC = 10000;
	const int audioLength20s = 48000 * 20;
	const int audioLength5s = 48000 * 2;
	const int audioLength1s = 48000;
	const int audioLength500ms = 24000;
	const int audioLength200ms = 9600;
	const int audioLength100ms = 4800;
	const int audioLength1ms = 48;

	int renderProcessDataNum = 0;
	int captureProcessDataNum = 0;
	int renderProcessPosition = 0;
	int captureProcessPosition = 0;

	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	const IID IID_IAudioClient = __uuidof(IAudioClient);
	const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
	const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

	const TCHAR *szLoadRenderDataEvent = L"szLoadRenderDataEvent";
	const TCHAR *szSaveCaptureDataEvent = L"szSaveCaptureDataEvent";
	const TCHAR *szRenderThreadEvent = L"RenderThreadEvent";
	//const TCHAR *szStopEvent = L"StopEvent";
	bool StopFlag = false;

	CRITICAL_SECTION criticalSection;

	WAVEFORMATEX *WaveFormat;

	int delayMs = 0;
	int caculateDelayMs = 0;
	bool randonDelayFlag = 0;
	int maxRandonDelayMs = 0;
	int minRandonDelayMs = 0;

	class WaveData
	{
	public:
		float* data;
		UINT size;
		WaveData* next;
		int count;

		WaveData() :
			data(NULL),
			size(0),
			next(NULL),
			count(0)
		{}

		~WaveData()
		{
			delete[] data;
		}
	};

	enum class AlgorithmEnum
	{
		LMS = 0,
		NLMS = 1,
		RLS = 2
	};

	enum class SoundType
	{
		origin = 0,
		mix = 1,
		processed = 2
	};
	float *tmpRenderData;
	UINT tmpRenderFramesNum;
	float *tmpCaptureData;
	UINT tmpCaptureFramesNum;
	WaveData *RenderDataQueue;
	WaveData *RenderProcessDataQueue;
	WaveData *CurrentRenderData;
	WaveData *RenderDataTail;
	WaveData *CaptureDataQueue;
	WaveData *CaptureProcessDataQueue;
	WaveData *CaptureDataTail;
	WaveData *OutputDataQueue;
	WaveData *OutputDataTail;


	AlgorithmEnum selectAlgorithm;
	//  Header for a WAV file - we define a structure describing the first few fields in the header for convenience.
	//
	struct WAVEHEADER
	{
		DWORD   dwRiff;                     // "RIFF"
		DWORD   dwSize;                     // Size
		DWORD   dwWave;                     // "WAVE"
		DWORD   dwFmt;                      // "fmt "
		DWORD   dwFmtSize;                  // Wave Format Size
	};

	//  Static RIFF header, we'll append the format to it.
	const BYTE szWaveHeader[] =
	{
		'R',   'I',   'F',   'F',  0x00,  0x00,  0x00,  0x00, 'W',   'A',   'V',   'E',   'f',   'm',   't',   ' ', 0x00, 0x00, 0x00, 0x00
	};

	//  Static wave DATA tag.
	const BYTE szWaveData[] = { 'd', 'a', 't', 'a' };

	void Stop()
	{
		StopFlag = true;

	}

	LRESULT SaveToFile()
	{
		WaveData *cDataQueue[3] = { RenderDataQueue,CaptureDataQueue,OutputDataQueue };
		char *filename[3] = { "farEndOut.wav","nearEndIn.wav","nearEndOut.wav" };
		for (int loop = 1; loop < 3; loop++)
		{
			FILE *fp = fopen(filename[loop], "wb");
			WaveData *DataQueue = cDataQueue[loop];
			if (fp)
			{
				vector<float> Buffer;
				size_t BufferSize = 0;
				while (DataQueue->next != nullptr)
				{
					BufferSize += DataQueue->size;
					for (int i = 0; i < DataQueue->size; i++)
					{
						Buffer.push_back(DataQueue->data[i]);
						Buffer.push_back(DataQueue->data[i]);
					}
					//Buffer.insert(Buffer.end(), DataQueue->data, DataQueue->data + DataQueue->size);
					DataQueue = DataQueue->next;
				}
				BufferSize *= sizeof(float) * 2;
				DWORD waveFileSize = sizeof(WAVEHEADER) + sizeof(WAVEFORMATEX) + WaveFormat->cbSize + sizeof(szWaveData) + sizeof(DWORD) + static_cast<DWORD>(BufferSize);
				BYTE *waveFileData = new BYTE[waveFileSize];
				BYTE *waveFilePointer = waveFileData;
				WAVEHEADER *waveHeader = reinterpret_cast<WAVEHEADER *>(waveFileData);

				if (waveFileData == NULL)
				{
					printf("Unable to allocate %d bytes to hold output wave data\n", waveFileSize);
					return false;
				}

				//
				//  Copy in the wave header - we'll fix up the lengths later.
				//
				CopyMemory(waveFilePointer, szWaveHeader, sizeof(szWaveHeader));
				waveFilePointer += sizeof(szWaveHeader);

				//
				//  Update the sizes in the header.
				//
				waveHeader->dwSize = waveFileSize - (2 * sizeof(DWORD));
				waveHeader->dwFmtSize = sizeof(WAVEFORMATEX) + WaveFormat->cbSize;

				//
				//  Next copy in the WaveFormatex structure.
				//
				CopyMemory(waveFilePointer, WaveFormat, sizeof(WAVEFORMATEX) + WaveFormat->cbSize);
				waveFilePointer += sizeof(WAVEFORMATEX) + WaveFormat->cbSize;


				//
				//  Then the data header.
				//
				CopyMemory(waveFilePointer, szWaveData, sizeof(szWaveData));
				waveFilePointer += sizeof(szWaveData);
				*(reinterpret_cast<DWORD *>(waveFilePointer)) = static_cast<DWORD>(BufferSize);
				waveFilePointer += sizeof(DWORD);

				//
				//  And finally copy in the audio data.
				//
				CopyMemory(waveFilePointer, Buffer.data(), BufferSize);

				//
				//  Last but not least, write the data to the file.
				//
				DWORD bytesWritten;
				if (!(bytesWritten = fwrite(waveFileData, sizeof(BYTE), waveFileSize, fp)))
				{
					printf("Unable to write wave file: %d\n", GetLastError());
					delete[]waveFileData;
					return false;
				}

				//if (bytesWritten != waveFileSize)
				//{
				//	printf("Failed to write entire wave file\n");
				//	delete[]waveFileData;
				//	return false;
				//}
				delete[]waveFileData;
				fclose(fp);
			}
		}
		return S_OK;
	}

	HRESULT LoadRenderDataThread()
	{
		HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, false, szLoadRenderDataEvent);
		HANDLE hRenderEvent = OpenEvent(EVENT_ALL_ACCESS, false, szRenderThreadEvent);
		int position = 0;
		while (true)
		{
			if (StopFlag)
				goto StopLoadData;
			WaitForSingleObject(hEvent, INFINITE);
			EnterCriticalSection(&criticalSection);
			//static int position = 0;
			int renderDataPosition = 0;
			tmpRenderData = new float[tmpRenderFramesNum * 2];
			while (CurrentRenderData->next != nullptr && renderDataPosition < tmpRenderFramesNum)
			{
				if (CurrentRenderData->size == 0)
				{
					CurrentRenderData = CurrentRenderData->next;
					continue;
				}
				if (position == -1)
				{
					position = 0;
					CurrentRenderData = CurrentRenderData->next;
				}
				for (; position < CurrentRenderData->size && renderDataPosition < tmpRenderFramesNum; position++, renderDataPosition++)
				{
					tmpRenderData[renderDataPosition * 2] = CurrentRenderData->data[position];
					tmpRenderData[renderDataPosition * 2 + 1] = CurrentRenderData->data[position];
				}
				if (position == CurrentRenderData->size)
					position = -1;
				if (CurrentRenderData->count == 200)
					Stop();

			}
			PulseEvent(hRenderEvent);
			//tmpRenderFramesNum = 20000;
			LeaveCriticalSection(&criticalSection);

		}
	StopLoadData:
		return S_OK;
	}

	HRESULT SaveCaptureDataThread()
	{
		/*HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, false, szSaveCaptureDataEvent);
		while (true)
		{
		if (StopFlag)
		return S_OK;
		WaitForSingleObject(hEvent, INFINITE);
		EnterCriticalSection(&criticalSection);*/
		CaptureDataTail->next = new WaveData();
		CaptureDataTail->next->count = CaptureDataTail->count + 1;
		//首节点不包含数据
		CaptureDataTail = CaptureDataTail->next;
		CaptureDataTail->data = new float[tmpCaptureFramesNum];
		for (int i = 0; i < tmpCaptureFramesNum; i++)
		{
			CaptureDataTail->data[i] = tmpCaptureData[i * 2];
		}
		CaptureDataTail->size = tmpCaptureFramesNum;
		delete[]tmpCaptureData;

		/*LeaveCriticalSection(&criticalSection);

		}*/
		return S_OK;
	}

	HRESULT RenderThread()
	{
		HRESULT hr;
		REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC / 10;
		IMMDeviceEnumerator *pEnumerator = NULL;
		IMMDevice *pDevice = NULL;
		IAudioClient *pAudioClient = NULL;
		IAudioRenderClient *pRenderClient = NULL;
		WAVEFORMATEX *pwfx = NULL;
		UINT32 bufferFrameCount;
		UINT32 numFramesAvailable;
		UINT32 numFramesPadding;
		BYTE *pData;
		DWORD flags = 0;

		HANDLE hEvent = CreateEvent(NULL, true, false, szLoadRenderDataEvent);
		HANDLE hRenderEvent = CreateEvent(NULL, true, false, szRenderThreadEvent);
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LoadRenderDataThread, NULL, 0, 0);

		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&pEnumerator);
		EXIT_ON_ERROR(hr);

		hr = pEnumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, &pDevice);
		EXIT_ON_ERROR(hr);

		hr = pDevice->Activate(
			IID_IAudioClient, CLSCTX_ALL,
			NULL, (void**)&pAudioClient);
		EXIT_ON_ERROR(hr);

		hr = pAudioClient->GetMixFormat(&pwfx);
		EXIT_ON_ERROR(hr);

		hr = pAudioClient->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			0,
			hnsRequestedDuration,
			0,
			pwfx,
			NULL);
		EXIT_ON_ERROR(hr);

		// Get the actual size of the allocated buffer.
		hr = pAudioClient->GetBufferSize(&bufferFrameCount);
		EXIT_ON_ERROR(hr);

		hr = pAudioClient->GetService(
			IID_IAudioRenderClient,
			(void**)&pRenderClient);
		EXIT_ON_ERROR(hr);

		// Grab the entire buffer for the initial fill operation.
		hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
		EXIT_ON_ERROR(hr);

		// Load the initial data into the shared buffer.
		EnterCriticalSection(&criticalSection);
		tmpRenderFramesNum = bufferFrameCount;
		LeaveCriticalSection(&criticalSection);
		PulseEvent(hEvent);
		WaitForSingleObject(hRenderEvent, 10);
		EnterCriticalSection(&criticalSection);
		CopyMemory(pData, tmpRenderData, tmpRenderFramesNum * 8);
		delete[] tmpRenderData;
		LeaveCriticalSection(&criticalSection);

		EXIT_ON_ERROR(hr);

		hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
		EXIT_ON_ERROR(hr);

		hr = pAudioClient->Start();  // Start playing.
		EXIT_ON_ERROR(hr);

		// Each loop fills about half of the shared buffer.
		while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
		{

			// See how much buffer space is available.
			hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
			EXIT_ON_ERROR(hr);

			numFramesAvailable = bufferFrameCount - numFramesPadding;
			// Grab all the available space in the shared buffer.
			hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
			EXIT_ON_ERROR(hr);

			// Get next 1/2-second of data from the audio source.
			EnterCriticalSection(&criticalSection);
			tmpRenderFramesNum = numFramesAvailable;
			LeaveCriticalSection(&criticalSection);
			PulseEvent(hEvent);
			WaitForSingleObject(hRenderEvent, 10);
			EnterCriticalSection(&criticalSection);
			CopyMemory(pData, tmpRenderData, tmpRenderFramesNum * 8);
			delete[] tmpRenderData;
			LeaveCriticalSection(&criticalSection);
			EXIT_ON_ERROR(hr);

			hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
			if (StopFlag)
			{
				goto RenderStop;
			}
			Sleep(50);
			EXIT_ON_ERROR(hr);
		}

	RenderStop:
		hr = pAudioClient->Stop();  // Stop playing.
		EXIT_ON_ERROR(hr);

		WaitForSingleObject(hThread, INFINITE);

		CoTaskMemFree(pwfx);
		SAFE_RELEASE(pEnumerator)
			SAFE_RELEASE(pDevice)
			SAFE_RELEASE(pAudioClient)
			SAFE_RELEASE(pRenderClient)

			return hr;
	}

	HRESULT CaptureThread()
	{
		HRESULT hr;
		REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
		UINT32 bufferFrameCount;
		UINT32 numFramesAvailable;
		IMMDeviceEnumerator *pEnumerator = NULL;
		IMMDevice *pDevice = NULL;
		IAudioClient *pAudioClient = NULL;
		IAudioCaptureClient *pCaptureClient = NULL;
		WAVEFORMATEX *pwfx = NULL;
		UINT32 packetLength = 0;
		BOOL bDone = FALSE;
		BYTE *pData;
		DWORD flags;

		HANDLE hEvent = CreateEvent(NULL, true, false, szSaveCaptureDataEvent);
		//HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SaveCaptureDataThread, NULL, 0, 0);
		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&pEnumerator);
		EXIT_ON_ERROR(hr);

		hr = pEnumerator->GetDefaultAudioEndpoint(
			eCapture, eConsole, &pDevice);
		EXIT_ON_ERROR(hr);

		hr = pDevice->Activate(
			IID_IAudioClient, CLSCTX_ALL,
			NULL, (void**)&pAudioClient);
		EXIT_ON_ERROR(hr);

		hr = pAudioClient->GetMixFormat(&pwfx);
		EXIT_ON_ERROR(hr);

		hr = pAudioClient->GetMixFormat(&WaveFormat);
		EXIT_ON_ERROR(hr);

		hr = pAudioClient->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			0,
			hnsRequestedDuration,
			0,
			pwfx,
			NULL);
		EXIT_ON_ERROR(hr);

		// Get the size of the allocated buffer.
		hr = pAudioClient->GetBufferSize(&bufferFrameCount);
		EXIT_ON_ERROR(hr);

		hr = pAudioClient->GetService(
			IID_IAudioCaptureClient,
			(void**)&pCaptureClient);
		EXIT_ON_ERROR(hr);

		// Notify the audio sink which format to use.
		//hr = pMySink->SetFormat(pwfx);
		EXIT_ON_ERROR(hr);

		hr = pAudioClient->Start();  // Start recording.
		EXIT_ON_ERROR(hr);

		// Each loop fills about half of the shared buffer.
		while (bDone == FALSE)
		{
			// Sleep for half the buffer duration.
			if (StopFlag)
			{
				goto CaptureStop;
			}
			Sleep(100);

			hr = pCaptureClient->GetNextPacketSize(&packetLength);
			EXIT_ON_ERROR(hr);

			while (packetLength != 0)
			{
				// Get the available data in the shared buffer.
				hr = pCaptureClient->GetBuffer(
					&pData,
					&numFramesAvailable,
					&flags, NULL, NULL);
				EXIT_ON_ERROR(hr);

				if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
				{
					pData = NULL;  // Tell CopyData to write silence.
				}

				// Copy the available capture data to the audio sink.
				EnterCriticalSection(&criticalSection);
				tmpCaptureData = new float[numFramesAvailable * 2];
				tmpCaptureFramesNum = numFramesAvailable;
				CopyMemory(tmpCaptureData, pData, numFramesAvailable * 8);
				SaveCaptureDataThread();
				LeaveCriticalSection(&criticalSection);
				PulseEvent(hEvent);
				EXIT_ON_ERROR(hr);

				hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
				EXIT_ON_ERROR(hr);
				hr = pCaptureClient->GetNextPacketSize(&packetLength);
				EXIT_ON_ERROR(hr);
			}

		}
	CaptureStop:
		hr = pAudioClient->Stop();  // Stop recording.
		EXIT_ON_ERROR(hr);

		CoTaskMemFree(pwfx);
		SAFE_RELEASE(pEnumerator)
			SAFE_RELEASE(pDevice)
			SAFE_RELEASE(pAudioClient)
			SAFE_RELEASE(pCaptureClient)

			return hr;
	}

	LRESULT ProcessThread()
	{


		int iter = 50;
		float *nearEnd_f = new float[audioLength5s + iter];
		float *farEnd_new = new float[audioLength5s + iter];
		float *farEnd_old = new float[audioLength5s + iter];
	BeginProcess:
		for (int i = 0; i < iter; i++)
		{
			nearEnd_f[i] = 0;
			farEnd_new[i] = 0;
		}
		FloatArray  *farEnd;
		FloatArray  *nearEnd;
		FloatArray *echo, *m, *en;
	ContinueProcess:
		if (StopFlag)
			goto StopProcess;
		EnterCriticalSection(&criticalSection);
		if (RenderProcessDataQueue->next != nullptr && CaptureProcessDataQueue->next != nullptr)
		{

			//TODO 更新指针

			while (captureProcessDataNum < audioLength5s)
			{
				if (CaptureProcessDataQueue->next != nullptr)
					CaptureProcessDataQueue = CaptureProcessDataQueue->next;
				else
				{
					LeaveCriticalSection(&criticalSection);
					goto ContinueProcess;
				}
				int copySize = 0;
				memcpy(nearEnd_f + iter + captureProcessDataNum, CaptureProcessDataQueue->data + captureProcessPosition, copySize = sizeof(float) * min(CaptureProcessDataQueue->size - captureProcessPosition, audioLength5s - captureProcessDataNum));
				copySize /= 4;
				if (copySize < CaptureProcessDataQueue->size - captureProcessPosition)
					captureProcessPosition += copySize;
				else
					captureProcessPosition = 0;

				captureProcessDataNum += copySize;

			}

			while (renderProcessDataNum < audioLength5s)
			{
				if (RenderProcessDataQueue->next != nullptr)
					RenderProcessDataQueue = RenderProcessDataQueue->next;
				else
				{
					LeaveCriticalSection(&criticalSection);
					goto ContinueProcess;
				}
				int copySize = 0;
				memcpy(farEnd_new + iter + renderProcessDataNum, RenderProcessDataQueue->data + renderProcessPosition, copySize = sizeof(float)*min(RenderProcessDataQueue->size - renderProcessPosition, audioLength5s - renderProcessDataNum));
				copySize /= 4;
				if (copySize < RenderProcessDataQueue->size - renderProcessPosition)
					renderProcessPosition += copySize;
				else
					renderProcessPosition = 0;

				renderProcessDataNum += copySize;

			}
			LeaveCriticalSection(&criticalSection);
		}
		else
		{
			LeaveCriticalSection(&criticalSection);
			Sleep(100);
			goto ContinueProcess;
		}
		captureProcessDataNum = 0;
		renderProcessDataNum = 0;
		farEnd = CreateFloatArray(farEnd_new, audioLength5s + iter, 1);
		nearEnd = CreateFloatArray(nearEnd_f, audioLength5s + iter, 1);
		int delay = delayEstimation(farEnd, nearEnd);
		emxDestroyArray_real32_T(farEnd);
		emxDestroyArray_real32_T(nearEnd);
		float *farEnd_tmp = new float[audioLength5s + iter];
		float *echo_f = new float[audioLength5s + iter];
		float *m_f = new float[audioLength5s + iter];
		float *en_f = new float[audioLength5s + iter];
		ZeroMemory(echo_f, sizeof(float)*(audioLength5s + iter));
		ZeroMemory(m_f, sizeof(float)*(audioLength5s + iter));
		ZeroMemory(en_f, sizeof(float)*(audioLength5s + iter));
		cout << delay << endl;
		//delay = 5000;
		if (delay > 0)
		{
			CopyMemory(farEnd_tmp, farEnd_old + audioLength5s + iter - delay, delay * sizeof(float));
			CopyMemory(farEnd_tmp + delay, farEnd_new, sizeof(float)*(audioLength5s + iter - delay));
			farEnd = CreateFloatArray(farEnd_tmp, 1, audioLength5s + iter);
			nearEnd = CreateFloatArray(nearEnd_f, 1, audioLength5s + iter);

			echo = CreateFloatArray(echo_f, 1, audioLength5s + iter);
			m = CreateFloatArray(m_f, 1, iter);
			en = CreateFloatArray(en_f, audioLength5s + iter, 1);

			NLMS(nearEnd, farEnd, iter, 1, 0, echo, m, en);
		}

		EnterCriticalSection(&criticalSection);
		OutputDataTail->size = audioLength5s;
		OutputDataTail->data = new float[audioLength5s];
		for (int i = 0; i < audioLength5s; i++)
		{
			OutputDataTail->data[i] = nearEnd_f[i + iter] - echo_f[i + iter];
		}
		OutputDataTail->next = new WaveData();
		OutputDataTail->next->count = OutputDataTail->count + 1;
		OutputDataTail = OutputDataTail->next;

		LeaveCriticalSection(&criticalSection);
		CopyMemory(farEnd_old, farEnd_new, sizeof(float)*(audioLength5s + iter));
	StopProcess:

		if (StopFlag)
		{
			//SaveToFile();
			delete[]farEnd_old;
			delete[]farEnd_new;
			delete[]nearEnd_f;
			return S_OK;
		}

		goto BeginProcess;
		return S_OK;
	}




	void LoadFromFile(CString fileName, CString echoFile = L"")
	{
		if (echoFile == L"")
		{
			FILE *fp_far = _wfopen(fileName, L"rb");
			short *far_frame = new short[44];
			fread(far_frame, sizeof(char), 44, fp_far);
			long fileBegin = ftell(fp_far);
			delete[]far_frame;
			fseek(fp_far, 0, SEEK_END);
			long fileEnd = ftell(fp_far);
			int fileLength = (fileEnd - fileBegin) / 2;
			far_frame = new short[audioLength100ms];
			fseek(fp_far, 44, SEEK_SET);
			int loopCount = 0;
			while (!feof(fp_far) && loopCount < 200)
			{
				RenderDataTail->next = new WaveData();
				RenderDataTail->next->count = RenderDataTail->count + 1;
				RenderDataTail = RenderDataTail->next;
				fread(far_frame, sizeof(short), audioLength100ms, fp_far);
				RenderDataTail->size = audioLength100ms;
				float *fTmp = new float[audioLength100ms];
				for (int i = 0; i < audioLength100ms; i++)
				{
					fTmp[i] = far_frame[i] / float(65536 / 2);
				}
				RenderDataTail->data = fTmp;
				loopCount++;
			}
			fclose(fp_far);
		}
		else
		{
			float delta = 1;
			if (fileName == echoFile)
			{
				delta = 0.9;
			}
			int delay = 0;
			if (randonDelayFlag)
			{
				time_t ThisTime;
				time(&ThisTime);
				srand(ThisTime);
				delay = minRandonDelayMs + rand() % (maxRandonDelayMs - minRandonDelayMs);
			}
			else
			{
				delay = delayMs;
			}
			FILE *fp_far = _wfopen(echoFile, L"rb");
			FILE *fp_near = _wfopen(fileName, L"rb");
			FILE *fp_mix = _wfopen(L"mix.bin", L"wb");
			FILE *fp_tmp = _wfopen(L"tmp.bin", L"wb");
			short *far_frame = new short[44];
			short *near_frame = new short[44];
			fread(far_frame, sizeof(char), 44, fp_far);
			fread(near_frame, sizeof(char), 44, fp_near);
			long farFileBegin = ftell(fp_far);
			long nearFileBegin = ftell(fp_near);
			delete[] far_frame;
			delete[] near_frame;
			fseek(fp_far, 0, SEEK_END);
			fseek(fp_near, 0, SEEK_END);
			long farFileEnd = ftell(fp_far);
			long nearFileEnd = ftell(fp_near);
			int farFileLength = (farFileEnd - farFileBegin) / 2;
			int nearFileLength = (nearFileEnd - nearFileBegin) / 2;
			//将远端音频写入临时文件
			far_frame = new short[delay * audioLength1ms];
			RtlZeroMemory(far_frame, delay * audioLength1ms * sizeof(short));
			fwrite(far_frame, sizeof(short), delay * audioLength1ms, fp_tmp);
			delete[] far_frame;
			far_frame = new short[audioLength1s * 30];
			fseek(fp_far, 44, SEEK_SET);
			fread(far_frame, sizeof(short), audioLength1s * 30, fp_far);
			fwrite(far_frame, sizeof(short), audioLength1s * 30, fp_tmp);
			delete[] far_frame;
			fclose(fp_tmp);
			fp_tmp = _wfopen(L"tmp.bin", L"rb");

			far_frame = new short[audioLength100ms];
			near_frame = new short[audioLength100ms];
			//fseek(fp_far, 44 + delay * audioLength1ms * 2, SEEK_SET);

			fseek(fp_near, 44, SEEK_SET);
			int loopCount = 0;
			while (!feof(fp_far) && loopCount < 200)
			{
				RenderDataTail->next = new WaveData();
				RenderDataTail->next->count = RenderDataTail->count + 1;
				RenderDataTail = RenderDataTail->next;
				fread(far_frame, sizeof(short), audioLength100ms, fp_tmp);
				fread(near_frame, sizeof(short), audioLength100ms, fp_near);
				RenderDataTail->size = audioLength100ms;
				float *fTmp = new float[audioLength100ms];
				for (int i = 0; i < audioLength100ms; i++)
				{
					fTmp[i] = (far_frame[i] + near_frame[i] * delta) / float(65536 / 2);
				}
				RenderDataTail->data = fTmp;
				loopCount++;
				fwrite(fTmp, sizeof(float), audioLength100ms, fp_mix);
			}
			fclose(fp_far);
			fclose(fp_near);
			fclose(fp_mix);
			fclose(fp_tmp);
		}

		//FILE *fp_far = fopen("farEnd.wav", "rb");
		//float *far_frame = new float[68];
		//fread(far_frame, sizeof(char), 68, fp_far);
		//long farFileBegin = ftell(fp_far);
		//delete[]far_frame;
		//fseek(fp_far, 0, SEEK_END);
		//long farFileEnd = ftell(fp_far);
		//int farFileLength = (farFileEnd - farFileBegin) / 2;
		//far_frame = new float[audioLength100ms * 2];
		//fseek(fp_far, 68, SEEK_SET);
		//while (!feof(fp_far))
		//{
		//	RenderDataTail->next = new WaveData();
		//	RenderDataTail->next->count = RenderDataTail->count + 1;
		//	RenderDataTail = RenderDataTail->next;
		//	fread(far_frame, sizeof(float), audioLength100ms * 2, fp_far);
		//	RenderDataTail->size = audioLength100ms;
		//	float *fTmp = new float[audioLength100ms];
		//	for (int i = 0; i < audioLength100ms; i++)
		//	{
		//		fTmp[i] = far_frame[i * 2];
		//	}
		//	RenderDataTail->data = fTmp;
		//}
		//fclose(fp_far);
	}


	void RenderProcess()
	{
		HANDLE hRenderThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RenderThread, NULL, 0, 0);
		WaitForSingleObject(hRenderThread, INFINITE);
		Sleep(100);
		DeleteCriticalSection(&criticalSection);
		while (RenderDataQueue != nullptr)
		{
			WaveData* tmp = RenderDataQueue;
			RenderDataQueue = RenderDataQueue->next;
			delete tmp;
		}
	}

	int DoProcess(CString fileName)
	{
		FILE *fp_far = _wfopen(fileName, L"rb");
		FILE *fp_mix = _wfopen(L"mix.bin", L"rb");
		fseek(fp_far, 44, SEEK_SET);
		short *frame_far = new short[audioLength1s * 20];
		fread(frame_far, sizeof(short), audioLength1s * 20, fp_far);
		float *fFrame_far = new float[audioLength1s * 20];
		for (int i = 0; i < audioLength1s * 20; i++)
		{
			fFrame_far[i] = frame_far[i] / float(65536 / 2);
		}
		float *fFrame_mix = new float[audioLength1s * 20];
		fread(fFrame_mix, sizeof(float), audioLength1s * 20, fp_mix);


		delayEstimation_initialize();
		FloatArray  *farEnd;
		FloatArray  *nearEnd;
		FloatArray *echo, *m, *en;
		int iter = 50;
		farEnd = CreateFloatArray(fFrame_far, audioLength1s * 20, 1);
		nearEnd = CreateFloatArray(fFrame_mix, audioLength1s * 20, 1);
		int delay = delayEstimation(farEnd, nearEnd);
		emxDestroyArray_real32_T(farEnd);
		emxDestroyArray_real32_T(nearEnd);
		float *echo_f = new float[audioLength1s * 20];
		float *m_f = new float[audioLength1s * 20];
		float *en_f = new float[audioLength1s * 20];
		float *farEnd_tmp = new float[audioLength1s * 20];
		ZeroMemory(echo_f, sizeof(float)*(audioLength1s * 20));
		ZeroMemory(m_f, sizeof(float)*(audioLength1s * 20));
		ZeroMemory(en_f, sizeof(float)*(audioLength1s * 20));
		ZeroMemory(farEnd_tmp, sizeof(float)*(audioLength1s * 20));

		CopyMemory(farEnd_tmp + delay, fFrame_far, sizeof(float)*(audioLength1s * 20 - delay));
		

		
		if (selectAlgorithm == AlgorithmEnum::LMS)
		{
			farEnd = CreateFloatArray(farEnd_tmp, 1, audioLength1s * 20);
			nearEnd = CreateFloatArray(fFrame_mix, 1, audioLength1s * 20);
			echo = CreateFloatArray(echo_f, 1, audioLength1s * 20);
			m = CreateFloatArray(m_f, 1, iter);
			en = CreateFloatArray(en_f, audioLength1s * 20, 1);
			LMS(nearEnd, farEnd, iter, 0.005, 0, echo, m, en);
		}
		else if (selectAlgorithm == AlgorithmEnum::NLMS)
		{
			farEnd = CreateFloatArray(farEnd_tmp, 1, audioLength1s * 20);
			nearEnd = CreateFloatArray(fFrame_mix, 1, audioLength1s * 20);
			echo = CreateFloatArray(echo_f, 1, audioLength1s * 20);
			m = CreateFloatArray(m_f, 1, iter);
			en = CreateFloatArray(en_f, audioLength1s * 20, 1);
			NLMS(nearEnd, farEnd, iter, 1, 0, echo, m, en);
		}
		else
		{
			farEnd = CreateFloatArray(farEnd_tmp, audioLength1s * 20, 1);
			nearEnd = CreateFloatArray(fFrame_mix, audioLength1s * 20,1);
			echo = CreateFloatArray(echo_f, audioLength1s * 20, 1);
			m = CreateFloatArray(m_f, 1, iter);
			en = CreateFloatArray(en_f, audioLength1s * 20, 1);
			RLS(nearEnd, farEnd, 50, 0, echo, m, en);
		}
		RenderDataTail->next = new WaveData();
		RenderDataTail->next->count = RenderDataTail->count + 1;
		RenderDataTail = RenderDataTail->next;
		RenderDataTail->size = audioLength1s * 20;
		RenderDataTail->data = new float[audioLength1s * 20];
		for (int i = 0; i < audioLength1s * 20; i++)
		{
			RenderDataTail->data[i] = fFrame_mix[i] - echo_f[i];
		}
		RenderDataTail->next = new WaveData();
		RenderDataTail->next->count = RenderDataTail->count + 1;
		delayEstimation_terminate();
		fclose(fp_far);
		fclose(fp_mix);

		return delay;
	}

	int Play(CString fileName, SoundType flag = SoundType::origin, CString echoFileName = L"")
	{
		//fileName为近端音频，echoFileName为远端音频

		StopFlag = false;
		//CaptureDataTail = new WaveData();

		//CaptureDataQueue = CaptureDataTail;
		//CaptureProcessDataQueue = CaptureDataTail;
		//RenderDataTail = CaptureDataQueue;
		RenderDataTail = new WaveData();
		RenderDataQueue = RenderDataTail;
		//RenderProcessDataQueue = RenderDataTail;
		CurrentRenderData = RenderDataTail;

		int delay = 0;
		//OutputDataTail = new WaveData();
		//OutputDataQueue = OutputDataTail;
		if (flag == SoundType::origin)
		{
			LoadFromFile(fileName);
		}
		else if (flag == SoundType::mix)
		{
			LoadFromFile(fileName, echoFileName);
		}
		else
		{
			delay = DoProcess(echoFileName);
		}
		InitializeCriticalSection(&criticalSection);

		HANDLE hRenderThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RenderProcess, NULL, 0, 0);
		//HANDLE hCaptureThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CaptureThread, NULL, 0, 0);
		//HANDLE hProcessThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessThread, NULL, 0, 0);
		//HANDLE hStopThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Stop, NULL, 0, 0);

		//WaitForSingleObject(hRenderThread, INFINITE);
		//Sleep(100);
		////SaveToFile();
		//DeleteCriticalSection(&criticalSection);


		return delay;
	}


}