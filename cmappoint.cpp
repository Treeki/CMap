#include "cmappoint.h"
#include <cmath>

uint qHash(const CMapPoint &point) {
	return (point.x << 11) | point.y;
}

#include <QtDebug>

QDebug operator<<(QDebug dbg, const CMapPoint &point) {
	dbg.nospace() << "CMapPoint(" << point.x << "," << point.y << ")";
	return dbg.maybeSpace();
}

QDebug operator<<(QDebug dbg, const CLine &line) {
	const char *angleString = "_";
	switch (line.angle) {
	case INVALID_LINE_ANGLE: angleString = "INVALID_LINE_ANGLE"; break;
	case VERTICAL: angleString = "VERTICAL"; break;
	case HORIZONTAL: angleString = "HORIZONTAL"; break;
	case NW_SE: angleString = "NW_SE"; break;
	case NE_SW: angleString = "NE_SW"; break;
	}

	dbg.nospace() <<
					 "CLine(" << angleString << "," <<
					 line.start << "," << line.end << ")";
	return dbg.maybeSpace();
}



void CLine::snapToPoints(const CMapPoint &anchor, const CMapPoint &other) {
	int xDiff = anchor.cartesianX() - other.cartesianX();
	int yDiff = anchor.cartesianY() - other.cartesianY();

	double angle = std::atan2(double(yDiff), double(xDiff)) * (180 / 3.14159265);

	// 0: North West
	// 45: North
	// 90: North East
	// 135: East
	// 180: South East
	// -135: South
	// -90: South West
	// -45: West

	// This code is kind of crap. Oh well. It works.. hopefully?
	int rawDirection = int(floor((angle + 22.5) / 45.0));
	OrdinalDirection direction;
	if (rawDirection >= 0)
		direction = (OrdinalDirection)rawDirection;
	else
		direction = (OrdinalDirection)(8 + rawDirection);

	int cartLength = std::floor(std::sqrt(double((xDiff*xDiff) + (yDiff*yDiff))));

	int isoXDiff = (anchor.x - other.x) / 2;
	int isoYDiff = (anchor.y - other.y) / 2;
	int isoLength = std::floor(std::sqrt(double((isoXDiff*isoXDiff) + (isoYDiff*isoYDiff))));

	CMapPoint endPoint = anchor;

	switch (direction) {
	// Simple isometric directions
	case NORTH_WEST:
		for (int i = 0; i < cartLength; i++)
			endPoint.moveNW();
		break;
	case NORTH_EAST:
		for (int i = 0; i < cartLength; i++)
			endPoint.moveNE();
		break;
	case SOUTH_EAST:
		for (int i = 0; i < cartLength; i++)
			endPoint.moveSE();
		break;
	case SOUTH_WEST:
		for (int i = 0; i < cartLength; i++)
			endPoint.moveSW();
		break;
	// Cardinal directions
	case WEST:
		endPoint.x -= (isoLength * 2);
		break;
	case EAST:
		endPoint.x += (isoLength * 2);
		break;
	case NORTH:
		endPoint.y -= (isoLength * 2);
		break;
	case SOUTH:
		endPoint.y += (isoLength * 2);
		break;
	}

	tryAssign(anchor, endPoint);
}

