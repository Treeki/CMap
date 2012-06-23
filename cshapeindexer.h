#ifndef CSHAPEINDEXER_H
#define CSHAPEINDEXER_H

#include "cmap.h"
#include "cshapepool.h"

class CShapeIndexer {
public:
	CShapeIndexer(CMap::ObjectType objectType, CShapePool *pool);

	virtual void setup();
	virtual int imageNumForShape(int shapeNum) const;
	virtual bool shapeNumIsValid(int shapeNum) const;

	CShapePool *pool() const { return m_pool; }

	CMap::ObjectType objectType() const { return m_objectType; }

	virtual int shapeNumForPickableIndex(int index) const;
	virtual int pickableIndexCount() const;
	virtual int pickableIndexForShapeNum(int index) const;

	virtual bool blankShapeZero() const;

protected:
	CShapePool *m_pool;
	CMap::ObjectType m_objectType;

	QVector<int> m_pickableIndices;
	QHash<int, int> m_pickableIndexReverseLookup;

	void resetPickableIndices();
	void addPickableIndex(int shapeNum);
};

class CRegionShapeIndexer : public CShapeIndexer {
public:
	CRegionShapeIndexer(CShapePool *pool);

	int imageNumForShape(int shapeNum) const;
	bool shapeNumIsValid(int shapeNum) const;

	int shapeNumForPickableIndex(int index) const;
	int pickableIndexCount() const;
	int pickableIndexForShapeNum(int index) const;
};

class CWallShapeIndexer : public CShapeIndexer {
public:
	CWallShapeIndexer(CShapePool *pool);

	void setup();

	int imageNumForShape(int shapeNum) const;
	bool shapeNumIsValid(int shapeNum) const;
};

#endif // CSHAPEINDEXER_H
