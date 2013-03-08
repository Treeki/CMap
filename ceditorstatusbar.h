#ifndef CEDITORSTATUSBAR_H
#define CEDITORSTATUSBAR_H

#include <QStatusBar>
struct CMapPoint;
class CMapWidget;
class QLabel;

class CEditorStatusBar : public QStatusBar
{
	Q_OBJECT
public:
	explicit CEditorStatusBar(CMapWidget *mapWidget, QWidget *parent = 0);
	
private:
	CMapWidget *m_mapWidget;

	QLabel *m_posLabel, *m_objLabel;

private slots:
	void hoverStatusChanged(
			bool fromValid, const CMapPoint &from,
			bool toValid, const CMapPoint &to,
			QMouseEvent *event);
};

#endif // CEDITORSTATUSBAR_H
