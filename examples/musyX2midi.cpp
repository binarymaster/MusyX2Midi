
//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu> 
// Creation Date: Tue Jan  8 10:08:15 PST 2002
// Last Modified: Tue Jan  8 10:31:08 PST 2002
// Filename:      ...sig/doc/examples/all/createmidifile/createmidifile.cpp
// Syntax:        C++
// 
// Description:   Demonstration of how to create a Multi-track MIDI file.
//

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <math.h>
#include "MidiFile.h"
#include "SEQ.h"

#ifndef OLDCPP
   #include <iostream>
   using namespace std;
#else
   #include <iostream.h>
#endif


typedef unsigned char uchar;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {

	FILE * seq;
	u32(*Read)(FILE *f, s32 b);
	Read = ReadBE;
	u8 tempVal, tempVal2, note, channel, velocity;
	u32 tempOffset, tempo, bend;
	bool endOfTrack = false;
	
	if (argc != 2) {
		printf("Usage:\n%s music.song", argv[0]);
		exit(0);
	}

	seq = fopen(argv[1], "rb");
	typeHeader = Read(seq, 32);

	if (typeHeader != 0x00000018 && typeHeader != 0x00000058) {
		printf("Not a valid sequence\n");
		fclose(seq);
		exit(1);
	}


	else {
		bseq song;
		song.headerSize = typeHeader;
		song.loops = false;
		song.pokemon = false;
		song.symphonia = false;
//		if (strchr(argv[1], 'unpacked'))
//			song.pokemon = true;		
		song.trackCount = 0;
		song.trackLocationOffset = Read(seq, 32);
		string name = string(argv[1]) + ".mid";
		song.trackCountOffset = Read(seq, 32);
		song.tempoInfoOffset = Read(seq, 32);
		printf("Tempo info located at 0x%X\n", song.tempoInfoOffset);
		if (song.headerSize < 0x58) {
			song.tempo = Read(seq, 32);
			song.loopStart = Read(seq, 32);
		}
		printf("Default tempo is %d BPM\n", song.tempo);
		song.trackPieces = 1;
		k = 0;
//		printf("Track info location at 0x%X\n", song.trackCountOffset);
		fseek(seq, song.trackCountOffset + 1, SEEK_SET);
//		tempVal = fgetc(seq);
		while(fgetc(seq) != 0xFF) {
			song.trackCount++;
//			song.tracks.resize(song.trackCount);
//			song.tracks[k].prgNumber = tempVal;
//			k++;
//			tempVal = fgetc(seq);
		}
//		printf("%d tracks for this file\n", song.trackCount);
		if (song.trackCount == 0) {
			printf("Well, this song is empty\n");
			fclose(seq);
			exit(1);
		}
		else {
			song.tracks.resize(song.trackCount);
			MidiFile outputfile;        // create an empty MIDI file with one track
			outputfile.absoluteTime();  // time information stored as absolute time
			// (will be coverted to delta time when written)
			     // Add another two tracks to the MIDI file
			Array<uchar> midievent;     // temporary storage for MIDI events 
			midievent.setSize(3);       // set the size of the array to 3 bytes
			int actionTime, tempTime;      // temporary storage for MIDI event time
			u16 noteLength = 0;
			int tpq = 386;              // default value in MIDI file is 48
			outputfile.setTicksPerQuarterNote(tpq);

			
			for (i = 0; i < song.trackCount; i++) {
				fseek(seq, song.headerSize + 4 + i * 4, SEEK_SET);
				song.tracks[i].infoOffset = Read(seq, 32);
				if (i > 0)
					song.tracks[i-1].size = song.tracks[i].infoOffset - song.tracks[i-1].infoOffset;				
			}
			song.tracks[song.trackCount - 1].size = song.trackLocationOffset - song.tracks[song.trackCount - 1].infoOffset;
			for (i = 0; i < song.trackCount; i++) {
				song.tracks[i].trackNo = outputfile.addTrack();
				fseek(seq, song.trackCountOffset + 1 + i, SEEK_SET);
				song.tracks[i].channel = fgetc(seq);
				song.tracks[i].pieces = 1;
				if (i == song.trackCount - 1)
					fseek(seq, song.trackLocationOffset - song.tracks[i].size, SEEK_SET);
				else
					fseek(seq, song.tracks[i+1].infoOffset - song.tracks[i].size, SEEK_SET);
				song.tracks[i].startTick[0] = Read(seq, 32);
				fseek(seq, 5, SEEK_CUR);
				song.tracks[i].songDataId[0] = fgetc(seq);
				fseek(seq, song.trackLocationOffset + song.tracks[i].songDataId[0] * 4, SEEK_SET);
				song.tracks[i].offset[0] = Read(seq, 32);
				if (song.tracks[i].size == 0x24) {
					song.tracks[i].pieces = 2;
					if (i == song.trackCount - 1) 
						fseek(seq, song.trackLocationOffset - 0x18, SEEK_SET);
					else
						fseek(seq, song.tracks[i+1].infoOffset - 0x18, SEEK_SET);
//					printf("Track %d has 2 sections 0x%X\n", i, ftell(seq));
					song.tracks[i].startTick[1] = Read(seq, 32);
					fseek(seq, 5, SEEK_CUR);
					song.tracks[i].songDataId[1] = fgetc(seq);
					fseek(seq, song.trackLocationOffset + song.tracks[i].songDataId[1] * 4, SEEK_SET);
					song.tracks[i].offset[1] = Read(seq, 32);
					song.tracks[i].end[0] = song.tracks[i].offset[1];					
				}		
				if (i == song.trackCount - 1) {
					fseek(seq, song.trackLocationOffset - 0xC, SEEK_SET);
					song.loopEnd = Read(seq, 32);
					fseek(seq, 5, SEEK_CUR);
					if (fgetc(seq) == 0xFE)
						song.loops = true;
				}	
			}
			if (song.tempo) {
				fseek(seq, (int)song.tempoInfoOffset, SEEK_SET);
				printf("Checking for tempo table at 0x%X\n", ftell(seq));
				if (ftell(seq) > 0) {
					midievent.setSize(6);
					midievent[0] = 0xff;
					midievent[1] = 0x51;
					midievent[2] = 0x03;
					while ((tempOffset = Read(seq, 32)) != 0xFFFFFFFF) {
						
						tempo = Read(seq, 32);
						printf("Writing tempo %d BPM\n", tempo);
						int microseconds = (int)(60.0 / tempo * 1000000.0 + 0.5);
						midievent[3] = (microseconds >> 16) & 0xff;
						midievent[4] = (microseconds >> 8) & 0xff;
						midievent[5] = (microseconds >> 0) & 0xff;
						outputfile.addEvent(0, tempOffset, midievent);
					}
				}
				
				else {
					midievent.setSize(6);
					midievent[0] = 0xff;
					midievent[1] = 0x51;
					midievent[2] = 0x03;
					int microseconds = (int)(60.0 / song.tempo * 1000000.0 + 0.5);
					midievent[3] = (microseconds >> 16) & 0xff;
					midievent[4] = (microseconds >> 8) & 0xff;
					midievent[5] = (microseconds >> 0) & 0xff;
					outputfile.addEvent(0, 0, midievent);
				}
			}
			for (i = 0; i < song.trackCount; i++) {
				printf("Track_%d:\n", i);
				actionTime = 0;	// Reset position when starting new track
				
//				midievent.setSize(2);
//				midievent[0] = 0x01;
//				midievent[1] = song.tracks[i].channel;
//				midievent[0] = 0xC0|i;    
//				midievent[1] = song.tracks[i].channel;		// setting song.tracks[i].instrument  
//				outputfile.addEvent(song.tracks[i].trackNo, actionTime, midievent);    
//				outputfile.addMetaEvent(song.tracks[i].trackNo, actionTime, 0x20, midievent);
				for (j = 0; j < song.tracks[i].pieces; j++) {
				if (song.tracks[i].pieces == 2) {
					if (i == song.trackCount - 1)
						song.tracks[i].end[1] = song.trackCountOffset;
					else
						song.tracks[i].end[1] = song.tracks[i + 1].offset[0];
				}
				else {
					if (i == song.trackCount - 1)
						song.tracks[i].end[0] = song.trackCountOffset;
					else
						song.tracks[i].end[0] = song.tracks[i + 1].offset[0];
				}
				actionTime = song.tracks[i].startTick[j];
//				printf("Starting at 0x%X\n, part %d of %d\n", song.tracks[i].startTick[j], j+1, song.tracks[i].pieces);
				fseek(seq, song.tracks[i].offset[j] + 4, SEEK_SET);
				typeHeader = Read(seq, 32);
				if (typeHeader > 0)
					song.tracks[i].end[j] = typeHeader;
				Read(seq, 32);
//				printf("This track should end at 0x%X\n", song.tracks[i].end[j]);
				while (ftell(seq) < song.tracks[i].end[j]) {
					tempChar = fgetc(seq);
					tempTime = 0;
					if (tempChar < 256) {
						fseek(seq, -1, SEEK_CUR);
						tempTime = Read(seq, 16);
					actionTime += tempTime;
					tempVal = fgetc(seq);
					tempVal2 = fgetc(seq);
					if (tempVal < 0x80) { // note on event
						note = tempVal;
						velocity = tempVal2;
						channel = song.tracks[i].channel;
						if (song.pokemon) {
							switch(song.tracks[i].realInstrument) {
								case 27:
									note = 38;
									break;
								case 9:
									if (note == 84) {
										note = 49;
										printf("Found CrashCymbal 2 at %X\n", ftell(seq));
										channel = 9;
										velocity = 127;
									}
									break;
								default:
									break;
							}
						}
						else if (song.symphonia) {	// fixing Symphonia instruments
							switch(song.tracks[i].realInstrument) {
								case 33:
								case 123:
									note -= 12;	// moving bass and tom drum notes down 1 octave
									break;
								case 115:
								case 121:
									note = 38;
									break;
								case 116:
								case 120:
									note = 36;
									break;									
								default:
									break;
							}
						}
						midievent.setSize(3);
						midievent[0] = 0x90|channel;  
						midievent[1] = note;
						midievent[2] = velocity;       // store attack/release velocity for note command
						outputfile.addEvent(song.tracks[i].trackNo, actionTime, midievent);
						noteLength = Read(seq, 16);	// get length of note
//						printf("\tTick %d - Writing note on #%X, velocity %d - offset 0x%X\n", actionTime, midievent[1], midievent[2], ftell(seq));
						midievent[0] = 0x80|channel;     // store a note off command (MIDI channel 1)
						outputfile.addEvent(song.tracks[i].trackNo, actionTime + noteLength, midievent);
//						printf("\tTick %d - Writing note off #%X\n", actionTime + noteLength, midievent[1]);
					}
					
					else {
						switch (tempVal2) {
/*							case 0x8B:	// volume adjust?
								midievent.setSize(3);
								midievent[0] = 0xB0|song.tracks[i].channel;     // store a note on command (MIDI channel 1)
								midievent[1] = 0x07;
								midievent[2] = tempVal - 0x80;
								outputfile.addEvent(song.tracks[i].trackNo, actionTime, midievent);								
								break; */
							case 0x8A:	//	pan
							case 0x87:	// main volume
//							case 0x8B:	// expression?
//							case 0x94:case 0x95:case 0x96:case 0x97:case 0x98:
								midievent.setSize(3);
								midievent[0] = 0xB0|song.tracks[i].channel;     // store a note on command (MIDI channel 1)
								midievent[1] = tempVal2 - 0x80;
								midievent[2] = tempVal - 0x80;
								outputfile.addEvent(song.tracks[i].trackNo, actionTime, midievent);
								break;
/*							case (bend value here):	// trying pitch bend
								midievent.setSize(3);
								bend = (tempVal - 0x100) * 16384/256;
								midievent[0] = 0xE0|song.tracks[i].channel;     // store a note on command (MIDI channel 1)
								midievent[1] = bend&0x7f; 
								midievent[2] = (bend>>7)&0x7f;
								outputfile.addEvent(song.tracks[i].trackNo, actionTime, midievent);								
								break; */
							case 0x00:	// set song.tracks[i].instrument
								midievent.setSize(2); 
								song.tracks[i].instrument = song.tracks[i].realInstrument = tempVal - 0x80;
								if (song.pokemon) {
									switch (song.tracks[i].realInstrument) {
										case 5:
											song.tracks[i].instrument = 48;
											break;
										case 9:
											song.tracks[i].instrument = 47;
											break;
										case 67:
											song.tracks[i].instrument = 60;
											break;
										case 16:
											song.tracks[i].instrument = 61;
											break;
										case 17:
											song.tracks[i].instrument = 33;
											break;
										case 1:
											song.tracks[i].instrument = 38;
											break;
										case 64:
											song.tracks[i].instrument = 73;
											break;
										case 63:
											song.tracks[i].instrument = 72;
											break;
										case 27:
											song.tracks[i].instrument = 0;
											song.tracks[i].channel = 9;
											break;
										case 32:
											song.tracks[i].instrument = 29;
											break;
										case 59:
											song.tracks[i].instrument = 46;
											break;
										default:
											song.tracks[i].instrument =  song.tracks[i].realInstrument;
											break;
									} 
								}
								else if (song.symphonia) {
									switch (song.tracks[i].realInstrument) {
										case 115:
										case 116:
										case 120:
										case 121:
										case 123:
											song.tracks[i].instrument = 0;
											song.tracks[i].channel = 9;
											break;
										default:
											song.tracks[i].instrument =  song.tracks[i].realInstrument;
											break;
									} 
								}
								midievent[0] = 0xC0|song.tracks[i].channel;  
								midievent[1] = song.tracks[i].instrument;		// setting instrument
								outputfile.addEvent(song.tracks[i].trackNo, actionTime, midievent); 
								break; 
							default:
								printf("\tTick %d, Unknown event %X at 0x%X\n", actionTime, tempVal2, ftell(seq) - 1);
								break;
						}
					}
				}
				
				else {
					printf("\tUnknown event %X at 0x%X\n", tempChar, ftell(seq) - 1);
					fgetc(seq);
				}
				}
				
			}
			}
			// writing loop points
			if (song.loops) {
				outputfile.addMetaEvent(0, song.loopStart, 0x06, "loopStart");
				outputfile.addMetaEvent(0, song.loopEnd, 0x06, "loopEnd");
			}
			outputfile.sortTracks();         // make sure data is in correct order
			outputfile.write(name.c_str()); // write Standard MIDI File
			printf("%.02f seconds long\n", outputfile.getTimeInSeconds(song.loopEnd));
			fclose(seq);
			return 0;
		}
	}
}


/*  FUNCTIONS available in the MidiFile class:

void absoluteTime(void);
   Set the time information to absolute time.
int addEvent(int aTrack, int aTime, Array<uchar>& midiData);
   Add an event to the end of a MIDI track.
int addTrack(void);
   Add an empty track to the MIDI file.
int addTrack(int count);
   Add a number of empty tracks to the MIDI file.
void deltaTime(void);
   Set the time information to delta time.
void deleteTrack(int aTrack);
   remove a track from the MIDI file.
void erase(void);
   Empty the contents of the MIDI file, leaving one track with no data.
_MFEvent& getEvent(int aTrack, int anIndex);
   Return a MIDI event from the Track.
int getTimeState(void);
   Indicates if the timestate is TIME_STATE_ABSOLUTE or TIME_STATE_DELTA.
int getTrackState(void);
   Indicates if the tracks are being processed as multiple tracks or 
   as a single track.
int getTicksPerQuarterNote(void);
   Returns the ticks per quarter note value from the MIDI file.
int getTrackCount(void);
   Returns the number of tracks in the MIDI file.
int getNumTracks(void);
   Alias for getTrackCount();
int getNumEvents(int aTrack);
   Return the number of events present in the given track.
void joinTracks(void);
   Merge all tracks together into one track.  This function is reversable,
   unlike mergeTracks().
void mergeTracks(int aTrack1, int aTrack2);
   Combine the two tracks into a single track stored in Track1.  Track2
   is deleted.
int read(char* aFile);
   Read the contents of a MIDI file into the MidiFile class data structure
void setTicksPerQuarterNote    (int ticks);
   Set the MIDI file's ticks per quarter note information
void sortTrack(Collection<_MFEvent>& trackData);
   If in absolute time, sort particular track into correct time order.
void sortTracks(void);
   If in absolute time, sort tracks into correct time order.

*/



// md5sum: 89b83de0fbe279cccd183ef8c1583f62 createmidifile.cpp [20100703]
