
/*
Name:		EasyBuzzer.h
Version:	1.0.3
Created:	9/29/2017 12:03:48 AM
Updated:	1/19/2023 12:19:00 AM
Author:		Evert Arias
Changed:  Abdelrahman Shehata
Github:		https://github.com/evert-arias/EasyBuzzer
            Copyright (c) 2019 Evert Arias
*/

#ifndef _EasyBuzzer_h
#define _EasyBuzzer_h

#include "Arduino.h"
#include "Song.h"
#include "Config.h"

class SongPlayer
{
private:
    Song* mSong = nullptr;
    size_t mIndex = 0; 
    bool mStopped = false;
    unsigned int mPin = DEFAULT_PIN;
    unsigned int mChannel = DEFAULT_CHANNEL;
    unsigned int mResolution = DEFAULT_RESOLUTION;
public:
    SongPlayer(unsigned int pin): mPin(pin) {}
    ~SongPlayer() = default;

    void setSong(Song* song) {
      mSong = song;
    }

    void start() {
      Serial.println("Starting");
      mStopped = false;      
    }
    
    /* Stop beeping. */
    void stop()
    {
        Serial.println("Stopping");
        mStopped = true;
        noTone(mPin);
    }

    /* Update function that keeps the library running. */
    void update()
    {
        if (mStopped) return;

        const auto currentMelody = mSong->data[mIndex % mSong->length];
        const auto duration = currentMelody.duration;
        tone(mPin, currentMelody.frequency, duration * 0.9);
        delay(duration);
        noTone(buzzerDrunk);

        mIndex += 1;
    };
};

#endif
