#ifndef CSHAPEMODEL_H
#define CSHAPEMODEL_H

#include <QAbstractListModel>
#include "cshapepool.h"

class CShapeModel : public QAbstractListModel {
	Q_OBJECT
public:
	explicit CShapeModel(CShapePool *pool = 0, QObject *parent = 0);

	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;

	void setPool(CShapePool *pool);

	int indexToNumber(int index) const { return m_validIndices.at(index); }
	
protected:
	CShapePool *m_pool;
	QVector<int> m_validIndices;

signals:
	
public slots:
	
};

#endif // CSHAPEMODEL_H
