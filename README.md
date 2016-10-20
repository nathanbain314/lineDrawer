# lineDrawer
Draws an image using only straight lines.

#RunLineDraw
Based on the website [linify.me](http://linify.me), this finds the darkest point on an image, and then looks chooses the darkest line passing through it. 
##options
- -i,  --input : Input image.
- -o,  --output : Output image.
- -n,  --number : Number of lines to draw.
- -d,  --darkness : Darkness of lines. Integer from 1 to 255, with 255 being completely black.
- -v, --inverted : Draw white lines on a black background

#RunLineRect
Finds the darkest line on the image between two points on the outside of the image and draws it.
##options
- -i,  --input : Input image.
- -o,  --output : Output image.
- -n,  --number : Number of lines to draw.
- -p,  --points : Number of points on outside
- -d,  --darkness : Darkness of lines. Integer from 1 to 255, with 255 being completely black.
- -v, --inverted : Draw white lines on a black background

#RunLineEllipse
Finds the darkest line on the image between two points on an ellipse and draws it.
##options
- -i,  --input : Input image.
- -o,  --output : Output image.
- -n,  --number : Number of lines to draw.
- -p,  --points : Number of points on outside
- -d,  --darkness : Darkness of lines. Integer from 1 to 255, with 255 being completely black.
- -w,  --width : Width of eclipse.
- -h,  --height : Height of eclipse.
- -v, --inverted : Draw white lines on a black background