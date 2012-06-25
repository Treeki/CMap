#include "cshapepicker.h"
#include "cshapeitemdelegate.h"

#include <QListView>
#include <QGridLayout>

CShapePicker::CShapePicker(CMap::ObjectType objectType, QWidget *parent) :
	QWidget(parent), m_objectType(objectType),
	m_model(0), m_indexer(0) {

	QGridLayout *layout = new QGridLayout;

	m_view = new QListView(this);
	m_view->setItemDelegate(new CShapeItemDelegate(this));

	layout->addWidget(m_view, 0, 0, 1, 2);

	setLayout(layout);
}


void CShapePicker::setup(CShapeIndexer *indexer) {
	// clean up the old selection model if there's one,
	// apparently we have to do this!
	if (m_view->selectionModel())
		m_view->selectionModel()->deleteLater();

	if (m_model)
		m_model->deleteLater();

	m_indexer = indexer;

	m_model = new CShapeModel(indexer->pool(), indexer, this);

	m_view->setModel(m_model);
	m_view->setViewMode(QListView::IconMode);
	m_view->setResizeMode(QListView::Adjust);
	m_view->setUniformItemSizes(true);

	setSelectedShape(0);

	connect(m_view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(handleRowChanged(QModelIndex,QModelIndex)));
}


void CShapePicker::handleRowChanged(const QModelIndex &current, const QModelIndex &previous) {
	(void)previous;

	emit selectedItemChanged(m_indexer->shapeNumForPickableIndex(current.row()));
}

int CShapePicker::selectedItem() const {
	return m_indexer->shapeNumForPickableIndex(m_view->currentIndex().row());
}


void CShapePicker::setSelectedShape(int newShape) {
	m_view->selectionModel()->clearSelection();

	int canIndex = m_indexer->pickableIndexForShapeNum(newShape);
	if (canIndex > -1)
		m_view->selectionModel()->setCurrentIndex(m_model->index(canIndex), QItemSelectionModel::SelectCurrent);
}
