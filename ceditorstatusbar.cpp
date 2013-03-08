#include "ceditorstatusbar.h"
#include "cmapwidget.h"
#include <QLabel>

CEditorStatusBar::CEditorStatusBar(CMapWidget *mapWidget, QWidget *parent) :
	QStatusBar(parent) {

	m_objLabel = new QLabel(this);
	m_posLabel = new QLabel(this);

	insertPermanentWidget(0, m_objLabel);
	insertPermanentWidget(1, m_posLabel);

	m_mapWidget = mapWidget;
	connect(mapWidget,
			SIGNAL(hoverStatusChanged(bool,CMapPoint,bool,CMapPoint,QMouseEvent*)),
			SLOT(hoverStatusChanged(bool,CMapPoint,bool,CMapPoint,QMouseEvent*)));
}

void CEditorStatusBar::hoverStatusChanged(
		bool fromValid, const CMapPoint &from,
		bool toValid, const CMapPoint &to, QMouseEvent *event) {

	(void)fromValid;
	(void)from;
	(void)event;

	if (toValid) {
		auto map = m_mapWidget->map();

		m_posLabel->setText(QStringLiteral("(%1, %2)").arg(to.x).arg(to.y));
		m_objLabel->setText(
					QStringLiteral("Item: %1 - Floor: %2 - Wall: %3 - Region: %4 - Effect: %5")
					.arg(map->items.get(to))
					.arg(map->floors.get(to))
					.arg(map->walls.get(to))
					.arg(map->regions.get(to))
					.arg(map->effects.get(to)));
	} else {
		m_posLabel->setText(QStringLiteral("-"));
		m_objLabel->setText(QStringLiteral(""));
	}
}
