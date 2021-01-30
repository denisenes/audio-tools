//
// Created by charlie on 24.10.2020.
//

#ifndef SOUNDLIB_MIXER_H
#define SOUNDLIB_MIXER_H

#include "Wave.h"


class Mixer {
public:
    static Wave * mix(int64_t N, ...);
    static void mix2(Wave * wave1, Wave * wave2);
};


#endif //SOUNDLIB_MIXER_H
