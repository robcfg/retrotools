# DOS68/DOS68UI
A command-line and a graphic utility to manage disk images formatted with Smoke Signal Broadcasting (SSD) DOS68, like those from Southwest Technical Products Corporation SWTPC 6800 machines with SSD disk controllers.

## Usage
Dos68ui can be launched by double-clicking on the executable file or typing its name on a terminal window.

Dos68 must be launched from a terminal window, by typing its name.

Dos68 expects a command (or operation) to perform, followed by necessary and/or optional parameters.

**dos68 \<command\> [\<args\> ...]**

The following commands are available:
  
* **help**

  Displays help text.

      dos68 help

* **info \<filename\>**

  Displays disk image information like type, free space and number of files.

      dos68 info fortran.img

* **list \<filename\>**

  Displays a list of the files on a disk image and the following data:\
  Index, file name, size in sectors and size in bytes.

      dos68 list fortran.img

* **extract \<filename\> [\<index\>]**

  Extracts all files from within the disk image to the current directory.\
  If an optional file index is specified, only that file will be extracted to\
  current directory.

      dos68 extract fortran.img
      dos68 extract fortran.img 2

* **new \<filename\>**

  Creates a new, formatted disk image.

      dos68 new test.img

* **delete \<filename\> \<index\>**

  Deletes a file from the disk image.
  A backup copy of the disk image will be created.

      dos68 delete test.img 4

* **insert \<image filename\> \<filename to insert\> \<ascii|binary\>**

  Inserts a file into the requested disk image.\
  The name and extension of the file to be inserted will be truncated to\
  fit the 6:3 size limitation, and converted to uppercase.\
  Addtionally, the type of the file, ascii or binary, needs to be specified.

      dos68 insert test.img FILE.TXT ascii

## How to build it

* Install [CMake](https://cmake.org/)

* Install [FLTK](https://www.fltk.org/)

* Clone this repository (git clone https://github.com/robcfg/retrotools.git)

* Open a terminal to the folder where you cloned the repository and navigate to the retrotools/build folder.

* Type `cmake -DCMAKE_BUILD_TYPE=Release ..` This will generate a Makefile on MacOS and Linux platforms, and a Visual Studio solution on Windows.

* On MacOS and Linux, type `make`. On Windows, open the solution with Visual Studio, select 'Release' as build type and build the solution.

## Links    
* **[The video that started it all](https://www.youtube.com/watch?v=ApNEkeAL4fA)**

* **Southwest Technical Products Corporation**
  * https://en.wikipedia.org/wiki/SWTPC
  * https://archive.org/details/swtpc-6800-computer-system
  * https://archive.org/details/hack42_SWTPC_6800_System_Documentation_Notebook_6800_Hardware
  * https://archive.org/details/hack42_SWTPC_6800_System_Documentation_Notebook_6800_Software
  * https://archive.org/details/hack42_SWTPC_6800_System_Documentation_Notebook_6800_Programming
  * https://archive.org/details/hack42_Southwest_Technical_Products_Corporation_SWTPC_6800_Disassembler

* **Smoke Signal Broadcasting**
  * https://en.wikipedia.org/wiki/Smoke_Signal_Broadcasting
  * https://archive.org/details/ssb-bfd-68-system-hw
  * https://deramp.com/downloads/swtpc/software/DOS68/Manuals/DOS09%20Manual.pdf
  * https://deramp.com/downloads/swtpc/software/DOS68/
  * https://deramp.com/downloads/swtpc/hardware/Smoke%20Signal%20Broadcasting/
  * https://archive.org/details/UpdateBFDLFDDOS68Ver51CC080ThroughDFFF1979SmokeSignalBroadcasting
  * https://archive.org/details/SmokeSignalBroadcastingFortranCompilerForAB000DOS681979
  * https://archive.org/details/SmokeSignalBroadcastingFortranCompilerFor67000DOS681979
  * https://archive.org/details/SmokeSignalBroadcastingFortranCompilerForCD000DOS681979
  * https://archive.org/details/MOSPROMUnknownSourceAssemblyCodeForMOSPROM1981
  * https://archive.org/details/smoke-signal-broadcasting-dos-68-disks

* **Magazines**
  * https://archive.org/details/68micro-vol-1-num-1
  * https://archive.org/details/68micro-vol-01-num-06
  * https://archive.org/details/68micro-vol-1-num-7
  * https://archive.org/details/68micro-vol-02-num-01
  * https://archive.org/details/68micro-vol-02-num-05
  * https://archive.org/details/68micro-vol-02-num-06
  * https://archive.org/details/68micro-vol-02-num-09

