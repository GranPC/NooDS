/*
    Copyright 2019-2020 Hydr8gon

    This file is part of NooDS.

    NooDS is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NooDS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NooDS. If not, see <https://www.gnu.org/licenses/>.
*/

#include "noo_app.h"
#include "../settings.h"

int NooApp::keyMap[] = { 'L', 'K', 'G', 'H', 'D', 'A', 'W', 'S', 'P', 'Q', 'O', 'I' };

bool NooApp::OnInit()
{
    // Define the platform settings
    std::vector<Setting> platformSettings =
    {
        Setting("keyA",      &keyMap[0],  false),
        Setting("keyB",      &keyMap[1],  false),
        Setting("keySelect", &keyMap[2],  false),
        Setting("keyStart",  &keyMap[3],  false),
        Setting("keyRight",  &keyMap[4],  false),
        Setting("keyLeft",   &keyMap[5],  false),
        Setting("keyUp",     &keyMap[6],  false),
        Setting("keyDown",   &keyMap[7],  false),
        Setting("keyR",      &keyMap[8],  false),
        Setting("keyL",      &keyMap[9],  false),
        Setting("keyX",      &keyMap[10], false),
        Setting("keyY",      &keyMap[11], false)
    };

    // Load the settings
    Settings::load(platformSettings);

    // Set up the window
    frame = new NooFrame(&emulator);
    panel = new NooPanel(frame, &emulator);
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(panel, 1, wxEXPAND);
    frame->SetSizer(sizer);
    Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(NooApp::requestDraw));

    // Start the audio service
    PaStream *stream;
    Pa_Initialize();
    Pa_OpenDefaultStream(&stream, 0, 2, paInt16, 48000, 1024, audioCallback, &emulator);
    Pa_StartStream(stream);

    return true;
}

void NooApp::requestDraw(wxIdleEvent &event)
{
    // Refresh the display
    panel->Refresh();
    event.RequestMore();

    // Update the FPS in the window title if the core is running
    if (emulator.running)
        frame->SetLabel(wxString::Format(wxT("NooDS - %d FPS"), emulator.core->getFps()));
}

int NooApp::audioCallback(const void *in, void *out, unsigned long frames,
                          const PaStreamCallbackTimeInfo *info, PaStreamCallbackFlags flags, void *data)
{
    int16_t *buffer = (int16_t*)out;
    Emulator *emulator = (Emulator*)data;

    // The NDS sample rate is 32768Hz, and PortAudio supports this frequency directly
    // It causes issues on some systems though, so a more standard frequency of 48000Hz is used instead
    // Get 700 samples at 32768Hz, which is equal to about 1024 samples at 48000Hz
    uint32_t original[700];
    for (int i = 0; i < 700; i++)
        original[i] = emulator->running ? emulator->core->getSample() : 0;

    // Stretch the 700 samples out to 1024 samples in the audio buffer
    for (int i = 0; i < frames; i++)
    {
        uint32_t sample = original[i * 700 / 1024];
        buffer[i * 2 + 0] = sample >>  0;
        buffer[i * 2 + 1] = sample >> 16;
    }

    return 0;
}
