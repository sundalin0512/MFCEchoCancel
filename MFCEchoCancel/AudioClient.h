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
	const int audioLength5s = 48000 * 2;
	const int audioLength1s = 48000;
	const int audioLength500ms = 24000;
	const int audioLength200ms = 9600;
	const int audioLength100ms = 4800;

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
				BYTE *waveFileData = new (std::nothrow) BYTE[waveFileSize];
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
		while (true)
		{
			if (StopFlag)
				goto StopLoadData;
			WaitForSingleObject(hEvent, INFINITE);
			EnterCriticalSection(&criticalSection);
			static int position = 0;
			int renderDataPosition = 0;
			tmpRenderData = new float[tmpRenderFramesNum * 2];
			while (CurrentRenderData->next != nullptr && renderDataPosition < tmpRenderFramesNum)
			{
				if (position == 0)
					CurrentRenderData = CurrentRenderData->next;
				for (; position < CurrentRenderData->size && renderDataPosition < tmpRenderFramesNum; position++, renderDataPosition++)
				{
					tmpRenderData[renderDataPosition * 2] = CurrentRenderData->data[position];
					tmpRenderData[renderDataPosition * 2 + 1] = CurrentRenderData->data[position];
				}
				if (position == CurrentRenderData->size)
					position = 0;

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

	void Stop()
	{
		char ch;
		scanf("%c", &ch);
		StopFlag = true;
	}


	void LoadFromFile()
	{
		//FILE *fp_far = fopen("farEnd.wav", "rb");
		//short *far_frame = new short[44];
		//fread(far_frame, sizeof(char), 44, fp_far);
		//long fileBegin = ftell(fp_far);
		//delete[]far_frame;
		//fseek(fp_far, 0, SEEK_END);
		//long fileEnd = ftell(fp_far);
		//int fileLength = (fileEnd - fileBegin) / 2;
		//far_frame = new short[audioLength100ms];
		//fseek(fp_far, 44, SEEK_SET);
		//while (!feof(fp_far))
		//{
		//	RenderDataTail->next = new WaveData();
		//	RenderDataTail->next->count = RenderDataTail->count + 1;
		//	RenderDataTail = RenderDataTail->next;
		//	fread(far_frame, sizeof(short), audioLength100ms, fp_far);
		//	RenderDataTail->size = audioLength100ms;
		//	float *fTmp = new float[audioLength100ms];
		//	for (int i = 0; i < audioLength100ms; i++)
		//	{
		//		fTmp[i] = far_frame[i] / float(65536 / 2);
		//	}
		//	RenderDataTail->data = fTmp;
		//}
		//fclose(fp_far);


		FILE *fp_far = fopen("farEnd.wav", "rb");
		float *far_frame = new float[68];
		fread(far_frame, sizeof(char), 68, fp_far);
		long fileBegin = ftell(fp_far);
		delete[]far_frame;
		fseek(fp_far, 0, SEEK_END);
		long fileEnd = ftell(fp_far);
		int fileLength = (fileEnd - fileBegin) / 2;
		far_frame = new float[audioLength100ms * 2];
		fseek(fp_far, 68, SEEK_SET);
		while (!feof(fp_far))
		{
			RenderDataTail->next = new WaveData();
			RenderDataTail->next->count = RenderDataTail->count + 1;
			RenderDataTail = RenderDataTail->next;
			fread(far_frame, sizeof(float), audioLength100ms * 2, fp_far);
			RenderDataTail->size = audioLength100ms;
			float *fTmp = new float[audioLength100ms];
			for (int i = 0; i < audioLength100ms; i++)
			{
				fTmp[i] = far_frame[i * 2];
			}
			RenderDataTail->data = fTmp;
		}
		fclose(fp_far);
	}
	int init()
	{
		CaptureDataTail = new WaveData();

		CaptureDataQueue = CaptureDataTail;
		CaptureProcessDataQueue = CaptureDataTail;
		//RenderDataTail = CaptureDataQueue;
		RenderDataTail = new WaveData();
		RenderDataQueue = RenderDataTail;
		RenderProcessDataQueue = RenderDataTail;
		CurrentRenderData = RenderDataTail;

		OutputDataTail = new WaveData();
		OutputDataQueue = OutputDataTail;
		LoadFromFile();
		InitializeCriticalSection(&criticalSection);

		HANDLE hRenderThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RenderThread, NULL, 0, 0);
		HANDLE hCaptureThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CaptureThread, NULL, 0, 0);
		HANDLE hProcessThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessThread, NULL, 0, 0);
		HANDLE hStopThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Stop, NULL, 0, 0);

		WaitForSingleObject(hProcessThread, INFINITE);
		Sleep(100);
		SaveToFile();
		DeleteCriticalSection(&criticalSection);


		return 0;
	}


}