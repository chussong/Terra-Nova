# Terra Nova

Terra Nova is (or eventually will be) a strategy game in a custom engine built
using only SDL (for multiplatform window display), Boost, and standard C++14.
All of the art assets are my own work in GIMP, except for the portraits of the
characters which are taken from Advance Wars by Intelligent Systems (they will 
of course be replaced later). Of the three music tracks which are currently in 
the game, one is a midi recreation of the Morrowind theme, one is my own
composition, and one is an actual recording of La Marseillaise. The copyrighted
music should hopefully be replaced soon.  

Much of the engine is in place, but none of the actual levels are done yet. The
intro scene is very much a work in progress, so it spits out a bunch of errors;
if you skip the intro, though, the resulting game interface should work fine. 
The level I'm using for testing has a victory trigger which fires when you build
and power a Revival Chamber. Hopefully this should work with no errors.  

The following is an overview of the features implemented so far:  

* Flexible, modular systems for sprites, windows, UI, and audio.  

* Exposed definition and media files allowing easy modification, both for
development and for users who would like to change things.  

* Unit and map data defined in text files using a custom scripting language, 
which the game reads in and turns into levels.  

* Dialogue and event systems also defined in text files using a custom scripting 
language.  

* Unit movement and orders, including pathfinding, collision resolution, and
combat.  

* Resource collection and building construction mechanics, including a victory
condition based on constructing a particular building.  

* AI which can identify enemies who get close, pathfind to attack them, and
prioritize weaker targets.  

The current short-term goal is to finish the scripting support for the intro
cutscene and add some more guidance and challenge to the first mission. The
medium-term goal is to create a 10-mission single player campaign complete with
varied mission types and real, non-placeholder art and music assets. The 
long-term goal will then be to find a way to share it publicly, either as an
open-source game or commercially, and then expand to new features like
multiplayer, procedural level generation, larger campaigns, and so on.  
