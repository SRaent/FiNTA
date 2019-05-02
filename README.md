# Automated tracing algorithm for fibers of uniform thickness
This programm was specifically developed to trace the actin fibers visible in SEM images of mammalian cells after triton extraction. It can likely also be applied to any other fiber system where the fibers have sufficiently uniform thickness.

# Usage
To apply the tracing algorithm to a given image, the path to the image, and the path to a configuration file have to be handed over to the programm. To archieve this, run the programm in the command line with the paths as arguments. The path to the image has to be passed as a argument after the "-f" flag and the path to the settings file after the "-s" flag. To successfully execute the programm, type something like
```
atafut -s /path/to/settings_file.set -f /path/to/image.tif
```

# The settings file
This file defines the tracing parameters as well as the output of the programm. Comments in this file are defined as everything behind a "#".
## Tracing parameters
Adjustments to the tracing parameters have a big impact on the tracing quality. Therefore they have to be chosen with great care. Guidelines on how to choose them are provided in the choosing\_parameters.pdf that is provided with the programm. The parameters that can be chosen are: "sigma\_smooth", "sigma\_conf", "r\_min", "r\_max", "steps", "thresh", "r\_step", and "min\_loop\_length". From these only "sigma\_smooth", "sigma\_conf","r\_max" and "r\_step" have to be chosen for the programm to run, but it is recommended to manually choose all tracing parameters. The default values for the parametes that do not have to be chosen are: r\_min = 0, steps = 360, thresh = 0 and min\_loop\_length = 6. Parameters are set by writing their name followed by a whitespace and then the value they are assigned. These parameters can only be assigned once.

## Cropping the image
Since many imaging programs add a scalebar or other information at various places to the aquired image, it is possible to only analyze a specified part of the opened image. If the whole image was to be analyzed, any scalebars or text added to it would be considered to be features of the image and traced accordingly, skewing the outcome.

## Tracing start points
In order for the algorithm to function, at least one starting point has to be set on the image. Starting points can be set manually by the keyword "startpoint" followed by the x and y koordinate of the startpoint. A arbitrary number of startpoints can be defined this way by simply using the "startpoint" keyword in several lines each followed by the corresponding koordinates.
A function to automatically place startpoints is also provided. It is activated by the keyword "auto_startpoint" and can take between 0 and 3 arguments. The first argument is the maximal number of start points to be created and its default value is 100. The second argument is the minimal distance between the start points. Here already manually created start points are NOT taken into account. Its default vaulue is 20 * r\_step. The third argument is the minimum of the negative smaller eigenvalue of the hessian at the startpoint position. It can not be chosen below 0 and its default value is 0. For more information on how this function operates see the choosing\_parameters.pdf that is provided with the programm.

## Drawing commands
Drawing commands are initiated by the "draw" keyword. Next a number of properties of the image to be drawn can be specified in any order behind the "draw" keyword seperated by whitespaces. The background to be drawn on can be specified by exatly one of the following keywords per drawing command: "original"