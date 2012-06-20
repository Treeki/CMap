#include "cshapemodel.h"

CShapeModel::CShapeModel(CShapePool *pool, CShapeIndexer *indexer, QObject *parent) :
	QAbstractListModel(parent), m_pool(0), m_indexer(0) {

	if (pool && indexer)
		setPoolAndIndexer(pool, indexer);
}


void CShapeModel::setPoolAndIndexer(CShapePool *pool, CShapeIndexer *indexer) {
	m_pool = pool;
	m_indexer = indexer;
}


int CShapeModel::rowCount(const QModelIndex &parent) const {
	(void)parent;
	return m_indexer->pickableIndexCount();
}


QVariant CShapeModel::data(const QModelIndex &index, int role) const {
	int num = m_indexer->shapeNumForPickableIndex(index.row());

	switch (role) {
	case Qt::DisplayRole:
	case Qt::ToolTipRole:
		return QString::number(num);

	case Qt::DecorationRole: {
		if (num == 0 && m_indexer->blankShapeZero())
			return QVariant();

		int imageNum = m_indexer->imageNumForShape(num);
		if (imageNum == -1 || !m_pool->exists(imageNum))
			return QVariant();

		return m_pool->shape(imageNum).frames.first().pixmap;
	}

	case Qt::SizeHintRole:
		return QSize(64, 64);
	}

	return QVariant();
}
