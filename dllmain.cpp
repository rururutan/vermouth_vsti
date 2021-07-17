#include "compiler.h"
#include "dllmain.h"
#include <shlwapi.h>
#include "commng\cmmidioutvst.h"
#include "commng\vsthost\vsteditwnd.h"

#pragma comment(lib, "shlwapi.lib")

	HWND		hWndMain;
	HINSTANCE	hInst;

static CComMidiOutVst* s_pVst;

#define MIDI_BUFF_SIZE (4096)

#define MIDI_STATE_READY		0
#define MIDI_STATE_DATA2		1
#define MIDI_STATE_DATA3		2
#define MIDI_STATE_EXCLUSIVE	3
#define MIDI_STATE_TIMECODE		4
#define MIDI_STATE_SYSTEMDATA1	5
#define MIDI_STATE_SYSTEMDATA2	6
#define MIDI_STATE_SYSTEMDATA3	7

static int midi_state;
static unsigned char midi_buff[MIDI_BUFF_SIZE];
static int midi_buff_ptr;
static unsigned char midi_last_data;

#define MIDIOUTS(a, b, c)	(((c) << 16) + (b << 8) + (a))
#define MIDIOUTS2(a)		(*(UINT16 *)(a))
#define MIDIOUTS3(a)		((*(UINT32 *)(a)) & 0xffffff)

static void GetPath(LPTSTR lpModule, UINT cbModule)
{
	TCHAR szModule[MAX_PATH];
	::GetModuleFileName(hInst, szModule, _countof(szModule));
	::PathRenameExtension(szModule, TEXT(".ini"));

	TCHAR szVst[MAX_PATH];
	::GetPrivateProfileString(TEXT("VST"), TEXT("Module"), TEXT("%ProgramFiles%\\Roland\\Sound Canvas VA\\SOUND Canvas VA.dll"), szVst, _countof(szVst), szModule);

	::ExpandEnvironmentStrings(szVst, lpModule, cbModule);
}

DLLAPI int WINAPI vermouth_Open(UINT sample_rate) {

	TCHAR szModule[MAX_PATH];
	GetPath(szModule, _countof(szModule));

	CComMidiOutVst* pVst = new CComMidiOutVst;
	if (!pVst->Initialize(szModule, sample_rate))
	{
		delete pVst;
		return -1;
	}
	s_pVst = pVst;
	return 0;
}

DLLAPI int WINAPI vermouth_Close(void) {

	if (s_pVst)
	{
		CComMidiOutVst* pVst = s_pVst;
		s_pVst = NULL;
		delete pVst;
		return 0;
	}
	else
	{
		return -1;
	}
}

DLLAPI int WINAPI vermouth_Write(UINT8 data) {
	
	if (s_pVst == NULL)
	{
		return -1;
	}

	switch(data) {
		case 0xf8: // TIMING
		case 0xfa: // START
		case 0xfb: // CONTINUE
		case 0xfc: // STOP
		case 0xfe: // ACTIVE SENSING
		case 0xff: // SYSTEM RESET
			return(-1);
	}

	if ((midi_state == MIDI_STATE_READY) ||
		((midi_state == MIDI_STATE_DATA2) && (data & 0x80)) ||
		((midi_state == MIDI_STATE_DATA3) && (data & 0x80))) {
		midi_buff_ptr = 0;
		if (data & 0x80) {
			midi_last_data = data;
			switch(data & 0xf0) {
				case 0xc0: // PROGRAM CHANGE
				case 0xd0: // AFTER TOUCH
					midi_state = MIDI_STATE_DATA2;
					break;

				case 0x80: // NOTE OFF
				case 0x90: // NOTE ON
				case 0xa0: // POLYPHONIC KEY PRESSURE
				case 0xb0: // CONTROL CHANGE
				case 0xe0: // PITCH BENDER
					midi_state = MIDI_STATE_DATA3;
					break;

				default:
					switch(data) {
						case 0xf0: // EXCLUSIVE
							midi_state = MIDI_STATE_EXCLUSIVE;
							break;

						case 0xf1: // TIME CODE
							midi_state = MIDI_STATE_TIMECODE;
							break;

						case 0xf2: // SONG POS
							midi_state = MIDI_STATE_SYSTEMDATA3;
							break;

						case 0xf3: // SONG SELECT
							midi_state = MIDI_STATE_SYSTEMDATA2;
							break;

						case 0xf6: // TUNE REQUEST
							midi_state = MIDI_STATE_SYSTEMDATA1;
							break;

						default:
							return(-1);
					}
					break;
			}
		}
		else {
			midi_buff[midi_buff_ptr++] = midi_last_data;
			midi_state = MIDI_STATE_DATA3;
		}
	}

	midi_buff[midi_buff_ptr++] = data;

	switch (midi_state) {
		case MIDI_STATE_DATA2:
			if (midi_buff_ptr >= 2) {
				s_pVst->Short(MIDIOUTS2(midi_buff));
				midi_state = MIDI_STATE_READY;
			}
			break;

		case MIDI_STATE_DATA3:
			if (midi_buff_ptr >= 3) {
				s_pVst->Short(MIDIOUTS3(midi_buff));
				midi_state = MIDI_STATE_READY;
			}
			break;

		case MIDI_STATE_EXCLUSIVE:
			if (data == 0xf7) {
				s_pVst->Long(midi_buff, midi_buff_ptr);
				midi_state = MIDI_STATE_READY;
			}
			else if (midi_buff_ptr >= MIDI_BUFF_SIZE) {
				midi_state = MIDI_STATE_READY;
			}
			break;

		case MIDI_STATE_SYSTEMDATA1:
			if (midi_buff_ptr >= 1) {
				midi_state = MIDI_STATE_READY;
			}
			break;

		case MIDI_STATE_SYSTEMDATA2:
			if (midi_buff_ptr >= 2) {
				midi_state = MIDI_STATE_READY;
			}
			break;

		case MIDI_STATE_SYSTEMDATA3:
			if (midi_buff_ptr >= 3) {
				midi_state = MIDI_STATE_READY;
			}
			break;
	}
	return(0);
}

DLLAPI int WINAPI vermouth_ShortMsg(UINT32 data) {

	if (s_pVst) {
		s_pVst->Short(data);
		return(0);
	}
	else {
		return(-1);
	}
}

DLLAPI int WINAPI vermouth_LongMsg(const UINT8 *sysex, UINT32 len) {

	if (s_pVst) {
		s_pVst->Long(sysex, len);
		return(0);
	}
	else {
		return(-1);
	}
}

DLLAPI int WINAPI vermouth_Get16(SINT16 *pcm, UINT32 size) {

	if (s_pVst) {
		s_pVst->Process16(pcm, size);
		return(0);
	}
	else {
		return(-1);
	}
}

DLLAPI int WINAPI vermouth_Get32(SINT32 *pcm, UINT32 size) {

	if (s_pVst) {
		s_pVst->Process32(pcm, size);
		return(0);
	}
	else {
		return(-1);
	}
}


// ----

DLLAPI HWND WINAPI vermouth_SetParent(HWND hWnd) {

	HWND ret = hWndMain;
	hWndMain = hWnd;
	return(ret);
}


// ----

static void dllinitialize(HINSTANCE hInstance) {

	hWndMain = NULL;
	hInst = hInstance;
	CVstEditWnd::Initialize(hInstance);
}

static void dlldeinitialize()
{
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {

	switch(dwReason) {
		case DLL_PROCESS_ATTACH:
			dllinitialize(hInstance);
			break;

		case DLL_PROCESS_DETACH:
			dlldeinitialize();
			break;
	}
	return(TRUE);
}

