//
// Created by charlie on 24.10.2020.
//

#ifndef SOUNDLIB_ADSR_H
#define SOUNDLIB_ADSR_H

#include "Wave.h"

// simple linear ADSR volume envelope (mono)
// 0 <= a, d, s, r <= 100 (in percents)
// 0 <= susVal <= 1
class ADSR {
public:
    static void make(Wave * wave, double attack, double decay, double sustain, double susVal, double release);
};


#endif //SOUNDLIB_ADSR_H
