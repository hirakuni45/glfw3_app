#pragma once

#include <vector>

struct note_t {
	uint32_t	frame;
	uint32_t	count;
	uint8_t		key;
	uint8_t		velo;
	bool		sustain;
	note_t() : frame(0), count(0), key(0), velo(0), sustain(false) { } 
};

typedef std::vector<note_t> NOTES;

void convertMidiFileToText(smf::MidiFile& midifile, int tempo, NOTES& notes);

