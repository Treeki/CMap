#include "cedittoolbox.h"
#include "cmapwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

CEditToolbox::CEditToolbox(QWidget *parent) :
    QWidget(parent),
    m_mapWidget(0) {
	QVBoxLayout *layout = new QVBoxLayout;

	m_objectPicker = new CObjectPicker(this);
	layout->addWidget(m_objectPicker, 1);

	setLayout(layout);
}

CEditToolbox::~CEditToolbox() {
}


QToolButton *CEditToolbox::createActionToolButton(QAction *action) {
	QToolButton *button = new QToolButton(this);
	button->setDefaultAction(action);
	return button;
}


void CEditToolbox::setup(CMapWidget *mw) {
	// this is a bit weird, should refactor it later maybe?
	if (m_mapWidget)
		cleanup();

	m_mapWidget = mw;

	m_brushTool = new CBrushTool(this);
	m_brushTool->setMapWidget(mw);

	mw->setTool(m_brushTool);

	m_objectPicker->setup(mw->patches());

	connect(m_objectPicker, SIGNAL(selectionChanged(CMap::ObjectType,int)), m_brushTool, SLOT(setWhat(CMap::ObjectType,int)));

	connect(m_brushTool, SIGNAL(shapePicked(int)), m_objectPicker, SLOT(setSelectedShape(int)));
}


void CEditToolbox::cleanup() {
	delete m_brushTool;
}
