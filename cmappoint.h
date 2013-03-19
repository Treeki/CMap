#ifndef CMAPPOINT_H
#define CMAPPOINT_H

#include <QtGlobal>
#include <functional>

enum OrdinalDirection {
	NORTH_WEST = 0,
	NORTH = 1,
	NORTH_EAST = 2,
	EAST = 3,
	SOUTH_EAST = 4,
	SOUTH = 5,
	SOUTH_WEST = 6,
	WEST = 7
};

struct CMapPoint {
	qint16 x, y;

	CMapPoint() { x = -30000; y = -30000; }
	CMapPoint(int _x, int _y) { x = _x; y = _y; }

	bool operator==(CMapPoint &other) const {
		return (x == other.x) && (y == other.y);
	}

	void setRightWall() {
		x |= 1;
	}

	void clearRightWall() {
		x &= ~1;
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

	void makeInvalid() {
		x = -30000;
		y = -30000;
	}

	bool isValid() const {
		return (x > -30000);
	}

	bool intersectsLine(int withX, int withY, int multiplier) const {
		int xDiff = withX - x;
		int yDiff = (withY - y) * multiplier;

		if ((y & 1) == 0) {
			if ((withY & 1) == 0)
				return (xDiff == yDiff);
			else
				return ((xDiff - 1) == yDiff);
		} else {
			if ((withY & 1) == 0)
				return ((xDiff + 1) == yDiff);
			else
				return (xDiff == yDiff);
		}
	}

	bool intersectsLineNWSE(int withX, int withY) const {
		return intersectsLine(withX, withY, 1);
	}
	bool intersectsLineNESW(int withX, int withY) const {
		return intersectsLine(withX, withY, -1);
	}

	int cartesianX() const {
		return ((x & ~1) + y) / 2;
	}
	int cartesianY() const {
		return (y - (x & ~1) + 1) >> 1;
	}

	// NOTE: This one *might* not work properly...
	void setCartesian(int cX, int cY) {
		int work = (cX - cY) + 1;
		x = work - (work % 2);
		y = cX + cY;
	}
};

enum LineAngle {
	INVALID_LINE_ANGLE,
	VERTICAL,
	HORIZONTAL,
	NW_SE,
	NE_SW
};

struct CLine {
	CMapPoint start, end;
	LineAngle angle;

	CLine() {
		angle = INVALID_LINE_ANGLE;
	}

	bool operator==(CLine &other) const {
		return (angle == other.angle) &&
				(start == other.start) && (end == other.end);
	}

	void makeInvalid() {
		angle = INVALID_LINE_ANGLE;
	}
	bool isValid() const {
		return (angle != INVALID_LINE_ANGLE);
	}

	void forEach(std::function<void(const CMapPoint &)> func) {
		CMapPoint iter;

		switch (angle) {
		case INVALID_LINE_ANGLE:
			break;

		case VERTICAL:
			for (iter = start; iter.y <= (end.y + 1); iter.y += 2)
				func(iter);

			break;

		case HORIZONTAL:
			for (iter = start; iter.x <= end.x; iter.x += 2)
				func(iter);

			break;

		case NW_SE:
			for (iter = start; iter.y <= end.y; iter.moveSE())
				func(iter);

			break;

		case NE_SW:
			for (iter = start; iter.y <= end.y; iter.moveSW())
				func(iter);

			break;
		}
	}

	int size() const {
		switch (angle) {
		case INVALID_LINE_ANGLE:
			return 0;
		case VERTICAL:
			return ((end.y - start.y) + 1) / 2; // not 100% checked, fyi
		case HORIZONTAL:
			return ((end.x - start.x) / 2) + 1;
		case NW_SE:
			return (end.cartesianX() - start.cartesianX()) + 1;
		case NE_SW:
			return (end.cartesianY() - start.cartesianY()) + 1;
		}
		return 0;
	}

	bool contains(int x, int y) const {
		switch (angle) {
		case INVALID_LINE_ANGLE:
			return false;
		case VERTICAL:
			return (x == start.x) && (y >= start.y) && (y <= end.y) && ((y & 1) == (start.y & 1));
		case HORIZONTAL:
			return (y == start.y) && (x >= start.x) && (x <= end.x);
		case NW_SE:
			return (y >= start.y) && (y <= end.y) && start.intersectsLineNWSE(x, y);
		case NE_SW:
			return (y >= start.y) && (y <= end.y) && start.intersectsLineNESW(x, y);
		}
		return false;
	}

	bool tryAssign(const CMapPoint &newStart, const CMapPoint &newEnd) {
		bool mustSwap = false;

		if (newStart.x == newEnd.x) {
			angle = VERTICAL;
			mustSwap = (newStart.y > newEnd.y);
		} else if (newStart.y == newEnd.y) {
			angle = HORIZONTAL;
			mustSwap = (newStart.x > newEnd.x);
		} else if (newStart.intersectsLineNWSE(newEnd.x, newEnd.y)) {
			angle = NW_SE;
			mustSwap = (newStart.y > newEnd.y);
		} else if (newStart.intersectsLineNESW(newEnd.x, newEnd.y)) {
			angle = NE_SW;
			mustSwap = (newStart.y > newEnd.y);
		} else {
			return false;
		}

		// if we got here, one of the angles must have been valid
		if (mustSwap) {
			end = newStart;
			start = newEnd;
		} else {
			start = newStart;
			end = newEnd;
		}
		return true;
	}


	void snapToPoints(const CMapPoint &anchor, const CMapPoint &other);
};

struct CDiamond {
	CMapPoint top, bottom;
};

uint qHash(const CMapPoint &point);
QDebug operator<<(QDebug dbg, const CMapPoint &point);
QDebug operator<<(QDebug dbg, const CLine &line);

#endif // CMAPPOINT_H
