#include "cshapemodel.h"

CShapeModel::CShapeModel(CShapePool *pool, QObject *parent) :
	QAbstractListModel(parent), m_pool(0) {

	if (pool)
		setPool(pool);
}


void CShapeModel::setPool(CShapePool *pool) {
	m_pool = pool;

	m_validIndices.clear();

	for (int i = 0; i < pool->max(); i++) {
		if (pool->exists(i))
			m_validIndices.append(i);
	}
}


int CShapeModel::rowCount(const QModelIndex &parent) const {
	(void)parent;
	return m_validIndices.count();
}


QVariant CShapeModel::data(const QModelIndex &index, int role) const {
	int num = m_validIndices.at(index.row());

	switch (role) {
	case Qt::DisplayRole:
	case Qt::ToolTipRole:
		return QString("Shape %1").arg(num);
	case Qt::DecorationRole:
		if (num == 0) return QVariant();
		return m_pool->shape(num).frames.first().pixmap;
	case Qt::SizeHintRole:
		return QSize(64, 64);
	}

	return QVariant();
}
