#include "cshapepool.h"

CShapePool::CShapePool() {
}


void CShapePool::setup(int max) {
	m_max = max;
	m_shapes.resize(max);
	m_shapeExistence.resize(max);
}


void CShapePool::applyPatch(CPatchFile &file, int src, int dest, int count) {
	int cSrc = src;
	int cDest = dest;

	int realCount = qMin(count, file.shapes.count());
	int endAt = dest + realCount;

	while (cDest < endAt) {
		CShape &shape = file.shapes[cSrc];

		int placeAt = (shape.replacedIndex == -1) ? cDest : (dest + shape.replacedIndex - src);

		if (placeAt >= 0 && placeAt < (dest + count) && placeAt < m_shapes.count()) {
			m_shapes[placeAt] = shape;
			m_shapeExistence.setBit(placeAt, true);
		}

		cSrc++;
		cDest++;
	}
}
