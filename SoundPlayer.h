#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H
#include "SevenSegment.h"

using namespace std;

class SoundPlayer : public SevenSegment
{
	public:
		void PlayAll();
		void PlayNote(int note, double time);
		SoundPlayer();
		~SoundPlayer();
};
#endif