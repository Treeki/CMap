#ifndef COBJECTPICKER_H
#define COBJECTPICKER_H

#include <QWidget>
#include <QTabWidget>
#include "cshapepicker.h"
#include "cpatchcontext.h"

class CObjectPicker : public QWidget {
	Q_OBJECT
public:
	explicit CObjectPicker(QWidget *parent = 0);

	void setup(CPatchContext *context);
	
protected:
	CShapePicker *m_pickers[5];
	QTabWidget *m_tabs;

protected slots:
	void handleSelectedShapeChanged(int shape);
	void handleCurrentTabChanged(int index);

signals:
	void selectionChanged(CMap::ObjectType objectType, int value);
	
public slots:
	void prevObject();
	void nextObject();
	void prev8Objects();
	void next8Objects();

	void jumpObjectsBy(int amount);

	void setSelectedShape(int newShape);
	
public:
	CMap::ObjectType objectType() const {
		return m_pickers[m_tabs->currentIndex()]->objectType();
	}
	int selectedShape() const {
		return m_pickers[m_tabs->currentIndex()]->selectedShape();
	}
};

#endif // COBJECTPICKER_H
