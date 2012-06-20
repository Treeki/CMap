#ifndef CSHAPEMODEL_H
#define CSHAPEMODEL_H

#include <QAbstractListModel>
#include "cshapepool.h"
#include "cshapeindexer.h"

class CShapeModel : public QAbstractListModel {
	Q_OBJECT
public:
	explicit CShapeModel(CShapePool *pool = 0, CShapeIndexer *indexer = 0, QObject *parent = 0);

	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;

	void setPoolAndIndexer(CShapePool *pool, CShapeIndexer *indexer);

protected:
	CShapePool *m_pool;
	CShapeIndexer *m_indexer;

signals:
	
public slots:
	
};

#endif // CSHAPEMODEL_H
