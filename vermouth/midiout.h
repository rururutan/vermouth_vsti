/**
 * @file	midiout.h
 */

#pragma once

#include <deque>

#define ENABLE_PORTB

#ifndef VERMOUTHCL
#define	VERMOUTHCL
#endif

struct _midimodule;
typedef	struct _midimodule	_MIDIMOD;
typedef	struct _midimodule	*MIDIMOD;

struct _midictrl;
typedef	struct _midictrl	_MIDIHDL;
typedef	struct _midictrl	*MIDIHDL;

class CComMidiOutVst;

enum {
	MIDIOUT_SUCCESS		= 0,
	MIDIOUT_FAILURE		= -1,
	MIDIOUT_ABORT		= -2
};

#include	"midimod.h"
#include	"midinst.h"

struct vsthdl
{
	MIDIHDL hdl;
	CComMidiOutVst* vst;
};

struct _midimodule {
	UINT		samprate;
	UINT		lockcount;
	std::deque<vsthdl> handles;
};

struct _midictrl {
	UINT		samprate;
	UINT		worksize;
	UINT8		status;
	SINT8		gain;
	UINT8		master;
	UINT8		moduleid;

	MIDIMOD		module;
	CComMidiOutVst* vst;

	SINT32		*sampbuf;

#if defined(ENABLE_PORTB)
	MIDIHDL		portb;
#endif	// defined(ENABLE_PORTB);
};

#ifndef VERMOUTH_EXPORTS
#define	VEXTERN
#define	VEXPORT		VERMOUTHCL
#else
#define	VEXTERN		__declspec(dllexport)
#define	VEXPORT		WINAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

VEXTERN UINT VEXPORT midiout_getver(char *string, UINT leng);
VEXTERN _MIDIHDL * VEXPORT midiout_create(MIDIMOD mod, UINT worksize);
VEXTERN void VEXPORT midiout_destroy(MIDIHDL hdl);
VEXTERN void VEXPORT midiout_shortmsg(MIDIHDL hdl, UINT32 msg);
VEXTERN void VEXPORT midiout_longmsg(MIDIHDL hdl, const UINT8 *msg, UINT size);
VEXTERN const SINT32 * VEXPORT midiout_get(MIDIHDL hdl, UINT *samples);
VEXTERN UINT VEXPORT midiout_get16(MIDIHDL hdl, SINT16 *pcm, UINT size);
VEXTERN UINT VEXPORT midiout_get32(MIDIHDL hdl, SINT32 *pcm, UINT size);
VEXTERN void VEXPORT midiout_setgain(MIDIHDL hdl, int gain);
VEXTERN void VEXPORT midiout_setmoduleid(MIDIHDL hdl, UINT8 moduleid);
VEXTERN void VEXPORT midiout_setportb(MIDIHDL hdl, MIDIHDL portb);

VEXTERN _MIDIMOD * VEXPORT midimod_create(UINT samprate);
VEXTERN void VEXPORT midimod_destroy(MIDIMOD mod);
VEXTERN BRESULT VEXPORT midimod_cfgload(MIDIMOD mod, const OEMCHAR *filename);
VEXTERN void VEXPORT midimod_loadprogram(MIDIMOD mod, UINT num);
VEXTERN void VEXPORT midimod_loadrhythm(MIDIMOD mod, UINT num);
VEXTERN void VEXPORT midimod_loadgm(MIDIMOD mod);
VEXTERN void VEXPORT midimod_loadall(MIDIMOD mod);
VEXTERN int VEXPORT midimod_loadallex(MIDIMOD mod, FNMIDIOUTLAEXCB cb,
															void *userdata);

#ifdef __cplusplus
}
#endif

