#pragma once
class Sequence : public CObject {
public:
	Sequence(void);
	~Sequence(void);
	void ToggleSub(const CPoint& pt);

	bool GetBeat(int x, int y) const;
	void Clear(void);
	BYTE GetInstrument(int i) const;

	enum { NINSTRUMENTS = 10 };	// number of instruments
	enum { NBARS = 1 };			// number of bars
	enum { RESOLUTION = 4 };	// number of subdivisions per beat
	enum { BEATS_PER_BAR = 4 };	// number of beats per bar
	enum { NSUBS = RESOLUTION * BEATS_PER_BAR * NBARS };	// number of subdivisions

private:
	bool m_beats[NINSTRUMENTS][NSUBS];
	DECLARE_SERIAL(Sequence)
public:
	virtual void Serialize(CArchive& ar);
};
