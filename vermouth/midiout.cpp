/**
 * @file	midiout.cpp
 */

#include	"compiler.h"
#include	"midiout.h"
#include <vector>
#include <math.h>
#include "../commng/cmmidioutvst.h"


#define	MIDIOUT_VERSION		0x116
#define	MIDIOUT_VERSTRING	"VERMOUTH 1.16"

static const char vermouthver[] = MIDIOUT_VERSTRING;


VEXTERN UINT VEXPORT midiout_getver(char *string, UINT leng) {

	leng = min(leng, sizeof(vermouthver));
	CopyMemory(string, vermouthver, leng);
	return((MIDIOUT_VERSION << 8) | 0x00);
}

VEXTERN MIDIHDL VEXPORT midiout_create(MIDIMOD mod, UINT worksize) {

	UINT	size;
	MIDIHDL	ret;

	if (mod == NULL) {
		return(NULL);
	}
	worksize = min(worksize, 512U);
	worksize = max(worksize, 16384U);
	size = sizeof(_MIDIHDL);
	size += sizeof(SINT32) * 2 * worksize;
	ret = static_cast<MIDIHDL>(malloc(size));
	if (ret) {
		midimod_lock(mod);
		ZeroMemory(ret, size);
		ret->samprate = mod->samprate;
		ret->worksize = worksize;
		ret->module = mod;
	//	ret->master = 127;
		ret->vst = midimod_getvst(mod, ret);
		ret->sampbuf = (SINT32 *)(ret + 1);
	}
	return(ret);
}

VEXTERN void VEXPORT midiout_destroy(MIDIHDL hdl) {

	MIDIMOD mod;

	if (hdl) {
		mod = hdl->module;
		midimod_freevst(mod, hdl);
		free(hdl);
		midimod_lock(mod);
	}
}

VEXTERN void VEXPORT midiout_shortmsg(MIDIHDL hdl, UINT32 msg) {

	UINT	cmd;

	if (hdl == NULL) {
		return;
	}
	cmd = msg & 0xff;
	if (cmd & 0x80) {
		hdl->status = cmd;
	}
	else {
		msg <<= 8;
		msg += hdl->status;
	}
	if (hdl->vst)
	{
		hdl->vst->Short(msg);
	}
}

static void VERMOUTHCL longmsg_uni(MIDIHDL hdl, const UINT8 *msg, UINT size) {

	if ((size >= 6) && (msg[2] == 0x7f)) {
		switch(msg[3]) {
			case 0x04:
				if ((msg[4] == 0x01) && (size >= 8)) {
					hdl->master = msg[6] & 0x7f;
				}
				break;
		}
	}
}

static bool VERMOUTHCL longmsg_roland(MIDIHDL hdl, const UINT8 *msg,
																UINT size) {

	UINT	addr;

	if (size <= 10) {
		return false;
	}
	// GS data set
	if ((msg[2] != 0x10) || (msg[3] != 0x42) || (msg[4] != 0x12)) {
		return false;
	}
	addr = (msg[5] << 16) + (msg[6] << 8) + msg[7];
	if ((addr & (~0x400000)) == 0x7f) {			// GS reset
		return false;
	}

	if ((addr & 0xfffff000) == 0x00400000)
	{
		return false;
	}
	if ((addr & 0xfff00000) == 0x00400000) {
		if (hdl->vst)
		{
			hdl->vst->Long(msg, size);
		}
	}
#if defined(ENABLE_PORTB)
	else if ((addr & 0xfff00000) == 0x00500000) {
		if ((hdl->portb) && (hdl->portb->vst))
		{
			std::vector<UINT8> msg2(msg, msg + size);
			msg2[5] = 0x40;
			hdl->portb->vst->Long(&msg2.at(0), size);
		}
	}
#endif	// defined(ENABLE_PORTB)
	return true;
}

VEXTERN void VEXPORT midiout_longmsg(MIDIHDL hdl, const UINT8 *msg, UINT size) {

	UINT	id;

	if ((hdl == NULL) || (msg == NULL)) {
		return;
	}
	if (size > 3) {							// (msg[size - 1] == 0xf7)
		id = msg[1];
		if (id == 0x7f) {					// Universal realtime
			longmsg_uni(hdl, msg, size);
		}
		else if (id == 0x7e) {				// GM
//			longmsg_gm(hdl, msg, size);
		}
		else if (id == 0x41) {				// Roland
			if (longmsg_roland(hdl, msg, size))
			{
				return;
			}
		}
	}
	if (hdl->vst)
	{
		hdl->vst->Long(msg, size);
	}
	if ((hdl->portb) && (hdl->portb->vst))
	{
		hdl->portb->vst->Long(msg, size);
	}
}

static UINT	VERMOUTHCL preparepcm(MIDIHDL hdl, UINT size) {

	UINT	ret;
	SINT32	*buf;

	ret = 0;
	size = min(size, hdl->worksize);
	buf = hdl->sampbuf;
	ZeroMemory(buf, size * 2 * sizeof(SINT32));
	if (hdl->vst)
	{
		hdl->vst->Process32(buf, size);
		ret = size;
	}
	return(ret);
}

VEXTERN const SINT32 * VEXPORT midiout_get(MIDIHDL hdl, UINT *samples) {

	UINT	size;

	if ((hdl == NULL) || (samples == NULL)) {
		goto moget_err;
	}
	size = *samples;
	if (size == 0) {
		goto moget_err;
	}
	size = preparepcm(hdl, size);
	if (size == 0) {
		goto moget_err;
	}

	*samples = size;
	return(hdl->sampbuf);

moget_err:
	return(NULL);
}

VEXTERN UINT VEXPORT midiout_get16(MIDIHDL hdl, SINT16 *pcm, UINT size) {

	UINT	step;
	SINT32	*buf;
	SINT32	l;
	SINT32	r;

	if (hdl != NULL) {
		while(size) {
			step = preparepcm(hdl, size);
			if (step == 0) {
				break;
			}
			size -= step;
			buf = hdl->sampbuf;
			do {
				l = pcm[0];
				r = pcm[1];
				l += buf[0];
				r += buf[1];
				if (l < -32768) {
					l = -32768;
				}
				else if (l > 32767) {
					l = 32767;
				}
				if (r < -32768) {
					r = -32768;
				}
				else if (r > 32767) {
					r = 32767;
				}
				pcm[0] = l;
				pcm[1] = r;
				buf += 2;
				pcm += 2;
			} while(--step);
		}
	}
	return(0);
}

VEXTERN UINT VEXPORT midiout_get32(MIDIHDL hdl, SINT32 *pcm, UINT size) {

	UINT	step;
	SINT32	*buf;

	if (hdl != NULL) {
		while(size) {
			step = preparepcm(hdl, size);
			if (step == 0) {
				break;
			}
			size -= step;
			buf = hdl->sampbuf;
			do {
				pcm[0] += buf[0];
				pcm[1] += buf[1];
				buf += 2;
				pcm += 2;
			} while(--step);
		}
	}
	return(0);
}

VEXTERN void VEXPORT midiout_setgain(MIDIHDL hdl, int gain) {

	if (hdl) {
		if (gain < -16) {
			gain = -16;
		}
		else if (gain > 8) {
			gain = 8;
		}
		hdl->gain = (SINT8)gain;
		if (hdl->vst)
		{
			hdl->vst->SetVolume(32767.0f * pow(2.0f, gain / 4.0f));
		}
	}
}

VEXTERN void VEXPORT midiout_setmoduleid(MIDIHDL hdl, UINT8 moduleid) {

	if (hdl) {
		hdl->moduleid = moduleid;
	}
}

VEXTERN void VEXPORT midiout_setportb(MIDIHDL hdl, MIDIHDL portb) {

#if defined(ENABLE_PORTB)
	if (hdl) {
		hdl->portb = portb;
	}
#endif	// defined(ENABLE_PORTB)
}

