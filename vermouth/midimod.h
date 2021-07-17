/**
 * @file	midimod.h
 */

#pragma once

#include "midiout.h"

void VERMOUTHCL midimod_lock(MIDIMOD mod);
void VERMOUTHCL midimod_unlock(MIDIMOD mod);
CComMidiOutVst* VERMOUTHCL midimod_getvst(MIDIMOD mod, MIDIHDL hdl);
void VERMOUTHCL midimod_freevst(MIDIMOD mod, MIDIHDL hdl);
