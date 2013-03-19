#ifndef CFILLTOOL_H
#define CFILLTOOL_H

#include "ccommandtoolbase.h"

class CFillTool : public CCommandToolBase
{
	Q_OBJECT
public:
	explicit CFillTool(QObject *parent = 0);

private:
	CMap::ObjectType m_whatType;
	int m_whatValue;

	CMapPoint m_hovered;
	int m_hoveredHalfX;

public:
	void tileMouseAltAction(const CMapPoint &tile, QMouseEvent *event);

public slots:
	void setWhat(CMap::ObjectType type, int value);
signals:
	void shapePicked(int newValue);

public:
	CMap::ObjectType whatType() const { return m_whatType; }
	int whatValue() const { return m_whatValue; }

	int whatThingFor(CMap::ObjectType type, int x, int y, int normal) const;

	CMap::ObjectType typesToPreview() const;

	void hoverStatusChanged(bool fromValid, const CMapPoint &from, bool toValid, const CMapPoint &to, QMouseEvent *event);

protected:
	void finaliseCommand();

private:
	void doFill();
signals:

public slots:

};

#endif // CFILLTOOL_H
