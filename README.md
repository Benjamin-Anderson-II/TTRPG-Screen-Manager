# TTRPG Table-Screen Manager (working title)
## What I Want
Simply put, I want a touch-screen display of modular size that I can overlay on top of any table.
The touchscreen should have sections at each players' seats where they can choose their mini, and select rulers to use during combat for spells, abilities, and the like.
Initially I want the touch-screen feature to be somewhat limited, implementing new features into the system as they come up at the table during play.

## Implementation
### Software
I'll be using C++ with OpenGL, GLFW, and GLAD for this project, mostly because these are the tools I was taught and so I have comfort using them.
I'm using CMake for my build tool, and I will not be using any GenAI during the development or deployment of this project.
Since I'm using CMake I'll also be using CTest for the testing suite.
I toyed around with using Docker for containerization, but ultimately couldn't see much of a reason to containerize so I opted out of it.
**I'm developing this in Debian(Trixie), and will be developing it such that it runs on my machine with my specs first and foremost.**
If, in the future I decide to build this for other operating systems, then I'll use Docker for testing those systems.
### Hardware
Currently, my thoughts for the how to make the touchscreen modular are to buy and connect a bunch of small roughly 6x6 inch screens together that I can store in a box or something for easy transportation.
In order to have the system recognize the miniatures I have considered a few options.
The more concrete idea is to have detachable bases for the minis.
The first method I thought of was to have a select number of pads on the bottom of the bases that register as fingers, where I then check the number of pads and in what order they're in.
The limitation here is primarily space.
Since I don't know what screens I'm going to be using I don't know how dense I can make the touch-pads, but my assumption is likely no more than 7, which makes for limited design space when designing encounters.
The other idea I had is to attach little rfid tags to each of the minis and create a subsurface layer below the screens that will detect where the tags are and display them to the screen accordingly.
I believe this to be more robust, but also more expensive and difficult.

## Why Do All This?
As far as I know, right now all of the TV-tables for ttrpgs are one big flat screen built into the table, making for a big, heavy, cumbersome piece of furniture.
As a recent college grad I'm likely to be moving a fair amount in my immediate future.
As such I don't want to be lugging around this huge table, limiting where I can move to based on dining/living room space.
Additionally, if I have this system portable enough I can bring it to someone else's home and play there, or even take it to my local game store.
It gives me more freedom to play how I want, where I want.

## Next Steps
Here I'll give a small checklist of next-steps, checking them off with dates as I complete them.
1. Create a blank window \[Completed 11/5/26]
   - This will be referred to as the "table window"
2. Create a second window
   - This will be referred to as the "GM window"
3. Print an image to the table window
4. Allow the GM window to open images and display them on the table window
