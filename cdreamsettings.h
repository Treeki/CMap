#ifndef CDREAMSETTINGS_H
#define CDREAMSETTINGS_H

#include <QDialog>
class CMap;

namespace Ui {
class CDreamSettings;
}

class QCheckBox;
class QLineEdit;

class CDreamSettings : public QDialog {
	Q_OBJECT
	
public:
	explicit CDreamSettings(CMap *map, QWidget *parent = 0);
	~CDreamSettings();

	bool anythingChanged() const { return m_anythingChanged; }
	bool patchSettingsChanged() const { return m_patchSettingsChanged; }

public slots:
	void saveToMap();

private slots:
	void choosePatchDir();

private:
	CMap *m_map;
	Ui::CDreamSettings *ui;
	bool m_anythingChanged;
	bool m_patchSettingsChanged;

	void saveProperty(QCheckBox *field, bool &target);
	void saveProperty(QLineEdit *field, QString &target, bool isPatch = false);
};

#endif // CDREAMSETTINGS_H
