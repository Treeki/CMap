CMap Architecture
=================

Since this is an open source program, I suppose I should document how my stuff
works, right? :p Here's a basic description of how everything works...

*- Treeki, 11th June 2012*


Random Thoughts
---------------

I expected to be 


Interface
---------

`CEditorWindow` handles the main editor window. I still need to flesh this out
significantly.
I'll add some documentation here about document management, etc when I do it.

`CEditToolbox` contains most of the UI for editing, and also holds instances
of each tool.

`CObjectPicker` is contained within `CEditToolbox`, and is simply a picker with
tabs for each type of shape. `CShapePicker` is used in each tab of the object
picker, and contains a list (and in the future will contain other widgets
related to choosing shapes).

`CMapWidget` displays and edits maps. Rocket science!


Patches
-------

Each patch file is a `CPatchFile`, which contains a list of `CShape` (which in
turn contains `CFrame` and `CStep`). All of these are declared in
`cpatchfile.h`. The same class is used for FOX and FSH files.

Once a patch file is loaded, it's not used directly by the editor; the shapes
from every patch file used in a specific space (eg. all items or all floors)
are loaded into `CShapePool`.

`CShapeIndexer` deals with the inconsistent mapping between the shape numbers
you can see in maps/DS/etc and the actual ShapePool shape that's pulled up.
It also holds a list of choosable shapes, which is used by `CShapeModel`. 

The base indexer class is used by items, floors and effects (but only by the
shape pickers). The subclasses `CWallShapeIndexer` and `CRegionShapeIndexer`
are used by walls and regions respectively and also handle the image number
mappings for CMapWidget, in addition to the shape picker stuff.

`CShapeModel` is a Qt model used by `CShapePicker`, which maps to one
`CShapeIndexer` and `CShapePool`. It just takes the list of pickable shapes
provided by `CShapeIndexer` and makes it usable in Qt's model/view stuff.

`CPatchContext` stores a palette, shape pools and indexers for the five shape
types used by maps.


Map
---

A position on the map is represented by `CMapPoint` (a POD struct). It stores
an X and Y position (the X is not halved, so it can refer to walls) and
provides convenience methods for moving in the basic Furcadia directions.

`CMapVector` is a template class that provides a resizable two-dimensional
vector/array, and can also be accessed using `CMapPoint` as an index.

`CMap` is the main class that stores a map and its data (both the header and
physical objects/floors/etc). It also has some utility methods (static where
possible) for dealing with randomised items, rendered wall shapes, etc.

`CEditableMap` is a subclass that adds some useful stuff for the editable map:
a signal and enum for updating a tile on-screen, an undo stack and the current
selection.


Map Editing
-----------

I already mentioned `CEditToolbox` earlier, but it's a class that holds an
instance of every tool, along with the UI for choosing tools and settings for
them.

`CEditCommand` is a subclass of `QUndoCommand` that stores a list of map
changes and will automatically apply them or roll them back. For convenience,
pretty much. Maybe I should rename this to `CMapEditCommand`?

`CUndoStack` is, well.. something I'm really unhappy with. I'll probably end
up removing it at some point. It's a subclass of CUndoStack that allows the
"clean" state to be disabled when a change has occurred that can't be undoed,
but won't affect the other changes (for example, the dream settings - if these
are modified it won't make a difference to the map edits and their
undoability).

`CToolBase` is the base class for every tool. When a tool is activated, the
`deactivate()` method is called on the previous tool (if there was one) and
`activate()` is called on the new tool. An active tool will receive various
mouse events on the map (sent through virtual functions) and will also have
the ability to preview in-progress changes, by overriding `typesToPreview()`
and returning what change types are passed through to it (this is just an
optimisation) and then overriding `whatFloorFor` and related methods to return
what each tile should have.

`CCommandToolBase` is a convenience subclass for tools that create a
`CEditCommand` while drawing. It creates one when the left mouse button is
held down and finalises it when the button is released or the tool is
deactivated.

`CBrushTool` is.. well, the brush tool. That's all I can say. When I get
around to adding more tools, I may refactor this a bit to remove duplicated
code. Wasn't quite sure what I'd need to reuse, so...

