#ifndef CTOOLBASE_H
#define CTOOLBASE_H

#include <QObject>
#include <QMouseEvent>

#include "ceditablemap.h"
class CMapWidget;

class CToolBase : public QObject {
	Q_OBJECT
public:
	explicit CToolBase(QObject *parent = 0);

	virtual CMap::ObjectType typesToPreview() { return CMap::NoObject; }

	virtual int whatFloorFor(int x, int y, int normal) { (void)x; (void)y; return normal; }
	virtual int whatItemFor(int x, int y, int normal) { (void)x; (void)y; return normal; }
	virtual int whatWallFor(int x, int y, int normal) { (void)x; (void)y; return normal; }
	virtual int whatRegionFor(int x, int y, int normal) { (void)x; (void)y; return normal; }
	virtual int whatEffectFor(int x, int y, int normal) { (void)x; (void)y; return normal; }

	virtual void hoverStatusChanged(
			bool fromValid, const CMapPoint &from,
			bool toValid, const CMapPoint &to,
			QMouseEvent *event) {
		(void)fromValid; (void)toValid;
		(void)from; (void)to;
		(void)event;
	}

	virtual void activate() { }
	virtual void deactivate() { }

	virtual void tileMousePress(const CMapPoint &tile, QMouseEvent *event) { (void)tile; (void)event; }
	virtual void tileMouseRelease(const CMapPoint &tile, QMouseEvent *event) { (void)tile; (void)event; }
	virtual void tileMouseAltAction(const CMapPoint &tile, QMouseEvent *event) { (void)tile; (void)event; }

private:
	CMapWidget *m_widget;

public:
	void setMapWidget(CMapWidget *mw) { m_widget = mw; }
	CMapWidget *widget() { return m_widget; }

signals:
	
public slots:
	
};

#endif // CTOOLBASE_H
