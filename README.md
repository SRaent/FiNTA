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
An example for setting the sigma_smooth parameter to 0.5 would be:
```
sigma_smooth 0.5
```

## Cropping the image
Since many imaging programs add a scalebar or other information at various places to the aquired image, it is possible to only analyze a specified part of the opened image. If the whole image was to be analyzed, any scalebars or text added to it would be considered to be features of the image and traced accordingly, skewing the outcome. In order to crop the image before tracing, the keyword "crop" has to be used, followed by 4 arguments seperated by whitespaces. The four arguments contain two edges of the cropping rectangle in the order, x-coordienate of the first corner of the corpping rectangle, y coordinate of this corner, x coordinate of the other corner and y coordinate of that corner. All operations will be performed on the cropped image. A example for how to cropp the image would be:
```
crop 0 0 1024 884
```

## Tracing start points
In order for the algorithm to function, at least one starting point has to be set on the image. Starting points can be set manually by the keyword "startpoint" followed by the x and y koordinate of the startpoint. A arbitrary number of startpoints can be defined this way by simply using the "startpoint" keyword in several lines each followed by the corresponding koordinates.
A function to automatically place startpoints is also provided. It is activated by the keyword "auto_startpoint" and can take between 0 and 3 arguments. The first argument is the maximal number of start points to be created and its default value is 100. The second argument is the minimal distance between the start points. Here already manually created start points are NOT taken into account. Its default vaulue is 20 * r\_step. The third argument is the minimum of the negative smaller eigenvalue of the hessian at the startpoint position. It can not be chosen below 0 and its default value is 0. For more information on how this function operates see the choosing\_parameters.pdf that is provided with the programm. A example for defining a startpoint manually would be:
```
startpoint 100 100
```
and for defining the settings for automated startpoint generation:
```
auto_startpoint 100 60 10
```
where a maximal number of 100 startpoints will be generated at least 60 pixel apart and with a minimum negative second order derivative of 10.


## Drawing commands
Drawing commands are initiated by the "draw" keyword. Next a number of properties of the image to be drawn can be specified in any order behind the "draw" keyword seperated by whitespaces. These properties have to be specified in the same line as the "draw" keyword.
A arbitrary number of drawing commands can be specified in the settings file, that will all be interpreted and result in a distinct image.
The path, the image will be saved to has to be specified directly behind the "name" keyword only seperated by a whitespace. Only certain file endings are allowed here. Supporded endings are: .bmp, .bmp, jpeg, .jpg, .jpe, .jp2, .png, .pbm, .pgm, .ppm, .sr, .ras, .tiff and .tif. If no ending is specified, the eding of the input image is used.
The background to be drawn on can be specified by exatly one of the following keywords per drawing command: "original", "cropped", "tubeness", "hessian", "visualized\_hessian", "white" and "black". All of these backgrounds will have the size of the cropped image exept the "original" backgroud. The backgrounds "original", "cropped", "white" and "black" are assumed to be self explanatory. The background "tubeness" maps each pixel of the cropped image to the smaller eigenvalue of the hessian with flipped sign. Values obtained this way are then rescaled to be between 0 and 255. The background "hessian" uses the three color channels blue, green and red to encode the values of the computed hessian a, b and c respectively. These values are also rescaled to be between 0 and 255 while all three channels are scaled uniformely. for more information on what the hessian matrix is see the choosing\_parameters.pdf that is provided with the programm. The background "visualized\_hessian" also containes all information about the hessian matrix in each pixel, but packaged in a more visible way. This background is generated in the HSV colorspace. The Hue (H) represents the direction of the smaller eigenvector of the hessian. The saturation (S) represents the relationship between the absolute values of the smaller and bigger eigenvalue of the hessian and the value (V) or brightness represents the size of the smaller eigenvalue of the hessian with flipped sign.
If only the background is specified, nothing will be drawn on it and it will be saved to the specified path directly. It is possible to draw the traced network in two different stages on the network before saving the image. The keywords for these two networks are "network" for the entire network and "only\_loops" for only the nodes that are part of closed loops. Also both of these networks can be drawn on the same background. In this case the entire network will always be drawn first and then be partially covered by the "only\_loops" protion of it. After each of the two keywords "network" and "only\_loops", 0 to 4 numerical values can be specified that determine the thickness of the lines and their color. If 0 numbers are provided, the default values are used. For "network" the default color is blue (255 0 0) and the default thickness is 1 pixel. For "only\_loops" the default color is red (0 0 255) and the default thickness is also 1 pixel. If one numerical value is provided, it is interpreted as the grayscale brightness to be drawn and the line thickness remaines 1 pixel. If two numerical values are provided after any of the two keywords "netowrk" and "only\_loops" the first one is interpreted as the grayscale value of the color and the second one as the line thickness in pixels. If three numerical values are provided, they are interpreted as a color, where the first number is used for the blue channel, the second one for the green channel and the thrid one for the red channel. All values relating to the color or grayscale value have to be between 0 and 255. If 4 numerical values are provided, the first three are again interpreted as a color and the fourth one is used as the line thickness in pixel.
A example for a drawing command would be:
```
draw original network 255 0 0 2 name ./results/test.png only_loops 0 0 255
```
Where the whole network will be drawn on the whole original image in blue with a line thickness of 2 pixels and the closed loops will be drawn on top of the already drawn network in red with a line thickness of 1 pixel. The image will be saved in the subfolder "results" with the name "test.png".
A different use if the drawing command could be:
```
draw visualized_hessian name ./results/vis_hes.tif
```
Where the visualized hesian of the cropped image will be saved without anything drawn on it.

## Setting the image scale
Since pixel can be a cumbersome unit to use, it is possible to define a arbitrary unit into which all output data will be converted. Also certain parameters can be defined using this unit. The keyword used to define such a unit is "set\_scale". It has to be followed by a number of pixels and a number of units the amount of pixels corresponds to. These two values have to be seperated by a "=" and the order in which they are stated is irrellevant. The number of pixels has to be followed by "px" or "pixel" after a whitespace. The number of units has to be followed by the word that defines the unit also after a whitespace. Examples to use "set_scale" would be:
```
set_scale 300 px = 1000 nm
```
or
```
set_scale 1 um = 300 pixel
```
where "um" or "nm" are from now on a keyword for the conversion of the now defined unit to pixels. The "set_scale" keyword can only be used once and only one unit can be defined this way. 
All positions or lengths of the output data will be converted to this unit and all areas will be converted to the unit squared. This will not be noted or indicated in the files where the data is saved.
Now it is also possible to define certain numerical arguments for other keywords using this unit. For example:
```
draw original network 255 0 0 8 nm name ./results/test.png
```
where the "8 nm" will be converted to 2.4 pixel automatically. This conversion does not work on a few specific parameters, that are scale invariant. Specifically: "sigma\_smooth", "steps", "thresh" and "min\_loop\_length", because defining them in terms of length does not make any sense. Note that the defined unit can only be used AFTER "set\_scale" has been called.

## Saving extracted data
Currently the programm only supports a small number of parameters that are extracted form the traced network.

### Saving loop areas
In order to save the loop areas, the keyword "save\_loop\_areas" is used. The path where the loop areas will be saved can be specified after a whitespace.

### Save loop circumferences
