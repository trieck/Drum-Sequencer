#include "StdAfx.h"
#include "Sequencer.h"
#include "outputdevs.h"
#include "resource.h"

Sequencer::Sequencer() : m_pStream(NULL), m_state(Stopped)
{
}

Sequencer::~Sequencer()
{
    Close();
    if (m_pStream != NULL) {
        delete m_pStream;
        m_pStream = NULL;
    }
}

BOOL Sequencer::Initialize()
{
    ASSERT(m_pStream == NULL);

    // Set the output stream to the midi mapper
    m_pStream = (MidiStream *)OutputDevices().GetStream(MIDI_MAPPER);
    if (m_pStream == NULL) {
        AfxMessageBox(IDS_COULDNOTOPENMIDIMAPPER);
        return FALSE;
    }

    m_pStream->SetSequencer(this);
    m_pStream->RegisterHook(StreamProc);

    // Open the output device
    MMRESULT result = m_pStream->Open();
    if (result != MMSYSERR_NOERROR) {
        AfxMessageBox(OutputDevice::GetErrorText(result));
        return FALSE;
    }

    // Set the time division
    MIDIPROPTIMEDIV prop;
    prop.cbStruct = sizeof(MIDIPROPTIMEDIV);
    prop.dwTimeDiv = DEFAULT_PPQN;
    if (m_pStream->Property((LPBYTE)&prop, MIDIPROP_SET | MIDIPROP_TIMEDIV) != MMSYSERR_NOERROR) {
        AfxMessageBox(IDS_COULDNOTSETTIMEDIVISION);
        return FALSE;
    }

    return TRUE;
}

void Sequencer::Close()
{
    if (m_pStream != NULL) {
        m_pStream->Close();
    }
}

BOOL Sequencer::Play(const Sequence &sequence)
{
    ASSERT(m_pStream != NULL);
    ASSERT(m_pStream->IsOpen());

    m_buffer.Encode(sequence);

    MMRESULT result;
    result = m_pStream->Stop();
    if (result != MMSYSERR_NOERROR)
        return FALSE;
    result = m_pStream->Out(m_buffer);
    if (result != MMSYSERR_NOERROR)
        return FALSE;
    result = m_pStream->Restart();
    if (result != MMSYSERR_NOERROR)
        return FALSE;

    m_state = Playing;

    return TRUE;
}

BOOL Sequencer::Stop()
{
    ASSERT(m_pStream != NULL);
    ASSERT(m_pStream->IsOpen());

    if (m_pStream->Stop() != MMSYSERR_NOERROR)
        return FALSE;

    m_state = Stopped;

    return TRUE;
}

void Sequencer::StreamProc(HMIDISTRM hMidiStream, UINT uMsg, DWORD dwInstance,
                           DWORD dwParam1, DWORD dwParam2)
{
    if (uMsg != MOM_DONE)
        return;

    MidiStream *pStream = (MidiStream *)dwInstance;
    ASSERT(pStream != NULL);

    Sequencer *pThis = pStream->GetSequencer();
    ASSERT(pThis != NULL);

    // Unprepare the midi header
    ::midiOutUnprepareHeader(
        (HMIDIOUT)hMidiStream,
        (LPMIDIHDR)dwParam1,
        sizeof(MIDIHDR));

    pThis->m_state = Stopped;
}
