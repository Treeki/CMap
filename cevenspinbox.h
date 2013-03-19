#ifndef CEVENSPINBOX_H
#define CEVENSPINBOX_H

#include <QSpinBox>

class CEvenSpinBox : public QSpinBox {
	Q_OBJECT
public:
	explicit CEvenSpinBox(QWidget *parent = 0);
	
protected:
	QValidator::State validate(QString &input, int &pos) const;
};

#endif // CEVENSPINBOX_H
