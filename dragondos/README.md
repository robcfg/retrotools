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
  current directory.\
  Binary files are extracted with the 9-byte DragonDOS header by default. To\
  extract them without the header, use the **-strip_binary_header** flag.

      dragondos extract mydisk.vdk
      dragondos extract mydisk.vdk 2
      dragondos extract mydisk.vdk 2 -strip_binary_header

* **new \<filename\> \<image size\> [\<image type\>] [\<sides num\>]**

  Creates a new, formatted disk image.\
  The size of the image in Kilobytes needs to be specified.\
  Valid values for the image size are 180, 360 and 720.\
  Optionally, you can specify a disk image format using the desired\
  format index from the **listimages** command.\
  The default image type is .VDK.\
  If you are creating a 360KB disk, you can specify the number of sides\
  (1 or 2) after the disk image format.\
  The default number of sides is 1.

      dragondos new mydisk.vdk 180     (1 side,  40 tracks, 180KB .VDK file)
      dragondos new mydisk.imd 720 2   (2 sides, 80 tracks, 720KB .IMD file)
      dragondos new mydisk.jvc 360 1 2 (2 sides, 40 tracks, 360KB .JVC file)

* **delete \<filename\> \<index\>**

  Deletes a file from the disk image.\
  A backup copy of the disk image will be created.

      dragondos delete mydisk.vdk 4

* **insertbasic \<image filename\> \<filename to insert\>**

  Inserts an ASCII Basic file into the requested disk image and tokenize it.\
  The name and extension of the file to be inserted will be truncated to\
  fit the 8:3 size limitation, and converted to uppercase.\
  Addtionally, the type of the file, ascii or binary, needs to be specified.

      dragondos insertbasic mydisk.vdk PROGRAM.BAS

  A backup copy of the disk image will be created.

* **insertbinary \<image filename\> \<filename to insert\>  \<load_address\> \<exec_address\>**

  Inserts a binary file into the requested disk image.\
  The name and extension of the file to be inserted will be truncated to\
  fit the 8:3 size limitation, and converted to uppercase.\
  Addtionally, the load and exec addresses need to be specified.\
  They can be supplied as decimal or hexadecimal numbers (starting with 0x).

      dragondos insertbinary mydisk.vdk file.bin 3072 0xC00

  A backup copy of the disk image will be created.

* **insertdata \<image filename\> \<filename to insert\>**

  Inserts a data file into the requested disk image.\
  The name and extension of the file to be inserted will be truncated to\
  fit the 8:3 size limitation, and converted to uppercase.

      dragondos insertdata mydisk.vdk file.dat

  A backup copy of the disk image will be created.

* **listimages**

  Displays a list of the available disk image formats and their\
  indices for use with the **new** command.

  |   |                     |                  |
  |---|---------------------|------------------|
  | 0 | Dragon VDK files    | (\*.vdk)         |
  | 1 | JVC disk images     | (\*.jvc, \*.dsk) |
  | 2 | ImageDISK IMD files | (\*.imd)         |
  | 3 | Raw image files     | (\*.\*)          |

## How to build it

* Install [CMake](https://cmake.org/)

* Install [FLTK](https://www.fltk.org/) (Minimum required version is 1.3.8)

* Clone this repository (git clone https://github.com/robcfg/retrotools.git)

* On Windows, edit CMakeLists.txt to set the path of the FLTK installation.

* Open a terminal to the folder where you cloned the repository and navigate to the dragondos/build folder.

* Type `cmake -DCMAKE_BUILD_TYPE=Release ..` This will generate a Makefile on MacOS and Linux platforms, and a Visual Studio solution on Windows.

* On MacOS and Linux, type `make`. On Windows, open the solution with Visual Studio, select 'Release' as build type and build the solution.

### Notes

* FLTK has started deprecating (X)Forms for Wayland on Linux, so starting on version 1.4.4, they are not built by default.\
  This creates a dependency even on non-Linux platforms. If this is your case, you'll need to compile FLTK using the\
  FLTK_BUILD_FORMS flag.

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
