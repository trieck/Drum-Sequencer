#pragma once

#include "miditime.h"

class Sequence : public CObject
{
public:
    Sequence();
    ~Sequence() = default;

    bool beat(int sub, int instrument) const;
    Duration delta() const;
    int bars() const;
    int instrument(int i) const;
    int resolution() const;
    int subdivisions() const;
    std::pair<int, int> timeSig() const;
    void clear();
    void create(int nbars, int tsTop, int tsBottom, int resolution);
    void toggleSub(const CPoint& pt);
    enum { NINSTRUMENTS = 10 }; // number of instruments
   
private:
    int m_bars, m_tsTop, m_tsBottom, m_resolution;
    CArray<bool> m_beats;

DECLARE_SERIAL(Sequence)
    void Serialize(CArchive& ar) override;
};
