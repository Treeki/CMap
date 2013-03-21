#ifndef CMAPWIDGET_H
#define CMAPWIDGET_H

#include <QWidget>
#include <QAbstractScrollArea>
#include "ceditablemap.h"
#include "cpatchcontext.h"
#include "ctoolbase.h"

class CMapWidget : public QWidget {
	Q_OBJECT
public:
	explicit CMapWidget(QWidget *parent = 0);

private:
	CEditableMap *m_map;
	CPatchContext *m_patches;

	QAbstractScrollArea *m_scrollArea;

public:
	void setScrollArea(QAbstractScrollArea *scrollArea) { m_scrollArea = scrollArea; }

	CEditableMap *map() const { return m_map; }
	void setMap(CEditableMap *newMap);

	CPatchContext *patches() const { return m_patches; }
	void setPatches(CPatchContext *newPatches);

protected:
	void paintEvent(QPaintEvent *);
	void paintShape(QPainter &painter, const QRect &drawRect, int x, int y, CShapePool &pool, int shapeNum, int xOffset = 0, int yOffset = 0);

	// Updating
public slots:
	void updateTile(int x, int y, CEditableMap::UpdateType type = CEditableMap::EverythingUpdate);
	void updateTile(const CMapPoint &pos, CEditableMap::UpdateType type = CEditableMap::EverythingUpdate) {
		if (!pos.isValid())
			return;

		if (type & CEditableMap::WallUpdate) {
			type = (CEditableMap::UpdateType)(type & ~CEditableMap::WallUpdate);
			type = (CEditableMap::UpdateType)(type | ((pos.x & 1) ? CEditableMap::RightWallUpdate : CEditableMap::LeftWallUpdate));
		}

		updateTile(pos.x / 2, pos.y, type);
	}

	// Zoom
private:
	float m_zoomFactorX, m_zoomFactorY;

public:
	void setZoomLevel(float x, float y);
	void setZoomLevel(float level) {
		setZoomLevel(level, level);
	}

public slots:
	void updateWorld(int x, int y, int w, int h);

public:
	void fixWidgetSize();

	// Editing
protected:
	CToolBase *m_currentTool;
public:
	void setTool(CToolBase *tool);

	QImage mouseMapImage;
	QPixmap highlightPixmap, highlightTransPixmap;
	QPixmap walkingBorderPixmap;

	bool isHovering;
	CMapPoint hovered;

protected:
	CMapPoint calculateClickedPosition(const QPoint &rawScreenPos);

	bool m_isDraggingMap;
	QPoint m_dragStart;
	float m_roughDragDistance;

	bool m_showWalkingBorders;

	bool m_showItems, m_showFloors, m_showWalls;
	bool m_showRegions, m_showEffects;

protected:
	void mouseMoveEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void leaveEvent(QEvent *);

signals:
	void hoverStatusChanged(
			bool fromValid, const CMapPoint &from,
			bool toValid, const CMapPoint &to,
			QMouseEvent *event);

	void copyPositionToClipboard(const CMapPoint &pos);

public slots:
	void setWalkingBordersShown(bool value);

	void setItemsShown(bool value);
	void setFloorsShown(bool value);
	void setWallsShown(bool value);
	void setRegionsShown(bool value);
	void setEffectsShown(bool value);
public:
	bool walkingBordersShown() const { return m_showWalkingBorders; }

	bool itemsShown() const { return m_showItems; }
	bool floorsShown() const { return m_showFloors; }
	bool wallsShown() const { return m_showWalls; }
	bool regionsShown() const { return m_showRegions; }
	bool effectsShown() const { return m_showEffects; }
};

#endif // CMAPWIDGET_H
