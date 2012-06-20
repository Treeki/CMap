#include "cshapepicker.h"

#include <QListView>
#include <QGridLayout>

CShapePicker::CShapePicker(CMap::ObjectType objectType, QWidget *parent) :
	QWidget(parent), m_objectType(objectType),
	m_model(0) {

	QGridLayout *layout = new QGridLayout;

	m_view = new QListView(this);

	layout->addWidget(m_view, 0, 0, 1, 2);

	setLayout(layout);
}


void CShapePicker::setup(CShapePool *pool) {
	// clean up the old selection model if there's one,
	// apparently we have to do this!
	if (m_view->selectionModel())
		m_view->selectionModel()->deleteLater();

	if (m_model)
		m_model->deleteLater();

	m_model = new CShapeModel(pool, this);

	m_view->setModel(m_model);
	m_view->setViewMode(QListView::IconMode);
	m_view->setResizeMode(QListView::Adjust);
	m_view->setUniformItemSizes(true);

	m_view->selectionModel()->clearSelection();
	m_view->selectionModel()->setCurrentIndex(m_model->index(0), QItemSelectionModel::SelectCurrent);

	connect(m_view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(handleRowChanged(QModelIndex,QModelIndex)));
}


void CShapePicker::handleRowChanged(const QModelIndex &current, const QModelIndex &previous) {
	(void)previous;

	emit selectedItemChanged(m_model->indexToNumber(current.row()));
}

int CShapePicker::selectedItem() const {
	return m_model->indexToNumber(m_view->currentIndex().row());
}
