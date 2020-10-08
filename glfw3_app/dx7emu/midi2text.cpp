#include "MidiFile.h"
#include "Options.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "midi2text.hpp"

#include "utils/format.hpp"

using namespace std;
using namespace smf;

// user interface variables
Options options;
int     debugQ = 0;             // use with --debug option
int     maxcount = 100000;
double  tempo = 50.0;

// function declarations:
void      setTempo              (MidiFile& midifile, int index, double& tempo);
void      checkOptions          (Options& opts, int argc, char** argv);
void      example               (void);
void      usage                 (const char* command);

//////////////////////////////////////////////////////////////////////////
#if 0
int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile(options.getArg(1));
   convertMidiFileToText(midifile);
   return 0;
}
#endif
//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// convertMidiFileToText --
//

void convertMidiFileToText(MidiFile& midifile, int tem, NOTES& notes)
{
	tempo = static_cast<double>(tem);

	midifile.absoluteTicks();
	midifile.joinTracks();

   vector<double> ontimes(128);
   vector<int> onvelocities(128);
   int i;
   for (i=0; i<128; i++) {
      ontimes[i] = -1.0;
      onvelocities[i] = -1;
   }

   double offtime = 0.0;

   int key = 0;
   int vel = 0;

   for (i=0; i<midifile.getNumEvents(0); i++) {
      int command = midifile[0][i][0] & 0xf0;
      if (command == 0x90 && midifile[0][i][2] != 0) {
         // store note-on velocity and time
         key = midifile[0][i][1];
         vel = midifile[0][i][2];
         ontimes[key] = midifile[0][i].tick * 60.0 / tempo /
               midifile.getTicksPerQuarterNote();
         onvelocities[key] = vel;
      } else if (command == 0x90 || command == 0x80) {
         // note off command write to output
         key = midifile[0][i][1];
         offtime = midifile[0][i].tick * 60.0 /
               midifile.getTicksPerQuarterNote() / tempo;

		note_t t;
		t.frame = ontimes[key] * 60.0;  // frame rate
		t.count = (offtime - ontimes[key]) * 60;  // frame rate
		t.key = key;
		t.velo = onvelocities[key];
		t.sustain = midifile[0][i].isSustainOn();
		notes.push_back(t);
		utils::format("Frame: %u, Len: %u, %c %02X, %02X\n") % t.frame % t.count
			% (t.sustain ? 'S' : '-')
			% static_cast<uint16_t>(t.key) % static_cast<uint16_t>(t.velo);
//         cout << "note\t" << ontimes[key]
//              << "\t" << offtime - ontimes[key]
//              << "\t" << key << "\t" << onvelocities[key] << endl;
         onvelocities[key] = -1;
         ontimes[key] = -1.0;
      }

      // check for tempo indication
      if (midifile[0][i][0] == 0xff &&
                 midifile[0][i][1] == 0x51) {
         setTempo(midifile, i, tempo);
      }
   }
}


//////////////////////////////
//
// setTempo -- set the current tempo
//

void setTempo(MidiFile& midifile, int index, double& tempo) {
   static int count = 0;
   count++;

   MidiEvent& mididata = midifile[0][index];

   int microseconds = 0;
   microseconds = microseconds | (mididata[3] << 16);
   microseconds = microseconds | (mididata[4] << 8);
   microseconds = microseconds | (mididata[5] << 0);

   double newtempo = 60.0 / microseconds * 1000000.0;
   if (count <= 1) {
      tempo = newtempo;
   } else if (tempo != newtempo) {
///      cout << "; WARNING: change of tempo from " << tempo
///           << " to " << newtempo << " ignored" << endl;
   }
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("author=b",  "author of program");
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");
   opts.define("h|help=b",  "short description");

   opts.define("debug=b",  "debug mode to find errors in input file");
   opts.define("max=i:100000", "maximum number of notes expected in input");

   opts.process(argc, argv);

   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 22 Jan 2002" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: 22 Jan 2002" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand().c_str());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   debugQ = opts.getBoolean("debug");
   maxcount = opts.getInteger("max");

   if (opts.getArgCount() != 1) {
      usage(opts.getCommand().c_str());
      exit(1);
   }

}



//////////////////////////////
//
// example --
//

void example(void) {

}



//////////////////////////////
//
// usage --
//

void usage(const char* command) {
   cout << "Usage: " << command << " midifile" << endl;
}



