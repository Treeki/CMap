#ifndef CEDITTOOLBOX_H
#define CEDITTOOLBOX_H

#include <QWidget>
#include <QAction>
#include <QToolButton>
#include "cobjectpicker.h"

#include "cbrushtool.h"

class CMapWidget;

class CEditToolbox : public QWidget {
	Q_OBJECT
	
public:
	explicit CEditToolbox(QWidget *parent = 0);
	~CEditToolbox();

	void setup(CMapWidget *mw);
	void cleanup();
	
private:
	QToolButton *createActionToolButton(QAction *action);

	CObjectPicker *m_objectPicker;

	CMapWidget *m_mapWidget;

	CBrushTool *m_brushTool;
};

#endif // CEDITTOOLBOX_H
