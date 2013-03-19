#include "cevenspinbox.h"

CEvenSpinBox::CEvenSpinBox(QWidget *parent) :
	QSpinBox(parent)
{
}

QValidator::State CEvenSpinBox::validate(QString &input, int &pos) const {
	auto origValue = QSpinBox::validate(input, pos);

	if (origValue == QValidator::Acceptable) {
		auto what = input.toInt();
		if ((what & 1) == 1)
			return QValidator::Intermediate;
	}

	return origValue;
}
