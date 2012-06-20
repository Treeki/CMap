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

	m_tabs->addTab(m_pickers[0], "Items");
	m_tabs->addTab(m_pickers[1], "Floors");
	m_tabs->addTab(m_pickers[2], "Walls");
	m_tabs->addTab(m_pickers[3], "Regions");
	m_tabs->addTab(m_pickers[4], "Effects");

	connect(m_tabs, SIGNAL(currentChanged(int)), SLOT(handleCurrentTabChanged(int)));

	for (int i = 0; i < 5; i++)
		connect(m_pickers[i], SIGNAL(selectedItemChanged(int)), SLOT(handleSelectedItemChanged(int)));

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
	emit selectionChanged(m_pickers[index]->objectType(), m_pickers[index]->selectedItem());
}

void CObjectPicker::handleSelectedItemChanged(int item) {
	emit selectionChanged(m_pickers[m_tabs->currentIndex()]->objectType(), item);
}
