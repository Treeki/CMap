#ifndef CBRUSHTOOL_H
#define CBRUSHTOOL_H

#include "ccommandtoolbase.h"
#include <QHash>

class CBrushTool : public CCommandToolBase {
	Q_OBJECT
public:
	explicit CBrushTool(QObject *parent = 0);

private:
	CMap::ObjectType m_whatType;
	int m_whatValue;

	QHash<int, int> m_paintedSoFar;

	CMapPoint m_hovered;
	int m_hoveredHalfX;

	int m_wallPaintDirection;

public:
	void tileMousePress(const CMapPoint &tile, QMouseEvent *event);
	void tileMouseAltAction(const CMapPoint &tile, QMouseEvent *event);

public slots:
	void setWhat(CMap::ObjectType type, int value);
signals:
	void shapePicked(int newValue);

public:
	CMap::ObjectType whatType() { return m_whatType; }
	int whatValue() { return m_whatValue; }

	int whatThingFor(CMap::ObjectType type, int x, int y, int normal);

	CMap::ObjectType typesToPreview();

	void hoverStatusChanged(bool fromValid, const CMapPoint &from, bool toValid, const CMapPoint &to, QMouseEvent *event);

protected:
	void finaliseCommand();
	
signals:
	
public slots:
	
};

#endif // CBRUSHTOOL_H
