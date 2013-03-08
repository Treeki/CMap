#include "ceditorwindow.h"

#include "cedittoolbox.h"
#include "cmapwidget.h"
#include "cdreamsettings.h"
#include <QUndoView>
#include <QDockWidget>
#include <QScrollArea>
#include <QAction>
#include <QIcon>
#include <QMenuBar>
#include <QToolBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QKeySequence>
#include <QSettings>

CEditorWindow::CEditorWindow(QWidget *parent) :
	QMainWindow(parent),
	m_mapIsSaved(false), m_map(0), m_patchContext(0) {

	m_mapScrollArea = new QScrollArea(this);
	m_mapWidget = new CMapWidget(this);

	m_mapWidget->setScrollArea(m_mapScrollArea);

	m_mapScrollArea->setWidget(m_mapWidget);
	setCentralWidget(m_mapScrollArea);


	// Create main toolbar
	m_toolbar = addToolBar("Editor");
	m_toolbar->setObjectName("EditorToolbar");
	m_toolbar->setIconSize(QSize(16, 16));

	// Create toolbox
	m_toolbox = new CEditToolbox(this);

	m_toolboxDock = new QDockWidget("Tools", this);
	m_toolboxDock->setObjectName("ToolboxDock");
	m_toolboxDock->setWidget(m_toolbox);
	addDockWidget(Qt::LeftDockWidgetArea, m_toolboxDock);

	// Create History panel
	m_undoView = new QUndoView(&m_undo, this);
	m_undoView->setCleanIcon(QIcon(":/icons/save-small.png"));

	m_undoDock = new QDockWidget("History", this);
	m_undoDock->setObjectName("UndoDock");
	m_undoDock->setWidget(m_undoView);
	addDockWidget(Qt::LeftDockWidgetArea, m_undoDock);


	setupActions();
	setupMenubar();
	setupToolbar();

	updateMRU();

	setupZoom();

	// Restore our state if we've got one
	QSettings settings;
	if (settings.contains("editorWindowGeometry")) {
		restoreGeometry(settings.value("editorWindowGeometry").toByteArray());
		restoreState(settings.value("editorWindowState").toByteArray());
	}

	// And there we go!
	loadMap();
	//loadMap("/home/me/Furcadia/Dreams/CH_G3/G3.map");
	//loadMap("/home/me/WINE/Program Files/Furcadia/maps/therio.map");
	//loadMap("/home/me/Furcadia/Dreams/cmtest.map");
	//loadMap("/home/me/pillar/pillar.map");
}

void CEditorWindow::setupActions() {
	// Basic Document Management
	m_newAction = new QAction(QIcon(":/icons/new.png"), "&New Map", this);
	m_openAction = new QAction(QIcon(":/icons/open.png"), "&Open...", this);
	m_saveAction = new QAction(QIcon(":/icons/save.png"), "&Save", this);
	m_saveAsAction = new QAction("Save &As...", this);

	connect(m_newAction, SIGNAL(triggered()), SLOT(newMap()));
	connect(m_openAction, SIGNAL(triggered()), SLOT(open()));
	connect(m_saveAction, SIGNAL(triggered()), SLOT(save()));
	connect(m_saveAsAction, SIGNAL(triggered()), SLOT(saveAs()));

	// MRU
	m_nullMruAction = new QAction("(None)", this);
	m_nullMruAction->setEnabled(false);
	for (int i = 0; i < MruCount; i++) {
		m_mruActions[i] = new QAction(this);
		m_mruActions[i]->setVisible(false);
		connect(m_mruActions[i], SIGNAL(triggered()), SLOT(openMRUFile()));
	}

	// Undo/redo
	m_undoAction = m_undo.createUndoAction(this, "Undo");
	m_undoAction->setIcon(QIcon(":/icons/undo.png"));
	m_redoAction = m_undo.createRedoAction(this, "Redo");
	m_redoAction->setIcon(QIcon(":/icons/redo.png"));

	// Zooming
	m_zoomInAction = new QAction(QIcon(":/icons/zoom-in.png"), "Zoom In", this);
	m_zoomActualAction = new QAction(QIcon(":/icons/zoom-actual.png"), "Zoom to Actual Size", this);
	m_zoomOutAction = new QAction(QIcon(":/icons/zoom-out.png"), "Zoom Out", this);
	connect(m_zoomInAction, SIGNAL(triggered()), SLOT(zoomIn()));
	connect(m_zoomActualAction, SIGNAL(triggered()), SLOT(zoomActual()));
	connect(m_zoomOutAction, SIGNAL(triggered()), SLOT(zoomOut()));

	// Dream stuff
	m_dreamSettingsAction = new QAction("Dream Settings", this);
	connect(m_dreamSettingsAction, SIGNAL(triggered()), SLOT(showDreamSettings()));
}

void CEditorWindow::setupMenubar() {
	QMenu *m;

	m = menuBar()->addMenu("&File");
	m->addAction(m_newAction);
	m->addAction(m_openAction);

	QMenu *mruMenu = m->addMenu("Open &Recent");
	mruMenu->addAction(m_nullMruAction);
	for (int i = 0; i < MruCount; i++)
		mruMenu->addAction(m_mruActions[i]);

	m->addAction(m_saveAction);
	m->addAction(m_saveAsAction);
	m->addSeparator();
	m->addAction("E&xit", this, SLOT(close()));

	m = menuBar()->addMenu("&Edit");
	m->addAction(m_undoAction);
	m->addAction(m_redoAction);
	m->addSeparator();
	m->addAction(m_dreamSettingsAction);

	m = menuBar()->addMenu("&View");
	m->addAction(m_zoomInAction);
	m->addAction(m_zoomOutAction);
	m->addAction(m_zoomActualAction);
	m->addSeparator();

	auto tbMenu = m->addMenu("&Toolbars");
	tbMenu->addAction(m_toolbar->toggleViewAction());
	m->addAction(m_toolboxDock->toggleViewAction());
	m->addAction(m_undoDock->toggleViewAction());

	m = menuBar()->addMenu("&Help");
	m->addAction("&About CMap", this, SLOT(showAboutBox()));
}

void CEditorWindow::setupToolbar() {
	m_toolbar->addAction(m_newAction);
	m_toolbar->addAction(m_openAction);
	m_toolbar->addAction(m_saveAction);
	m_toolbar->addSeparator();

	m_toolbar->addAction(m_undoAction);
	m_toolbar->addAction(m_redoAction);
	m_toolbar->addSeparator();

	m_toolbar->addAction(m_zoomInAction);
	m_toolbar->addAction(m_zoomActualAction);
	// possible: combobox for zooming with QWidgetAction?
	m_toolbar->addAction(m_zoomOutAction);
}


void CEditorWindow::updateMRU() {
	QSettings settings;
	QStringList mru = settings.value("recentFiles").toStringList();

	for (int i = 0; i < MruCount; i++) {
		if (i < mru.count()) {
			// TODO: trim down displayed names...?
			m_mruActions[i]->setText(mru.at(i));
			m_mruActions[i]->setData(mru.at(i));
			m_mruActions[i]->setVisible(true);
		} else
			m_mruActions[i]->setVisible(false);
	}

	m_nullMruAction->setVisible(mru.isEmpty());
}


void CEditorWindow::addToMRU(QString path) {
	QSettings settings;
	QStringList mru = settings.value("recentFiles").toStringList();

	int whereNow = mru.indexOf(path);
	if (whereNow == 0) {
		// already got this one, do nothing
		return;
	} else if (whereNow > 0) {
		// this one is already in the list, so we're moving it to the top
		mru.removeAt(whereNow);
	}

	mru.insert(0, path);
	while (mru.count() > MruCount)
		mru.removeLast();

	settings.setValue("recentFiles", mru);
	updateMRU();
}


void CEditorWindow::openMRUFile() {
	auto action = qobject_cast<QAction *>(sender());
	if (action)
		loadMap(action->data().toString());
}


void CEditorWindow::setMapPath(QString mapPath) {
	m_mapIsSaved = true;

	QFileInfo mapFI(mapPath);
	m_mapDir = mapFI.canonicalPath();
	m_mapFilename = mapFI.fileName();
	m_mapPath = mapPath;

	addToMRU(mapPath);
}


void CEditorWindow::loadMap(QString mapPath) {
	auto newMap = new CEditableMap(this);

	if (mapPath.isNull()) {
		m_mapIsSaved = false;
		m_mapDir = QString::null;
		m_mapFilename = QString::null;
		m_mapPath = QString::null;
	} else {
		setMapPath(mapPath);

		if (!newMap->load(mapPath)) {
			QMessageBox::warning(this, "Sorry",
								 "Something went wrong while loading this map :(");
			delete newMap;
			return;
		}
	}

	if (m_map)
		delete m_map;
	m_map = newMap;

	// The undo system is an absolute mess.
	// Don't use the UndoGroup's cleanChanged signal
	// or isClean method, okay?
	m_undo.removeStack(m_undo.activeStack());
	m_undo.addStack(m_map->undo.baseStack());
	m_map->undo.baseStack()->setActive();

	connect(&m_map->undo, SIGNAL(cleanChanged(bool)), SLOT(handleCleanChanged(bool)));

	m_mapWidget->setMap(m_map);
	loadPatches(); // sets up the edit toolbox too

	handleCleanChanged(true); // updates title bar too
}


void CEditorWindow::loadPatches() {
	if (m_patchContext)
		m_patchContext->deleteLater();

	m_patchContext = new CPatchContext(this);

	m_map->loadPatchContext(m_patchContext, CPatchFile::getDefaultPatchPath(), m_mapDir);

	m_mapWidget->setPatches(m_patchContext);
	// TODO: make this not destroy everything in the toolbox when
	// just reloading the patch
	m_toolbox->setup(m_mapWidget);
}


bool CEditorWindow::verifyMapSaved() {
	if (m_map->undo.isClean())
		return false;

	// must save!
	QMessageBox ask;
	ask.setText("This map has been modified.");
	ask.setInformativeText("Do you want to save your changes?");
	ask.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	ask.setDefaultButton(QMessageBox::Cancel);

	int value = ask.exec();

	if (value == QMessageBox::Cancel) {
		return true;
	} else if (value == QMessageBox::Save) {
		// try and save it
		save();

		// note: if the map was previously unsaved, and the user cancels the
		// "choose a name" dialog, it won't be saved, and we must cancel
		// whatever is calling verifyMapSaved()!
		if (!m_mapIsSaved)
			return true;
	}
	// don't handle Discard since it just returns false

	return false;
}


void CEditorWindow::closeEvent(QCloseEvent *event) {
	if (verifyMapSaved()) {
		// do not close!
		event->ignore();
	} else {
		// OK
		// Save our stuff first, though
		QSettings settings;
		settings.setValue("editorWindowGeometry", saveGeometry());
		settings.setValue("editorWindowState", saveState());

		event->accept();
	}
}


void CEditorWindow::handleCleanChanged(bool clean) {
	updateTitleBar();

	m_saveAction->setEnabled((!clean) || (!m_mapIsSaved));
}

void CEditorWindow::updateTitleBar() {
	QString title("CMap - ");
	if (!m_map->undo.isClean())
		title.append('*');

	if (m_mapIsSaved) {
		title.append(m_mapFilename);
		title.append(" [");
		title.append(m_mapDir);
		title.append(']');
	} else {
		title.append("(unsaved map)");
	}

	setWindowTitle(title);
}


void CEditorWindow::newMap() {
	if (verifyMapSaved())
		return;

	loadMap();
}

void CEditorWindow::open() {
	if (verifyMapSaved())
		return;

	QString filename = QFileDialog::getOpenFileName(
				this, "Open Map", QString(),
				"Furcadia dreams (*.map);;All files (*.*)");

	if (filename.isEmpty())
		return;

	addToMRU(filename);
	loadMap(filename);
}

void CEditorWindow::save(bool forceNewFilename) {
	if (forceNewFilename || !m_mapIsSaved) {
		// try this
		QString filename = QFileDialog::getSaveFileName(
					this, "Save Map", QString(),
					"Furcadia dreams (*.map);;All files (*.*)");

		if (filename.isEmpty())
			return;

		setMapPath(filename);
	}

	m_map->revision++;
	m_map->save(m_mapPath);
	m_map->undo.setClean();
}

void CEditorWindow::saveAs() {
	save(true);
}


static const float ZoomLevels[] = {
	0.05f, 0.1f, 0.25f, 0.375f, 0.5f, 0.75f, 1.0f, 1.5f, 2.0f
};
static const int ZoomLevelDefaultIndex = 6;

void CEditorWindow::zoomIn() {
	m_zoomLevelIndex++;
	m_mapWidget->setZoomLevel(ZoomLevels[m_zoomLevelIndex]);
	updateZoomActions();
}

void CEditorWindow::zoomOut() {
	m_zoomLevelIndex--;
	m_mapWidget->setZoomLevel(ZoomLevels[m_zoomLevelIndex]);
	updateZoomActions();
}

void CEditorWindow::zoomActual() {
	m_zoomLevelIndex = ZoomLevelDefaultIndex;
	m_mapWidget->setZoomLevel(1.0f);
	updateZoomActions();
}

void CEditorWindow::setupZoom() {
	m_zoomLevelIndex = ZoomLevelDefaultIndex;
	updateZoomActions();
}

void CEditorWindow::updateZoomActions() {
	m_zoomInAction->setEnabled(m_zoomLevelIndex < ((sizeof(ZoomLevels) / sizeof(ZoomLevels[0])) - 1));
	m_zoomOutAction->setEnabled(m_zoomLevelIndex > 0);
	m_zoomActualAction->setEnabled(m_zoomLevelIndex != ZoomLevelDefaultIndex);
}


void CEditorWindow::showAboutBox() {
	QMessageBox::about(this,
					   "About CMap",

					   "An alternative Furcadia map editor developed by Treeki.\n"
					   "\n"
					   "Source code and more information available here:\n"
					   "http://github.com/Treeki/CMap");
}


void CEditorWindow::showDreamSettings() {
	CDreamSettings *ds = new CDreamSettings(m_map, this);
	if (ds->exec() == QDialog::Accepted) {
		ds->saveToMap();
		if (ds->anythingChanged())
			m_map->undo.makeUnfixablyDirty();
	}
	ds->deleteLater();
}


CEditorWindow::~CEditorWindow() {
}
