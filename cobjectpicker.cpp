#include "cobjectpicker.h"
#include <QVBoxLayout>

CObjectPicker::CObjectPicker(QWidget *parent) : QWidget(parent) {
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);

	m_pickers[0] = new CShapePicker(CMap::Item, this);
	m_pickers[1] = new CShapePicker(CMap::Floor, this);
	m_pickers[2] = new CShapePicker(CMap::Wall, this);
	m_pickers[3] = new CShapePicker(CMap::Region, this);
	m_pickers[4] = new CShapePicker(CMap::Effect, this);

	m_tabs = new QTabWidget(this);
	m_tabs->setDocumentMode(true);

	m_tabs->addTab(m_pickers[0], QIcon(":/icons/item.png"), "");
	m_tabs->addTab(m_pickers[1], QIcon(":/icons/floor.png"), "");
	m_tabs->addTab(m_pickers[2], QIcon(":/icons/wall.png"), "");
	m_tabs->addTab(m_pickers[3], QIcon(":/icons/region.png"), "");
	m_tabs->addTab(m_pickers[4], QIcon(":/icons/effect.png"), "");

	connect(m_tabs, SIGNAL(currentChanged(int)), SLOT(handleCurrentTabChanged(int)));

	for (int i = 0; i < 5; i++)
		connect(m_pickers[i], SIGNAL(selectedShapeChanged(int)), SLOT(handleSelectedShapeChanged(int)));

	layout->addWidget(m_tabs);

	setLayout(layout);
}


void CObjectPicker::setup(CPatchContext *context) {
	m_pickers[0]->setup(&context->itemIndexer);
	m_pickers[1]->setup(&context->floorIndexer);
	m_pickers[2]->setup(&context->wallIndexer);
	m_pickers[3]->setup(&context->regionIndexer);
	m_pickers[4]->setup(&context->effectIndexer);
}


void CObjectPicker::handleCurrentTabChanged(int index) {
	emit selectionChanged(m_pickers[index]->objectType(), m_pickers[index]->selectedShape());
}

void CObjectPicker::handleSelectedShapeChanged(int shape) {
	emit selectionChanged(m_pickers[m_tabs->currentIndex()]->objectType(), shape);
}


void CObjectPicker::setSelectedShape(int newShape) {
	m_pickers[m_tabs->currentIndex()]->setSelectedShape(newShape);
}



void CObjectPicker::prevObject() {
	jumpObjectsBy(-1);
}
void CObjectPicker::nextObject() {
	jumpObjectsBy(1);
}
void CObjectPicker::prev8Objects() {
	jumpObjectsBy(-8);
}
void CObjectPicker::next8Objects() {
	jumpObjectsBy(8);
}

void CObjectPicker::jumpObjectsBy(int amount) {
	auto picker = m_pickers[m_tabs->currentIndex()];
	auto indexer = picker->indexer();

	int currentPickableIndex = indexer->pickableIndexForShapeNum(picker->selectedShape());
	int newPickableIndex = currentPickableIndex + amount;

	if (newPickableIndex < 0)
		newPickableIndex = 0;
	if (newPickableIndex >= indexer->pickableIndexCount())
		newPickableIndex = indexer->pickableIndexCount() - 1;

	picker->setSelectedShape(indexer->shapeNumForPickableIndex(newPickableIndex));
}
