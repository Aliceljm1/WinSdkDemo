// ----------------------------------------------------------------------------
// 
// This file is part of the Microsoft COM+ Samples.
// 
// Copyright (C) 1995-2000 Microsoft Corporation. All rights reserved.
// 
// This source code is intended only as a supplement to Microsoft
// Development Tools and/or on-line documentation. See these other
// materials for detailed information regarding Microsoft code samples.
// 
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
// 
// ----------------------------------------------------------------------------

// ===========================================================================
// Description:
// 
//  This is the client-portion of the SIMPLE Distributed COM sample. This
// application uses the CLSID_SimpleObject class implemented by the SSERVER.CPP
// module. Pass the machine-name to instantiate the object on, or pass no
// arguments to instantiate the object on the same machine. See the comments
// in SSERVER.CPP for further details.
// 
//  The purpose of this sample is to demonstrate what is minimally required
// to use a COM object, whether it runs on the same machine or on a different
// machine.
// 
// Instructions:
// 
//  To use this sample:
//   * build it using the NMAKE command. NMAKE will create SSERVER.EXE and
//     SCLIENT.EXE.
//   * install the SSERVER.EXE on the current machine or on a remote machine
//     according to the installation instructions found in SSERVER.CPP.
//   * run SCLIENT.EXE. use no command-line arguments to instantiate the object
//     on the current machine. use a single command-line argument of the remote
//     machine-name (UNC or DNS) to instantiate the object on a remote machine.
//   * SCLIENT.EXE displays some simple information about the calls it is
//     making on the object.
// 
// Copyright 1996 Microsoft Corporation.  All Rights Reserved.
// ===========================================================================

// %%Includes: ---------------------------------------------------------------
#define INC_OLE2
#include <stdio.h>
#include <windows.h>
#include <initguid.h>
#include <tchar.h>
#include <conio.h>

// %%GUIDs: ------------------------------------------------------------------
DEFINE_GUID(CLSID_SimpleObject, 0x5e9ddec7, 0x5767, 0x11cf, 0xbe, 0xab, 0x0, 0xaa, 0x0, 0x6c, 0x36, 0x6);

//
// 下面是各接口的IID
//
// {32bb8320-b41b-11cf-a6bb-0080c7b2d682}
static const IID IID_IX = 
{0x32bb8320, 0xb41b, 0x11cf,
{0xa6, 0xbb, 0x0, 0x80, 0xc7, 0xb2, 0xd6, 0x82}};

// 接口定义
interface IX : IUnknown
{
	virtual void __stdcall Fx() = 0;
};


// %%Constants: --------------------------------------------------------------
const ULONG cbDefault = 4096;    // default # of bytes to Read/Write

// ---------------------------------------------------------------------------
// %%Function: Message
// 
//  Formats and displays a message to the console.
// ---------------------------------------------------------------------------
void
	Message(LPTSTR szPrefix, HRESULT hr)
{
	LPTSTR   szMessage;

	if (hr == S_OK)
	{
		wprintf(szPrefix);
		return;
	}

	if (HRESULT_FACILITY(hr) == FACILITY_WINDOWS)
		hr = HRESULT_CODE(hr);

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
		(LPTSTR)&szMessage,
		0,
		NULL);

	wprintf(TEXT("%s: %s(%lx)\n"), szPrefix, szMessage, hr);

	LocalFree(szMessage);
}  // Message

// ---------------------------------------------------------------------------
// %%Function: OutputTime
// ---------------------------------------------------------------------------
void
	OutputTime(LARGE_INTEGER* pliStart, LARGE_INTEGER* pliFinish)
{
	LARGE_INTEGER liFreq;

	QueryPerformanceFrequency(&liFreq);
	wprintf(TEXT("%0.4f seconds\n"),
		(float)(pliFinish->LowPart - pliStart->LowPart)/(float)liFreq.LowPart);
}  // OutputTime

// ---------------------------------------------------------------------------
// %%Function: main
// ---------------------------------------------------------------------------
void __cdecl
	main(int argc, CHAR **argv)
{
	HRESULT hr,hrx;
	MULTI_QI    mq[2];
	COSERVERINFO csi, *pcsi=NULL,*pcsix;
	WCHAR wsz [MAX_PATH];
	ULONG cb = cbDefault;
	LARGE_INTEGER liStart, liFinish;

	// allow a machine-name as the command-line argument
	if (argc > 1)
	{
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, argv[1], -1,
			wsz, sizeof(wsz)/sizeof(wsz[0]));
		memset(&csi, 0, sizeof(COSERVERINFO));
		csi.pwszName = wsz;
		pcsi = &csi;
	}

	// allow a byte-size to be passed on the command-line
	if (argc > 2)
		cb = atol(argv[2]);

	// initialize COM for free-threaded use
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		Message(TEXT("Client: CoInitializeEx"), hr);
		exit(hr);
	}

	// create a remote instance of the object on the argv[1] machine
	Message(TEXT("Client: Creating Instance..."), S_OK);
	mq[0].pIID = &IID_IStream;
	mq[0].pItf = NULL;
	mq[0].hr = S_OK;

	mq[1].pIID = &IID_IX;
	mq[1].pItf = NULL;
	mq[1].hr = S_OK;
	QueryPerformanceCounter(&liStart);
	hr = CoCreateInstanceEx(CLSID_SimpleObject, NULL, CLSCTX_SERVER, pcsi, 2, mq);//查询两个接口,但是第二个接口查询失败了，
	QueryPerformanceCounter(&liFinish);
	OutputTime(&liStart, &liFinish);

	if (FAILED(hr))
		Message(TEXT("Client: CoCreateInstanceEx"), hr);
	else
	{

		HRESULT hr;  
		IX* pIX= NULL;
		hr = CoCreateInstance(CLSID_SimpleObject, NULL, CLSCTX_SERVER ,
			IID_IX, (void **)& pIX);
		if(SUCCEEDED(hr))
		{
			if (pIX)
			{
				Message(TEXT("Client:获取接口IX成功."),S_OK);
				pIX->Fx();          // 使用 IX.
			}else{
				Message(TEXT("Client:get interface ix faile."),S_OK);
			}
		}


		LPVOID      pv;
		LPSTREAM    pstm = (IStream*)mq[0].pItf;
		// make prefast "happy"
		if (!pstm)
		{
			Message(TEXT("Client: NULL Interface pointer"),E_FAIL);
			exit(E_FAIL);
		}

		// "read" some data from the object
		Message(TEXT("Client: Reading data..."), S_OK);
		pv = CoTaskMemAlloc(cb);
		QueryPerformanceCounter(&liStart);
		hr = pstm->Read(pv, cb, NULL);
		QueryPerformanceCounter(&liFinish);
		OutputTime(&liStart, &liFinish);
		if (FAILED(hr))
			Message(TEXT("Client: IStream::Read"), hr);

		// "write" some data to the object
		Message(TEXT("Client: Writing data..."), S_OK);
		QueryPerformanceCounter(&liStart);
		hr = pstm->Write(pv, cb, NULL);
		QueryPerformanceCounter(&liFinish);
		OutputTime(&liStart, &liFinish);
		if (FAILED(hr))
			Message(TEXT("Client: IStream::Write"), hr);




		// let go of the object
		pstm->Release();
	}

	CoUninitialize();
	Message(TEXT("Client: Done"), S_OK);
}  // main

// EOF =======================================================================
