# lineDrawer
Draws an image using only straight lines and computes a single path to create it.

# RunLineDraw
Based on the website [linify.me](http://linify.me), this finds the darkest point on an image, and then chooses the darkest line passing through it. Since there aren't any actual points used the shortest path is not computed.
## options
- -i,  --input : Input image.
- -o,  --output : Output image.
- -n,  --number : Number of lines to draw.
- -d,  --darkness : Darkness of lines. Integer from 1 to 255, with 255 being completely black.
- -v, --inverted : Draw white lines on a black background

# RunLineRect
Finds the darkest line on the image between two points on the outside of the image and draws it.
## options
- -i,  --input : Input image.
- -o,  --output : Output image.
- -n,  --number : Number of lines to draw.
- -p,  --points : Number of points on outside
- -d,  --darkness : Darkness of lines. Integer from 1 to 255, with 255 being completely black.
- -v, --inverted : Draw white lines on a black background.
- -t, --numPaths : Maximum number of paths to check.

# RunLinePolygon
Finds the darkest line on the image between two points on a polygon or ellipse and draws it.
## options
- -i,  --input : Input image.
- -o,  --output : Output image.
- -n,  --number : Number of lines to draw.
- -p,  --points : Number of points on outside
- -d,  --darkness : Darkness of lines. Integer from 1 to 255, with 255 being completely black.
- -s,  --sides : Number of sides on polygon. Less than three will create an ellipse.
- -w,  --width : Width of the encompassing ellipse.
- -h,  --height : Height of encompassing ellipse.
- -x,  --xOffset : x position offset.
- -y,  --yOffset : y position offset.
- -a,  --angle : Rotation angle.
- -v, --inverted : Draw white lines on a black background
- -t, --numPaths : Maximum number of paths to check.

# Tips
After the program decides on what edges to use it computes the shortest path that travels through every required edge. This algorithm has a rather high complexity and would take an exceptionally long time to compute the true shortest path, so in order to speed it up you can set a maximum number of paths to check per node. Increasing the number of lines increases the computation time linearly while increasing the number of points increases it quadratically. If you use a small number of points there will not be enough lines that can be drawn to create a similar image. If you have too many points the program may waste lines on short lines between close edges, so you will need to adjust the settings until the image looks correct. The RunLineDraw program is considerably faster since it can make use of a greedy algorithm to check fewer lines. Inverted pictures create an interesting look but will often leave black eye sockets on portraits.

## Examples  
#### Input  
![Input image](https://raw.githubusercontent.com/nathanbain314/lineDrawer/master/examples/turing.jpeg)  
#### RunLineDraw  
![Draw](https://raw.githubusercontent.com/nathanbain314/lineDrawer/master/examples/lineDraw.png)  
#### Inverse  
![Inverse](https://raw.githubusercontent.com/nathanbain314/lineDrawer/master/examples/lineDrawInverse.png)  
#### RunLineRect  
![Rectangle](https://raw.githubusercontent.com/nathanbain314/lineDrawer/master/examples/lineRect.png)  
#### RunLinePolygon  
Triangle flattened and rotated  
![Triangle](https://raw.githubusercontent.com/nathanbain314/lineDrawer/master/examples/triangle.png)  
Pentagon narrowed and rotated  
![Pentagon](https://raw.githubusercontent.com/nathanbain314/lineDrawer/master/examples/pentagon.png)  
Hexagon  
![Hexagon](https://raw.githubusercontent.com/nathanbain314/lineDrawer/master/examples/hexagon.png)  
Ellipse with 64, 128, 256, 512 points and 256, 512, 1024, 2048 lines of darkness 256, 128, 64, 32.  
![Ellipse](https://raw.githubusercontent.com/nathanbain314/lineDrawer/master/examples/ellipse.png)  
#### Path
I used the computed path and a single line of string to create a real example on a 16 sided polygon. The first picture is on a wall, the second is silhouetted on a bright sky, and the third is the shdoow of the string
![String](https://raw.githubusercontent.com/nathanbain314/lineDrawer/master/examples/string.png)  
