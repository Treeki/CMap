#include "cedittoolbox.h"
#include "cmapwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

CEditToolbox::CEditToolbox(QWidget *parent) :
    QWidget(parent),
    m_mapWidget(0) {
	QVBoxLayout *layout = new QVBoxLayout;

	// This is not very nice :|
	m_toolbar = new QToolBar(this);
	m_toolActions = new QActionGroup(this);
	m_brushToolAction = m_toolbar->addAction("Brush");
	m_lineToolAction = m_toolbar->addAction("Line");

	m_toolActions->addAction(m_brushToolAction);
	m_toolActions->addAction(m_lineToolAction);
	foreach (QAction *act, m_toolActions->actions())
		act->setCheckable(true);
	m_brushToolAction->setChecked(true);

	connect(m_toolActions, SIGNAL(triggered(QAction*)), SLOT(toolActionActivated(QAction*)));

	m_objectPicker = new CObjectPicker(this);

	layout->addWidget(m_toolbar);
	layout->addWidget(m_objectPicker, 1);

	setLayout(layout);
}

CEditToolbox::~CEditToolbox() {
}


void CEditToolbox::toolActionActivated(QAction *action) {
	if (action == m_brushToolAction)
		m_mapWidget->setTool(m_brushTool);
	else if (action == m_lineToolAction)
		m_mapWidget->setTool(m_lineTool);
}


void CEditToolbox::setup(CMapWidget *mw) {
	// this is a bit weird, should refactor it later maybe?
	if (m_mapWidget)
		cleanup();

	m_mapWidget = mw;

	m_brushTool = new CBrushTool(this);
	m_brushTool->setMapWidget(mw);

	m_lineTool = new CLineTool(this);
	m_lineTool->setMapWidget(mw);

	mw->setTool(m_brushTool);

	m_objectPicker->setup(mw->patches());

	connect(m_objectPicker, SIGNAL(selectionChanged(CMap::ObjectType,int)), m_brushTool, SLOT(setWhat(CMap::ObjectType,int)));
	connect(m_objectPicker, SIGNAL(selectionChanged(CMap::ObjectType,int)), m_lineTool, SLOT(setWhat(CMap::ObjectType,int)));

	connect(m_brushTool, SIGNAL(shapePicked(int)), m_objectPicker, SLOT(setSelectedShape(int)));
	connect(m_lineTool, SIGNAL(shapePicked(int)), m_objectPicker, SLOT(setSelectedShape(int)));
}


void CEditToolbox::cleanup() {
	delete m_brushTool;
	delete m_lineTool;
}
