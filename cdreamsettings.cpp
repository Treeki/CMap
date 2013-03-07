#include "cdreamsettings.h"
#include "ui_cdreamsettings.h"
#include "cmap.h"
#include <QPushButton>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>
#include <QButtonGroup>
#include <QFileDialog>

CDreamSettings::CDreamSettings(CMap *map, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CDreamSettings),
	m_anythingChanged(false)
{
	m_map = map;
	ui->setupUi(this);
	connect(ui->patchDirBrowse, SIGNAL(clicked()), SLOT(choosePatchDir()));

	// Set up the Standards list
	for (int i = 0; i < CMap::StandardCount; i++)
		ui->dreamStandard->addItem(CMap::StandardNames[i]);

	// Load everything... everything
	ui->dreamName->setText(map->dreamName);
	ui->encryptDream->setChecked(map->isEncrypted);

	ui->dreamStandard->setCurrentIndex((int)map->standard);

	switch (map->patchType) {
	case CMap::NoPatch:
		ui->noPatches->setChecked(true);
		break;
	case CMap::CustomPatch:
		ui->customPatches->setChecked(true);
		ui->patchDirEntry->setText(map->patchName);
		break;
	case CMap::ServerPatch:
		ui->publicPatch->setChecked(true);
		ui->publicPatchEntry->setText(map->patchName);
		break;
	}

	ui->allowJoinAndSummon->setChecked(map->allowSummon);
	ui->allowLargeDreams->setChecked(map->hasGroupPackage);
	ui->allowLeadAndFollow->setChecked(map->allowFollow);
	ui->allowSeasonals->setChecked(map->allowSeasonals);
	ui->allowShouting->setChecked(map->allowShouting);
	ui->allowURLEntry->setChecked(map->allowURLs);
	ui->forbidSittingOnTheFloor->setChecked(map->restrictSitting);
	ui->showNamesOnTab->setChecked(map->allowTab);
	ui->showPlayerList->setChecked(map->allowWho);
	ui->useParentalControls->setChecked(map->parentalWarning);
	ui->useSwearFilter->setChecked(map->enableFilter);
}

CDreamSettings::~CDreamSettings() {
	delete ui;
}


void CDreamSettings::saveToMap() {
	m_anythingChanged = false;

	saveProperty(ui->dreamName, m_map->dreamName);
	saveProperty(ui->encryptDream, m_map->isEncrypted);

	auto standard = (CMap::StandardType)ui->dreamStandard->currentIndex();
	if (m_map->standard != standard) {
		m_map->standard = standard;
		m_anythingChanged = true;
	}

	auto ptRadio = ui->patchTypeButtonGroup->checkedButton();
	CMap::PatchType ptype;
	if (ptRadio == ui->customPatches) {
		ptype = CMap::CustomPatch;
		saveProperty(ui->patchDirEntry, m_map->patchName, true);
	} else if (ptRadio == ui->publicPatch) {
		ptype = CMap::ServerPatch;
		saveProperty(ui->publicPatchEntry, m_map->patchName, true);
	} else {
		ptype = CMap::NoPatch;
		m_map->patchName.clear();
	}

	if (m_map->patchType != ptype) {
		m_map->patchType = ptype;
		m_anythingChanged = true;
		m_patchSettingsChanged = true;
	}

	saveProperty(ui->allowJoinAndSummon, m_map->allowSummon);
	saveProperty(ui->allowLargeDreams, m_map->hasGroupPackage);
	saveProperty(ui->allowLeadAndFollow, m_map->allowFollow);
	saveProperty(ui->allowSeasonals, m_map->allowSeasonals);
	saveProperty(ui->allowShouting, m_map->allowShouting);
	saveProperty(ui->allowURLEntry, m_map->allowURLs);
	saveProperty(ui->forbidSittingOnTheFloor, m_map->restrictSitting);
	saveProperty(ui->showNamesOnTab, m_map->allowTab);
	saveProperty(ui->showPlayerList, m_map->allowWho);
	saveProperty(ui->useParentalControls, m_map->parentalWarning);
	saveProperty(ui->useSwearFilter, m_map->enableFilter);
}

void CDreamSettings::saveProperty(QCheckBox *field, bool &target) {
	if (field->isChecked() != target) {
		m_anythingChanged = true;
		target = field->isChecked();
	}
}

void CDreamSettings::saveProperty(QLineEdit *field, QString &target, bool isPatch) {
	if (field->text() != target) {
		m_anythingChanged = true;
		if (isPatch)
			m_patchSettingsChanged = true;
		target = field->text();
	}
}


void CDreamSettings::choosePatchDir() {
	QString dir = QFileDialog::getExistingDirectory(this,
													"Choose a patch folder",
													ui->patchDirEntry->text());

	if (!dir.isEmpty())
		ui->patchDirEntry->setText(dir);
}
