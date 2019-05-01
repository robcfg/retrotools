# retrotools
Some nice programs aimed at preservation of software from old computers.

## What will you find here?
I'll be publishing some tools that I've made to help me restore media from old computers, or to help creating new programs for said computers.

There are tools for comparing disk images, for checking and edit data blocks on cassette, virtual filesystems to allow interacting with old media images through you favourite file explorer, image converters... You name it.

## Available tools
  * **Dskcomp**
  
    A command-line utility that compares structure and contents of disk image files in Marco Vieth, Ulrich Doewich and Kevin Thacker's DSK/EDSK format.
    
    **Usage:**

    DskComp filename1 filename2 [options]

    Options:
        -v
            Verbose, lots of info.

    DskComp returns with 0 if the disk images have the same contents, nonzero otherwise.
