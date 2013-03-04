#ifndef CSHAPEPICKER_H
#define CSHAPEPICKER_H

#include <QWidget>
#include "cmap.h"
#include "cshapemodel.h"

class QSpinBox;

class QListView;

class CShapePicker : public QWidget {
	Q_OBJECT
public:
	explicit CShapePicker(CMap::ObjectType objectType, QWidget *parent = 0);

	void setup(CShapeIndexer *indexer);

protected:
	CShapeModel *m_model;
	CShapeIndexer *m_indexer;

	QSpinBox *m_typeSpinBox;
	QListView *m_view;

	CMap::ObjectType m_objectType;
	int m_selectedShape;
	
public:
	CMap::ObjectType objectType() const { return m_objectType; }
	int selectedShape() const { return m_selectedShape; }

	CShapeIndexer *indexer() const { return m_indexer; }

public slots:
	void setSelectedShape(int newShape);
signals:
	void selectedShapeChanged(int shape);
	
protected slots:
	void handleRowChanged(const QModelIndex &current, const QModelIndex &previous);
};

#endif // CSHAPEPICKER_H
