# MMBExplorer

![MMBExplorer logo](/mmbexplorer/logo/MMBExplorer.svg)

A manager of MMB disk collections for Acorn Computers 8-bit systems.

## What is this program for?

I created this program to manage MMB files ( which are collections of disk images for systems like the Acorn BBC Micro and the Acorn Electron ) for use with my recently acquired ElkSD-Plus1 cartridge.

Software on disks for these machines are stored as files called disk images. For use on the aforementioned cartidge, these disk images need to be packed into a MMB file, which contains an index and the disk images.

MMBExplorer lets you create and edit MMB files, insert, remove and extract disk images, change attributes and peek the content of the disk.

There are already programs to do that, but they are Windows-only and without source code. So I took myself to learn about MMB files and create this program that you can use on any platform you can compile it, and if you're curious about it, learn about its inner workings.

## How do I compile it?

It should be as easy as following these simple steps:

* Install [CMake](https://cmake.org/)

* Install [FLTK](https://www.fltk.org/)

* Clone this repository (git clone https://github.com/robcfg/retrotools.git)

* Open a terminal to the folder where you cloned the repository and navigate to the mmbexplorer/build folder.

* Type `cmake -DCMAKE_BUILD_TYPE=Release ..` This will generate a Makefile on MacOS and Linux platforms, and a Visual Studio solution on Windows.

* On MacOS and Linux, type `make`. On Windows, open the solution with Visual Studio, select 'Release' as build type and build the solution.

## How do I use it

MMBExplorer can work as a command-line application or as a graphical one. If you run MMBExplorer without any parameters, it will launch the graphical version.

* Command-line operation

Type `mmbexplorer help` for a description of the available commands and its parameters.

```
MMBExplorer by Robcfg (Git:e479665).

Usage:
      mmbexplorer                   - Without parameters, launch
                                      the graphical user interface.

      mmbexplorer COMMAND [ARGS...] - Executes the given command with
                                      the following arguments.

      Available commands:
          help                              - Show this text.
          create 'filename' slots           - Create a new MMB file with
                                              a number of SSD image slots
          list 'filename'                   - List content of a MMB file.
          add 'filename' 'ssdname' slot     - Add SSD image to a given
                                              slot in the MMB file.
          remove 'filename' slot            - Remove a SSD image from a
                                              given slot on a MMB file.
          lock 'filename' slot              - Lock image in given slot.
          unlock 'filename' slot            - Unlock image in given slot.
          extract 'filename' slot           - Extracts given slot disk as
                                              'slot.sdd' (i.e. 34.ssd).
          extract 'filename' 'ssdname' slot - Extracts given slot disk as
                                              given SSD image name.
```
                                              
Examples:

To list the contents of a file names **beeb.mmb**, type 
      
`mmbexplorer list beeb.mmb`

To lock the disk image in slot 451 of the same file, type 
      
`mmbexplorer lock beeb.mmb 451`

* Graphical mode operation

![MMBExplorer GUI](/pictures/MMBExplorer_GUI.png)

The same operations available through command-line operation are available here through menus and keyboard shortcuts.

Action | MacOS | Linux/Windows
------ | ----- | -------------
Open MMB file | CMD+O | CTRL+O
Close MMB file | CMD+C | CTRL+C
Create new MMB file | CMD+N | CTRL+N
Insert disk image in slot | CMD+I | CTRL+I
Extract disk image in slot | CMD+E | CTRL+E
Remove disk image in slot | CMD+R | CTRL+R
Lock image in slot | CMD+L | CTRL+L
Unlock image in slot | CMD+U | CTRL+U

A slot can be selected by left-clicking on it. A selected slot will appear with yellow background.

Slot operations will be performed on the selected slot or a slot number will be asked if no slot is selected.

Additionally, MMBExplorer supports dragging of a SSD disk image file from finder/file explorer into a slot.

MMB files can contain up to 511 images. On the scrollable table you can see the status of each image slot.

Each slot is composed of an status icon, the slot number and the disk name.

There are three possible status icons:

![Empty](/mmbexplorer/icons/empty.png) Empty. It means, well, that there is no disk image in that slot.

![Unlocked](/mmbexplorer/icons/unlocked.png) Unlocked. It means that a regular image is occupying that slot.

![Locked](/mmbexplorer/icons/locked.png) Locked. It means that there's an image occupying that slot and it's marked as read-only.

## Remarks

* Changes to the MMB file are written immediately. There's no undo functionality, so make a backup of your MMB file before manipulating it.

* Drag and Drop only works for inserting images into slots. Extracting them can only be done using the extract command or menu action.

## That's it!

I hope you find this tool useful. If you find any error, or think there's a missing feature, please read the [contributing guidelines](/CONTRIBUTING.md).