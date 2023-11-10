# DragonDOS/DragonDOSUI
A command-line and a graphic utility to manage disk images formatted with the DragonDOS file system for Dragon computers.

![DragonDOSUI](/pictures/DRAGONDOSUI.png)

The graphical utility allows viewing the content of the files within a disk image.
It can show the contents as hex dump, text, detokenized and colorized BASIC, and even render images!

![DragonDOS view modes](/pictures/DragonDOS_ViewModes.png)

## Usage
DragonDOSUI can be launched by double-clicking on the executable file or typing its name on a terminal window.

DragonDOS must be launched from a terminal window, by typing its name.

DragonDOS expects a command (or operation) to perform, followed by necessary and/or optional parameters.

**dragondos \<command\> [\<args\> ...]**

The following commands are available:
  
* **help**

  Displays help text.

      dragondos help

* **info \<filename\>**

  Displays disk image information like type, free space and number of files.

      dragondos info mydisk.vdk

* **list \<filename\>**

  Displays a list of the files on a disk image and the following data:\
  Index, file name, size in sectors and size in bytes.

      dragondos list mydisk.vdk

* **extract \<filename\> [\<index\>]**

  Extracts all files from within the disk image to the current directory.\
  If an optional file index is specified, only that file will be extracted to\
  current directory.

      dragondos extract mydisk.vdk
      dragondos extract mydisk.vdk 2

* **new \<filename\>**

  Creates a new, formatted disk image.

      dragondos new test.img

* **delete \<filename\> \<index\>**

  Deletes a file from the disk image.
  A backup copy of the disk image will be created.

      dragondos delete test.img 4

* **insert \<image filename\> \<filename to insert\> \<ascii|binary\>**

  Inserts a file into the requested disk image.\
  The name and extension of the file to be inserted will be truncated to\
  fit the 6:3 size limitation, and converted to uppercase.\
  Addtionally, the type of the file, ascii or binary, needs to be specified.

      dragondos insert test.img FILE.TXT ascii

## How to build it

* Install [CMake](https://cmake.org/)

* Install [FLTK](https://www.fltk.org/) (Minimum required version is 1.3.8)

* Clone this repository (git clone https://github.com/robcfg/retrotools.git)

* On Windows, edit CMakeLists.txt to set the path of the FLTK installation.

* Open a terminal to the folder where you cloned the repository and navigate to the dragondos/build folder.

* Type `cmake -DCMAKE_BUILD_TYPE=Release ..` This will generate a Makefile on MacOS and Linux platforms, and a Visual Studio solution on Windows.

* On MacOS and Linux, type `make`. On Windows, open the solution with Visual Studio, select 'Release' as build type and build the solution.

## Links    
* **Dragon computers**
  * https://en.wikipedia.org/wiki/Dragon_32/64
  * https://archive.worldofdragon.org/
  * http://dragondata.co.uk/

* **Disk image formats**
  * https://archive.worldofdragon.org/index.php?title=Tape%5CDisk_Preservation#VDK_File_Format

* **DragonDOS**
  * http://dragon32.info/info/drgndos.txt

* **Dragon BASIC**
  * http://dragon32.info/info/basicfmt.txt
  * http://dragon32.info/info/bastoken.txt
  * http://dragon32.info/info/dostoken.txt
