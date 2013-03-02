#ifndef CEDITTOOLBOX_H
#define CEDITTOOLBOX_H

#include <QWidget>
#include <QAction>
#include <QToolButton>
#include <QToolBar>
#include "cobjectpicker.h"

#include "cbrushtool.h"
#include "clinetool.h"

class CMapWidget;

class CEditToolbox : public QWidget {
	Q_OBJECT
	
public:
	explicit CEditToolbox(QWidget *parent = 0);
	~CEditToolbox();

	void setup(CMapWidget *mw);
	void cleanup();
	
private:
	QToolBar *m_toolbar;
	QActionGroup *m_toolActions;
	QAction *m_brushToolAction, *m_lineToolAction;

	CObjectPicker *m_objectPicker;

	CMapWidget *m_mapWidget;

	CBrushTool *m_brushTool;
	CLineTool *m_lineTool;

private slots:
	void toolActionActivated(QAction *action);
};

#endif // CEDITTOOLBOX_H
