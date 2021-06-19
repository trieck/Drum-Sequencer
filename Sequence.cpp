#include "StdAfx.h"
#include "Sequence.h"

IMPLEMENT_SERIAL(Sequence, CObject, VERSIONABLE_SCHEMA)

ANON_BEGIN

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
    ASSERT(!m_beats.IsEmpty());

    const auto nsub = subdivisions();
    if (nsub > 0) {
        const auto x = pt.x % nsub;
        const auto y = pt.y % NINSTRUMENTS;
        const auto index = y * nsub + x;

        m_beats[index] = !m_beats[index];
    }
}

void Sequence::create(int nbars, int tsTop, int tsBottom, int resolution)
{
    m_bars = nbars;
    m_tsTop = tsTop;
    m_tsBottom = tsBottom;
    m_resolution = resolution;

    auto size = NINSTRUMENTS * subdivisions();

    m_beats.RemoveAll();
    m_beats.SetSize(size);
}

bool Sequence::beat(int sub, int instrument) const
{
    auto ndivs = subdivisions();
    if (ndivs == 0) {
        return false;
    }

    sub %= ndivs;
    instrument %= NINSTRUMENTS;

    auto index = instrument * ndivs + sub;

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
    m_beats.RemoveAll();
}

void Sequence::Serialize(CArchive& ar)
{
    ar.SerializeClass(RUNTIME_CLASS(Sequence));

    if (ar.IsStoring()) {
        ar << m_bars;
        ar << m_tsTop;
        ar << m_tsBottom;
        ar << m_resolution;
    } else {
        ar >> m_bars;
        ar >> m_tsTop;
        ar >> m_tsBottom;
        ar >> m_resolution;
    }

    m_beats.Serialize(ar);
}
