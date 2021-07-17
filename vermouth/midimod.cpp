/**
 * @file	midimod.cpp
 */

#include	"compiler.h"
#include	"midiout.h"
#include "../commng/cmmidioutvst.h"

VEXTERN MIDIMOD VEXPORT midimod_create(UINT samprate) {

	MIDIMOD	ret;

	ret = new _MIDIMOD;
	ret->samprate = samprate;
	ret->lockcount = 0;

	midimod_lock(ret);
	return(ret);
}

void VERMOUTHCL midimod_lock(MIDIMOD mod) {

	mod->lockcount++;
}

void VERMOUTHCL midimod_unlock(MIDIMOD mod) {

	if (!mod->lockcount) {
		return;
	}
	mod->lockcount--;
	if (mod->lockcount) {
		return;
	}

	while (!mod->handles.empty())
	{
		delete mod->handles.front().vst;
		mod->handles.pop_front();
	}
	delete mod;
}

VEXTERN void VEXPORT midimod_destroy(MIDIMOD mod) {

	if (mod) {
		midimod_unlock(mod);
	}
}

VEXTERN BRESULT VEXPORT midimod_cfgload(MIDIMOD mod,
												const OEMCHAR *filename) {

	return 0;
}

VEXTERN void VEXPORT midimod_loadprogram(MIDIMOD mod, UINT num) {
}

VEXTERN void VEXPORT midimod_loadrhythm(MIDIMOD mod, UINT num) {
}

VEXTERN void VEXPORT midimod_loadgm(MIDIMOD mod) {
}

VEXTERN void VEXPORT midimod_loadall(MIDIMOD mod) {
}

VEXTERN int VEXPORT midimod_loadallex(MIDIMOD mod, FNMIDIOUTLAEXCB cb,
															void *userdata) {

	int					result;

	result = MIDIOUT_SUCCESS;
	return result;
}

CComMidiOutVst* VERMOUTHCL midimod_getvst(MIDIMOD mod, MIDIHDL hdl)
{
	for (std::deque<vsthdl>::iterator it = mod->handles.begin(); it != mod->handles.end(); ++it)
	{
		if (it->hdl == NULL)
		{
			vsthdl vh = *it;
			mod->handles.erase(it);
			vh.hdl = hdl;
			mod->handles.push_back(vh);
			return vh.vst;
		}
	}

	if (mod->handles.size() < 2)
	{
		vsthdl vh;
		vh.hdl = hdl;
		vh.vst = new CComMidiOutVst;
		vh.vst->Initialize(mod->samprate);
		mod->handles.push_back(vh);
		return vh.vst;
	}

	if (!mod->handles.empty())
	{
		vsthdl vh = mod->handles.front();
		mod->handles.pop_front();
		vh.hdl->vst = NULL;
		vh.hdl = hdl;
		mod->handles.push_back(vh);
		return vh.vst;
	}

	return NULL;
}

void VERMOUTHCL midimod_freevst(MIDIMOD mod, MIDIHDL hdl)
{
	if (hdl == NULL)
	{
		return;
	}
	for (std::deque<vsthdl>::iterator it = mod->handles.begin(); it != mod->handles.end(); ++it)
	{
		if (it->hdl == hdl)
		{
			it->hdl = NULL;
			break;
		}
	}
}
