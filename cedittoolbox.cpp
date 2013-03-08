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
	m_toolbar->setIconSize(QSize(16, 16));
	m_toolActions = new QActionGroup(this);
	m_brushToolAction = m_toolbar->addAction(QIcon(":/icons/brush.png"), "Brush");
	m_lineToolAction = m_toolbar->addAction(QIcon(":/icons/line.png"), "Line");

	m_toolActions->addAction(m_brushToolAction);
	m_toolActions->addAction(m_lineToolAction);
	foreach (QAction *act, m_toolActions->actions())
		act->setCheckable(true);
	m_brushToolAction->setChecked(true);

	connect(m_toolActions, SIGNAL(triggered(QAction*)), SLOT(toolActionActivated(QAction*)));

	m_objectPicker = new CObjectPicker(this);

	layout->setContentsMargins(2, 0, 2, 0);
	layout->addWidget(m_toolbar);
	layout->addWidget(m_objectPicker, 1);

	setupKeyActions();

	setLayout(layout);
}

CEditToolbox::~CEditToolbox() {
}


void CEditToolbox::setupKeyActions() {
	addAction(m_prevObjAction = new QAction("Previous Object", this));
	m_prevObjAction->setShortcut(Qt::Key_Minus);

	addAction(m_nextObjAction = new QAction("Next Object", this));
	QList<QKeySequence> nextObjKeys = {Qt::Key_Equal, Qt::Key_Plus};
	m_nextObjAction->setShortcuts(nextObjKeys);

	addAction(m_prev8ObjAction = new QAction("Previous 8 Objects", this));
	m_prev8ObjAction->setShortcut(Qt::Key_Comma);

	addAction(m_next8ObjAction = new QAction("Next 8 Objects", this));
	m_next8ObjAction->setShortcut(Qt::Key_Period);

	connect(m_prevObjAction, SIGNAL(triggered()), m_objectPicker, SLOT(prevObject()));
	connect(m_nextObjAction, SIGNAL(triggered()), m_objectPicker, SLOT(nextObject()));
	connect(m_prev8ObjAction, SIGNAL(triggered()), m_objectPicker, SLOT(prev8Objects()));
	connect(m_next8ObjAction, SIGNAL(triggered()), m_objectPicker, SLOT(next8Objects()));
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

	m_objectPicker->setup(mw->patches());

	m_brushTool = new CBrushTool(this);
	m_brushTool->setMapWidget(mw);
	m_brushTool->setWhat(m_objectPicker->objectType(), m_objectPicker->selectedShape());

	m_lineTool = new CLineTool(this);
	m_lineTool->setMapWidget(mw);
	m_lineTool->setWhat(m_objectPicker->objectType(), m_objectPicker->selectedShape());

	mw->setTool(m_brushTool);

	connect(m_objectPicker, SIGNAL(selectionChanged(CMap::ObjectType,int)), m_brushTool, SLOT(setWhat(CMap::ObjectType,int)));
	connect(m_objectPicker, SIGNAL(selectionChanged(CMap::ObjectType,int)), m_lineTool, SLOT(setWhat(CMap::ObjectType,int)));

	connect(m_brushTool, SIGNAL(shapePicked(int)), m_objectPicker, SLOT(setSelectedShape(int)));
	connect(m_lineTool, SIGNAL(shapePicked(int)), m_objectPicker, SLOT(setSelectedShape(int)));
}


void CEditToolbox::cleanup() {
	if (m_mapWidget)
		m_mapWidget->setTool(0);
	delete m_brushTool;
	delete m_lineTool;
}
