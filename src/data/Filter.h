/**
 * This class implements a butterworth bandpass filter.
 *
 * It is used for filtering an input signal in the time domain by
 * convoluting it withe the filter.
 *
 * Butterworth bandpass filter.
 * (http://www-users.cs.york.ac.uk/~fisher/mkfilter/)
 *
    filtertype  = Butterworth
    passtype    = Bandpass
    ripple      =
    order       = 2
    samplerate  = 8
    corner1	   = 0.7
    corner2	   = 3.9
    adzero	   =
    logmin	   =
 */

#ifndef FILTER_H
#define FILTER_H

#include "FFTBuffer.h"

// Default filter properties
#define NZEROS 4
#define NPOLES 4
#define GAIN 1.564354775e+00

#define MIN_PULSE_FREQUENCY 0.7
#define MAX_PULSE_FREQUENCY 3.9
#define SAMPLE_RATE 8.0

namespace hrm
{

    class Filter
    {
        private:
            int nzeros;
            int npoles;
            double gain;

            double minFrequency;
            double maxFrequency;
            double sampleRate;

            std::vector<float> xv;
            std::vector<float> yv;

        public:
            Filter();

            /**
             * Convolute the given buffer with the filter and cut the
             * given number of samples from the buffer (stabilization
             * time).
             */
            void filter(FFTBuffer *buffer, int cut);

            double getMinFrequency();

            double getMaxFrequency();
    };

}

#endif
