#include "cmappoint.h"

uint qHash(const CMapPoint &point) {
	return (point.x << 11) | point.y;
}
