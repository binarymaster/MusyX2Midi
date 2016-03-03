typedef   signed char      s8;
typedef unsigned char      u8;
typedef   signed short     s16;
typedef unsigned short     u16;
typedef   signed int       s32;
typedef unsigned int       u32;

u16 tempHeader;
u32 typeHeader;
u32 tempOffset;
u8 tempChar;
int i, j, k, l, m;

static inline u32 ReadBE(FILE *f, s32 b) {
	u32 v = 0;
	for (s32 i = b - 8; i >= 0; i -= 8) v |= fgetc(f) << i;
	return v;
}

typedef struct {
	u32 offset[2];
	u32 startTick[2];
	u32 infoOffset;
	u32 size;
	u32 end[2];
	u8 songDataId[2];
	u8 instrument;
	u8 realInstrument;
	int trackNo;
	int pieces;
	u8 channel;
} seqTrack;

typedef struct {
	u32 headerSize;
	u32 trackLocationOffset;
	u32 trackCountOffset;
	u32 trackInfoOffset;
	u32 tempoInfoOffset;
	u32 trackInfoSize;
	u32 tempo;
	u32 timeBase;
	u32 loopStart;
	u32 loopEnd;
	bool loops;
	bool pokemon;
	bool symphonia;
	int trackCount;
	int trackPieces;
	vector<seqTrack> tracks;
} bseq;
