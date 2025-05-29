#include <portaudio.h>
#include <iostream>
#include <soundtouch/SoundTouch.h>

using namespace soundtouch;

#define SAMPLE_RATE 48000
#define FRAMES_PER_BUFFER 1024
#define NUM_CHANNELS 2

SoundTouch soundTouch;

static int audioCallback(const void* input, void* output,
                         unsigned long frameCount, const PaStreamCallbackTimeInfo*,
                         PaStreamCallbackFlags,
                         void* userData)
{
    float *in = (float*)input;
    float *out = (float*)output;
    
    soundTouch.putSamples(in, frameCount);

    int recieved = soundTouch.receiveSamples(out, frameCount);

    if(recieved < frameCount)
    {
        std::fill(out + recieved, out + frameCount, 0.0f);
    }

    return paContinue;
}

int main()
{
    Pa_Initialize();

    soundTouch.setSetting(SETTING_USE_AA_FILTER, 1);
    soundTouch.setSetting(SETTING_AA_FILTER_LENGTH, 64);
    soundTouch.setSetting(SETTING_SEQUENCE_MS, 40);
    soundTouch.setSetting(SETTING_SEEKWINDOW_MS, 20);
    soundTouch.setSetting(SETTING_OVERLAP_MS, 15);

    soundTouch.setSampleRate(SAMPLE_RATE);
    soundTouch.setChannels(NUM_CHANNELS);
    soundTouch.setPitchSemiTones(-4.0f);
    soundTouch.setTempo(1.0);

    PaStream *stream;

    Pa_OpenDefaultStream(&stream,
                         NUM_CHANNELS,
                         NUM_CHANNELS,
                         paFloat32,
                         SAMPLE_RATE,
                         FRAMES_PER_BUFFER,
                         audioCallback,
                         nullptr);

    Pa_StartStream(stream);
    std::cout << "|...Running...| Press Enter to exit.\n";
    std::cin.get();

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}