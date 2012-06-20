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
	void handleSelectedItemChanged(int item);
	void handleCurrentTabChanged(int index);

signals:
	void selectionChanged(CMap::ObjectType objectType, int value);
	
public slots:
	
};

#endif // COBJECTPICKER_H
