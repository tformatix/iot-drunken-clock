#ifndef _Song_h
#define _Song_h

#define __ASSERT_USE_STDERR

#include <assert.h>

struct Melody {
  unsigned int frequency;
  unsigned int duration;
};

struct Song {
  Melody *data = nullptr;
  size_t length = 0;

  Song(Melody *_data, size_t _length)
    : data(_data), length(_length) {}

  template<size_t N>
  static Song create(const int (&_data)[N], int note, Melody* buffer) {
    assert(N % 2 == 0);
    // maybe a memcpy would be better
    for (size_t i = 0; i < N; i += 2) {
      const auto divider = _data[i + 1];
      // Serial.println(String(divider));
      unsigned long duration = 0;
      if (divider > 0) {
        // regular note, just proceed
        duration = (note) / divider;
      } else if (divider < 0) {
        // dotted notes are represented with negative durations!!
        duration = (note) / abs(divider);
        duration *= 1.5;  // increases the duration in half for dotted notes
      }

      buffer[i / 2].frequency = _data[i];
      buffer[i / 2].duration = duration;
    }

    return Song(buffer, static_cast<size_t>(N / 2));
  }

  template<size_t N>
  static Song create(Melody *data[N]) {
    return Song(data, N);
  }
};

#endif