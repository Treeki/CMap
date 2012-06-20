#ifndef CMAPPOINT_H
#define CMAPPOINT_H

#include <QtGlobal>

struct CMapPoint {
	quint16 x, y;

	CMapPoint() { }
	CMapPoint(int _x, int _y) { x = _x; y = _y; }

	bool operator==(CMapPoint &other) const {
		return (x == other.x) && (y == other.y);
	}

	void moveNW() {
		if (y & 1)
			x -= 2;
		y--;
	}

	void moveNE() {
		if (!(y & 1))
			x += 2;
		y--;
	}

	void moveSW() {
		if (y & 1)
			x -= 2;
		y++;
	}

	void moveSE() {
		if (!(y & 1))
			x += 2;
		y++;
	}
};

uint qHash(const CMapPoint &point);
QDebug operator<<(QDebug dbg, const CMapPoint &point);

#endif // CMAPPOINT_H
