CMap (Working Title)
====================

CMap is an alternative map editor written by Treeki for the multiplayer online
game [Furcadia][furc].

[furc]: http://www.furcadia.com

It's written in C++ using the Qt framework, which means that it's fully
compatible with Windows, Linux and Mac OS, as well as anything else supported
by Qt. :p

I develop and test with Qt 5. It *may* compile with Qt 4, but I can't
guarantee that. Will try to make that happen for an eventual official alpha
release though!

# Design Ideas/Philosophy

I've been dreamweaving for 8 years and used pretty much every dream editor
that's been released. Without going into too much history, I used to be a huge
fan of Reunion's Dreamer editor, but it hasn't been updated in years and is
missing support for many things, and the recent editions are rather slow and
buggy.

I wrote my own editor back in 2008, but it didn't really offer much over stock
DreamEd and I eventually got bored of working on it. I also didn't really have
the skills to implement the features I wanted... and the code was extremely
messy and Windows-only.

Fast forward to 2012: I've become a much better programmer, learned how to use
Qt, and I've decided to tackle this problem again. This is my attempt.

My goal with CMap is to bring some of the features that are common in raster
graphics editors over to a dream editor. Regular images are a grid of pixels;
Furcadia maps are a grid of tiles: why not treat them similarly?

Instead of basic painting like we have now in pretty much every dream editor
ever written, I want to add more advanced tools like lines and rectangles that
will save us time. Selections don't have to be just for copying and pasting -
they could serve as "only draw within this area", like in some image editors.

I know I've wasted quite a bit of time drawing lines and removing stray walls
that have got out of the line.

And it'll be open source so if/when I get bored of it, someone else can pick
it up and make it work with whatever gets added to Furcadia later on.


# Implemented so far:

- Loading/saving of maps; patch loading
- Rudimentary object picker
- Basic brush tool
- Basic line tool
- Map widget that displays and edits a map, with scrolling and zooming in/out
- Undo/Redo with multiple levels
- Loading of FOX and FSH/FBJ files
- Dream Settings dialog
- Resize dialog


# Planned/To do:

## Object Picker:

- nicer look
- saving of scroll position when resizing
- FBJ flag display
- a way to open up a temporary larger picker, similar to DreamEd's
- a shortcut for selecting "no object" (or wall, region, etc) without
  scrolling to the top of the list
- perhaps filtering? not sure how this'd work yet

## Patch Support:

- FS2 support
- double-checking of patch boundaries etc
- KitterSpeak support

## Map Widget:

- Display of Furcadia field-of-view
- Configurable highlight
- Configurable views (show/hide various layers, set translucency (maybe), etc)

## Brush Tool:

- Larger brushes; checkerboard brush with two parity options

## Other Editing Features:

- Tools: Selection, rectangle/diamond, ...
- Copy/paste
- "Clips" feature, similar to Dreamer Reloaded

## Minor (Short term) Things:

- Refactor the tool classes so that the logic for a tool that holds a specific
  object and can place/pick it is all in one place, and not duplicated
  between Brush, Line and Fill
- Unobtrusive way to show errors: failed patch loads (non-existent dir etc),
  invalid object IDs that could cause issues when the dream is uploaded, ...

Of course, I'm sure there's lots of stuff I've missed. I'd like to release an
initial Alpha pretty soon - perhaps once I get it to feature parity with the
standard Dream Editor. I think implementing the stuff in that last section
plus flood fill should get me to that level, so we'll see! Really looking
forward to that.

I'd love to eventually expand into other tools - a DS editor, patch editor,
and even a walkthrough mode - but these are lofty goals and I'm going to work
on making a great dream editor first.


# Contact Me

Send me a message on GitHub or on the Furcadia forums, or just whisper me
in-game. I use the username Treeki on all of those.

# Acknowledgements

This app uses the "Fugue" icon set by [Yusuke Kamiyamane][yk], licensed under
the Creative Commons Attribution 3.0 License.

[yk]: http://p.yusukekamiyamane.com

