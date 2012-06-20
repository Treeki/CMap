#include "cpatchcontext.h"

CPatchContext::CPatchContext(QObject *parent) :
	QObject(parent),
	itemIndexer(CMap::Item, &items),
	floorIndexer(CMap::Floor, &floors),
	effectIndexer(CMap::Effect, &effects),
	wallIndexer(&walls),
	regionIndexer(&regions) {
}


void CPatchContext::loadDefaultPalette() {
	palette.resize(256);

	QFile file(":/graphics/default.pal");
	file.open(QFile::ReadOnly);

	quint8 data[768];
	file.read((char*)data, 768);

	palette[0] = qRgba(0, 0, 0, 0);
	for (int i = 1; i < 256; i++) {
		palette[i] = qRgb(data[i*3], data[i*3+1], data[i*3+2]);
	}
}


void CPatchContext::loadPatches(QString defaultPath, QString customPath) {
	loadDefaultPalette();

	items.setup(3400);
	floors.setup(1000);
	walls.setup(20*18);
	regions.setup(200);
	effects.setup(1000);

	QDir defaultDir(defaultPath);
	loadPatchDir(defaultDir, false);

	if (!customPath.isNull()) {
		QDir customDir(customPath);
		loadPatchDir(customDir, true);
	}

	itemIndexer.setup();
	floorIndexer.setup();
	wallIndexer.setup();
	//regionIndexer.setup();
	effectIndexer.setup();
}


void CPatchContext::loadPatchDir(QDir dir, bool isCustom) {
	// ITEMS
	CPatchFile pItem, pItem2, pItem3, pItem4, pIteme;

	if (pItem.loadFromDir(dir, "item", palette))
		items.applyPatch(pItem, 0, 0, 305);
	if (pItem2.loadFromDir(dir, "item2", palette))
		items.applyPatch(pItem2, 0, 305, 695);
	if (pItem3.loadFromDir(dir, "item3", palette))
		items.applyPatch(pItem3, 0, 1000, 349); // Todo: Check this
	if (pItem4.loadFromDir(dir, "item4", palette))
		items.applyPatch(pItem4, 0, 1349, 449); // Todo: Check this
	if (pIteme.loadFromDir(dir, "iteme", palette))
		items.applyPatch(pIteme, 0, 2400, 1000);

	// FLOORS
	CPatchFile pFloors, pFloors2, pFloore;

	if (pFloors.loadFromDir(dir, "floors", palette))
		floors.applyPatch(pFloors, 0, 0, 208);
	if (pFloors2.loadFromDir(dir, "floors2", palette))
		floors.applyPatch(pFloors2, 0, 209, 26);
	if (pFloore.loadFromDir(dir, "floore", palette))
		floors.applyPatch(pFloore, 0, 500, 500);

	// WALLS
	// Todo: proper junction support. Some time?
	CPatchFile pJunction, pWall;

	if (pJunction.loadFromDir(dir, "junction", palette)) {
		walls.applyPatch(pJunction, 6, 1, 1);
		walls.applyPatch(pJunction, 9, 0, 1);

		walls.applyPatch(pJunction, 21, 19, 1);
		walls.applyPatch(pJunction, 24, 18, 1);
	}

	if (pWall.loadFromDir(dir, "wall", palette)) {
		walls.applyPatch(pWall, 0, 2, 16);
		walls.applyPatch(pWall, 16, 20, 16);
	}

	for (int i = 0; i < 20; i++) {
		CPatchFile pOneWall;
		if (pOneWall.loadFromDir(dir, QString("wall_%1").arg(i + 1), palette)) {
			walls.applyPatch(pOneWall, 0, (i*18)+2, 16);
			walls.applyPatch(pOneWall, 16, i*18, 2);
			// TODO: simulate the behaviour where textures 18, 19 and 20 do not
			// have the correct walkability properties
		}
	}

	// REGIONS
	CPatchFile pRegions;

	if (pRegions.loadFromDir(dir, "regions", palette))
		regions.applyPatch(pRegions, 0, 0, 200);

	// EFFECTS
	CPatchFile pEffects, pEffectse;

	if (pEffects.loadFromDir(dir, "effects", palette))
		effects.applyPatch(pEffects, 0, 0, 200);
	if (pEffectse.loadFromDir(dir, "effectse", palette))
		effects.applyPatch(pEffectse, 0, 200, 800);
}
