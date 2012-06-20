#ifndef CSHAPEPOOL_H
#define CSHAPEPOOL_H

#include <QtCore/QVector>
#include <QtCore/QBitArray>
#include <cpatchfile.h>

class CShapePool {
public:
	CShapePool();

private:
	QVector<CShape> m_shapes;
	QBitArray m_shapeExistence;
	int m_max;


public:
	int max() { return m_max; }

	const CShape &operator[](int i) const { return m_shapes.at(i); }
	const CShape &shape(int i) const { return m_shapes.at(i); }
	bool exists(int i) const { return m_shapeExistence.testBit(i); }

	void setup(int max);
	void applyPatch(CPatchFile &file, int src, int dest, int count);
};

#endif // CSHAPEPOOL_H
