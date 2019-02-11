# rl
a small roguelike i hack on when i'm drunk.

the only really interesting part of the code in my opinion 
is `load_level()` in `src/worldmap/load_level.c`.

### maps
Maps are written in json. Here's an example of a small map:
```json
{
	"rows":25, "cols":25, "tile":"floor",
	"structures":[
		{
			"rows":25, "cols":25, "tile":"wall", "hollow": true,
			"structures":[
				{ "xpos":8, "ypos":12, "rows":5, "cols":5, "tile":"water" }
			]
		}
	]
}
```
At the moment, the map system only communicates the map tiles. 
It can't communicate the existance of entities or items 
or their positions, though this is planned. 

The way the map system works, is essentially you describe a 
heirarchy of rectangles in json. Each rectangle specifies 
what tile is to be used to draw it, 
and is positioned relative to it's parent rectangle.

This enables me to represent very large but simple 
maps in an incredibly succinct and efficient way, to the 
point where they can be handwritten. This was in fact the original
intent behind loading the maps from json files; a small
increase in complexity or detail though, and the complexity 
becomes too much to want to manage by hand.


There are two measures I have planned to mitigate this.

* "ergonomic" shortcut attributes
  * basically, i write some complex shapes in json, and bake them
into the executable, then objects in map files can specify 
"shape", and "orientation". This would allow me to 
represent a complex structure made of dozens of rectangles with only
one in the actual file I have to write.
* map editor / compiler
  * a sister application that allows a user to manipulate a 2d
buffer and draw the map. the application would upon save / exporting
compile the runtime representation of the map that the user is editing
into json that is then loaded by rl. 

After implementing both of these, it's my vision that the sister editor 
app is the end all be all of map creation / editing, but it'd 
still be possible to tweak maps by hand.
