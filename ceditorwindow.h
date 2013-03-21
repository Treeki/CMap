#ifndef CEDITORWINDOW_H
#define CEDITORWINDOW_H

#include <QMainWindow>
#include <QUndoGroup>

class CEditToolbox;
class CMapWidget;
class QUndoView;
class QDockWidget;
class QScrollArea;
class QAction;
class CEditableMap;
class CPatchContext;
class CDreamSettings;
struct CMapPoint;

class CEditorWindow : public QMainWindow {
	Q_OBJECT
	
public:
	explicit CEditorWindow(QWidget *parent = 0);
	~CEditorWindow();

protected:
	int m_zoomLevelIndex;
	void setupZoom();
	void updateZoomActions();

public slots:
	void newMap();
	void open();
	void save(bool forceNewFilename = false);
	void saveAs();

	void zoomIn();
	void zoomOut();
	void zoomActual();

	void showAboutBox();
	void showDreamSettings();
	void showResizeDream();

	void reloadPatches();

private:
	void setupActions();
	void setupMenubar();
	void setupToolbar();

	void updateMRU();
	void addToMRU(QString path);

	void loadPatches(bool forNewMap = true);

	void setMapPath(QString mapPath);

private slots:
	void openMRUFile();
	void handleCleanChanged(bool clean);

	void handleWalkingBordersChanged(bool value);

	void handleCopyPositionToClipboard(const CMapPoint &pos);

protected slots:
	void updateTitleBar();

protected:
	bool verifyMapSaved();

	void loadMap(QString mapPath=QString());
	bool m_mapIsSaved;
	QString m_mapPath;
	QString m_mapDir, m_mapFilename;
	CEditableMap *m_map;
	CPatchContext *m_patchContext;

	QAction *m_newAction;
	QAction *m_openAction;
	QAction *m_saveAction;
	QAction *m_saveAsAction;

	QAction *m_undoAction;
	QAction *m_redoAction;

	QAction *m_dreamSettingsAction;
	QAction *m_reloadPatchAction;
	QAction *m_resizeDreamAction;

	QAction *m_zoomInAction;
	QAction *m_zoomOutAction;
	QAction *m_zoomActualAction;

	QAction *m_walkingBorderAction;

	QAction *m_showItemsAction, *m_showFloorsAction, *m_showWallsAction;
	QAction *m_showRegionsAction, *m_showEffectsAction;

	static const int MruCount = 8;
	QAction *m_nullMruAction;
	QAction *m_mruActions[MruCount];

	QUndoGroup m_undo;

	QUndoView *m_undoView;
	QDockWidget *m_undoDock;

	CEditToolbox *m_toolbox;
	QDockWidget *m_toolboxDock;

	QToolBar *m_toolbar;

	QScrollArea *m_mapScrollArea;
	CMapWidget *m_mapWidget;

	void closeEvent(QCloseEvent *);
};

#endif // CEDITORWINDOW_H
