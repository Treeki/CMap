#include "cshapeindexer.h"
#include "cshapepool.h"

CShapeIndexer::CShapeIndexer(CMap::ObjectType objectType, CShapePool *pool) :
	m_pool(pool), m_objectType(objectType) {
}


void CShapeIndexer::setup() {
	m_pickableIndices.clear();

	for (int i = 0; i < m_pool->max(); i++) {
		if (m_pool->exists(i))
			m_pickableIndices.append(i);
	}
}

int CShapeIndexer::imageNumForShape(int shapeNum) const {
	return shapeNum;
}

bool CShapeIndexer::shapeNumIsValid(int shapeNum) const {
	return m_pool->exists(shapeNum);
}

int CShapeIndexer::shapeNumForPickableIndex(int index) const {
	return m_pickableIndices.at(index);
}

int CShapeIndexer::pickableIndexCount() const {
	return m_pickableIndices.count();
}


bool CShapeIndexer::blankShapeZero() const {
	return (m_objectType != CMap::Floor);
}


/******************************************************************************/


CRegionShapeIndexer::CRegionShapeIndexer(CShapePool *pool) :
	CShapeIndexer(CMap::Region, pool) {
}

int CRegionShapeIndexer::imageNumForShape(int shapeNum) const {
	return (shapeNum == 0) ? -1 : ((shapeNum - 1) % 200);
}

bool CRegionShapeIndexer::shapeNumIsValid(int shapeNum) const {
	return (shapeNum >= 0) && (shapeNum <= MAX_REGION);
}

int CRegionShapeIndexer::shapeNumForPickableIndex(int index) const {
	return index;
}

int CRegionShapeIndexer::pickableIndexCount() const {
	return MAX_REGION + 1;
}


/******************************************************************************/


CWallShapeIndexer::CWallShapeIndexer(CShapePool *pool) :
	CShapeIndexer(CMap::Wall, pool) {
}

int CWallShapeIndexer::imageNumForShape(int shapeNum) const {
	// short-circuit this for simple cases
	if (shapeNum == 0)
		return -1;

	int texture = shapeNum / 12;
	int shape = shapeNum % 12;

	// nothing for invalid textures
	// note: Furcadia loads up to wall_20 (0..19), even though the game only
	// officially supports textures 1 to 17, but the extras will have the
	// default walkability properties. I suspect the client is loading the extra
	// files, but the server is ignoring them when reading the FBJ properties...
	if (texture > 19)
		return -1;

	// nothing for blank shapes
	// 2 is actually used for junction pieces, but I won't get into that now
	if (shape == 0 || shape == 2)
		return -1;

	// blank wall
	if (shape == 1 || shape == 3)
		return texture * 18;

	// something else
	return (texture * 18) + ((shape - 3) * 2);
}

bool CWallShapeIndexer::shapeNumIsValid(int shapeNum) const {
	if (shapeNum == 0)
		return false;

	if (shapeNum >= (12 * 20))
		return false;

	int shape = shapeNum % 12;
	return !(shape == 0 || shape == 2);
}


void CWallShapeIndexer::setup() {
	// this class piggybacks on m_pickableIndices but builds its own

	m_pickableIndices.clear();
	m_pickableIndices.append(0);

	for (int texture = 0; texture < 20; texture++) {
		for (int shape = 0; shape < 12; shape++) {
			if (shape == 0 || shape == 2)
				continue;

			int num = (texture * 12) + shape;
			if (m_pool->exists(imageNumForShape(num)))
				m_pickableIndices.append(num);
		}
	}
}
