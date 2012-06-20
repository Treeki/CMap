#include "cmappoint.h"

uint qHash(const CMapPoint &point) {
	return (point.x << 11) | point.y;
}

#include <QtDebug>

QDebug operator<<(QDebug dbg, const CMapPoint &point) {
	dbg.nospace() << "CMapPoint(" << point.x << "," << point.y << ")";
	return dbg.maybeSpace();
}
