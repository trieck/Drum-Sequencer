#include "StdAfx.h"
#include "Sequence.h"

IMPLEMENT_SERIAL(Sequence, CObject, VERSIONABLE_SCHEMA)

ANON_BEGIN

// sequence file marker
constexpr BYTE SEQ_MARKER[3] = {
    'S',
    'E',
    'Q'
};

// MIDI instruments
constexpr BYTE INSTRUMENTS[Sequence::NINSTRUMENTS] = {
    42, // closed hi-hat
    44, // pedal hi-hat
    46, // open hi-hat
    49, // crash cymbal
    51, // ride cymbal
    38, // acoustic snare
    50, // hi tom
    48, // hi-mid tom
    45, // low tom
    36 // bass drum1
};

ANON_END

Sequence::Sequence() : m_bars(0), m_tsTop(0), m_tsBottom(0), m_resolution(0)
{
}

void Sequence::toggleSub(const CPoint& pt)
{
    ASSERT(!m_beats.empty());

    const auto x = pt.x % subdivisions();
    const auto y = pt.y % NINSTRUMENTS;

    auto index = y * subdivisions() + x;

    m_beats[index] = !m_beats[index];
}

void Sequence::create(int nbars, int tsTop, int tsBottom, int resolution)
{
    m_bars = nbars;
    m_tsTop = tsTop;
    m_tsBottom = tsBottom;
    m_resolution = resolution;

    auto size = NINSTRUMENTS * subdivisions();

    m_beats.clear();
    m_beats.resize(size);
}

bool Sequence::beat(int sub, int instrument) const
{
    sub = sub % subdivisions();
    instrument = instrument % NINSTRUMENTS;

    auto index = instrument * subdivisions() + sub;

    return m_beats[index];
}

Duration Sequence::delta() const
{
    return static_cast<Duration>(resolution());
}

int Sequence::bars() const
{
    return m_bars;
}

int Sequence::instrument(int i) const
{
    if (i < 0 || i >= NINSTRUMENTS)
        return 0;

    return INSTRUMENTS[i];
}

int Sequence::resolution() const
{
    return m_resolution;
}

int Sequence::subdivisions() const
{
    if (m_tsBottom == 0) {
        return 0;
    }
    return m_bars * m_tsTop * (m_resolution / m_tsBottom);
}

std::pair<int, int> Sequence::timeSig() const
{
    return { m_tsTop, m_tsBottom };
}

void Sequence::clear()
{
    m_bars = m_tsTop = m_tsBottom = m_resolution = 0;
    m_beats.clear();
}

void Sequence::Serialize(CArchive& ar)
{
    // FIXME
    AfxThrowNotSupportedException();

    BYTE buffer[3];

    if (ar.IsStoring()) {
        ar << SEQ_MARKER[0]; // 'S'
        ar << SEQ_MARKER[1]; // 'E'
        ar << SEQ_MARKER[2]; // 'Q'

        ar.Write(&m_beats, sizeof(m_beats));
    } else {
        // Is this a valid sequence archive
        ar.Read(buffer, 3);
        if (memcmp(buffer, SEQ_MARKER, 3) != 0)
            AfxThrowArchiveException(CArchiveException::badSchema);

        const auto nRead = ar.Read(&m_beats, sizeof(m_beats));
        if (nRead != sizeof(m_beats)) {
            AfxThrowArchiveException(CArchiveException::endOfFile);
        }
    }
}
