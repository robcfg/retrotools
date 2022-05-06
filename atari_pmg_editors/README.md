# Atari PMG Editors

## What are these for?

Some time ago I decided to take a look at what can be done on an Atari 800XL, as it was my first computer, and back in the day there was no internet, so information was scarce.

I find tinkering with an old computer's graphics system the best entry point to the machine. So I learned how to initialize the display lists, how the video memory is organized, and ended up toying with the Atari's Player and Missile graphics.

At this point, I was entering values for the graphics manually, which is rather cumbersome, so I decided to create these HTML editors to make it easy and fast to create your Player and Missile graphics, and get the byte values in different bases.

You'll notice that there are two very similar editors, one for Player graphics and one for Missile graphics. This is because the encoding of the image data is slightly different and I also wanted to know which bits are assigned to each Missile graphic.

## How do I use them?

It couldn't be easier!

Just open one of the HTML files on a web browser and click on any cell and it will flip its state. White cells are background/transparent cells, while colored cells represent visible pixels.

Once you've created your graphics, simply copy the values into your assembly source file and load them to the appropriate address.

![Player Graphics Editor](/pictures/AtariPlayerEditor.png) ![Missile Graphics Editor](/pictures/AtariMissileEditor.png)

## That's it!

I hope you find these tools useful. If you find any error, or think there's a missing feature, please read the [contributing guidelines](/CONTRIBUTING.md).
