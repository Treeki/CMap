#include "cshapepicker.h"
#include "cshapeitemdelegate.h"

#include <QListView>
#include <QSpinBox>
#include <QGridLayout>
#include <QLabel>

CShapePicker::CShapePicker(CMap::ObjectType objectType, QWidget *parent) :
	QWidget(parent), m_objectType(objectType),
	m_model(0), m_indexer(0) {

	QGridLayout *layout = new QGridLayout;
	layout->setContentsMargins(4, 4, 4, 4);

	m_view = new QListView(this);
	m_view->setItemDelegate(new CShapeItemDelegate(this));

	m_typeSpinBox = new QSpinBox(this);

	layout->addWidget(m_view, 0, 0, 1, 2);
	layout->addWidget(new QLabel("Number:", this), 1, 0, 1, 1);
	layout->addWidget(m_typeSpinBox, 1, 1, 1, 1);

	setLayout(layout);

	// hook up all signals that don't depend on the ShapeModel or
	// SelectionModel (as we'll do that in setup() later)
	connect(this, SIGNAL(selectedShapeChanged(int)), m_typeSpinBox, SLOT(setValue(int)));
	connect(m_typeSpinBox, SIGNAL(valueChanged(int)), SLOT(setSelectedShape(int)));
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

	m_typeSpinBox->setRange(0, indexer->pool()->max() - 1);

	m_selectedShape = (m_objectType == CMap::Floor) ? 0 : 1;
	setSelectedShape(m_selectedShape);

	connect(m_view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(handleRowChanged(QModelIndex,QModelIndex)));
}


void CShapePicker::handleRowChanged(const QModelIndex &current, const QModelIndex &previous) {
	(void)previous;

	if (current.isValid()) {
		m_selectedShape = m_indexer->shapeNumForPickableIndex(current.row());
		emit selectedShapeChanged(m_selectedShape);
	}
}


void CShapePicker::setSelectedShape(int newShape) {
	m_view->selectionModel()->clearSelection();

	int canIndex = m_indexer->pickableIndexForShapeNum(newShape);
	if (canIndex > -1) {
		m_view->selectionModel()->setCurrentIndex(m_model->index(canIndex), QItemSelectionModel::SelectCurrent);
	} else if (newShape >= 0 && newShape < m_indexer->pool()->max()) {
		m_selectedShape = newShape;
		emit selectedShapeChanged(m_selectedShape);
	}
}
