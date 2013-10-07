All required brushes can be selected from the brush menu. I also added a "halo" brush which draws a halo shape based off the line thickness and circle size.

There is a drop down for the angle setting methods. The right click one draws a white line showing the direction.

The kernel settings can also be accessed from the brush menu. The sliders adjust the size of the kernel up to at most 9x9 and requiring both the number of rows and columns to be odd numbers. The divisor field is applied to the sum of the result after applying the kernel. 

I also implemented gradient direction for lines. This first takes in the image, applies a 5x5 blur filter, and then uses Sobel kernels to calculate the gradient at each point being painted. Using a similar process (the same without the initial smoothing) magnitudes of the gradient are also generated. If "clip edges" is selected from the brushes menu (it's a light button), if while performing a brush stroke, you go over a edge with magnitude higher than the threshold specified by the slider, the brush stroke stops.

The point in the original image being used can be seen with a red crosshair.
