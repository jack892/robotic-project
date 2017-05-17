## Welcome to my robotic project

I've upgrade an old robotic project made by a group of guys for an university project, with the help of my co-worker [Alessandro](https://github.com/AMDeveloperC).

The main project is developed to be run on a microcontroller, Galileo v2 by Intel, the purpose was to drive a little car by hand gesture with the [SparkFun RGB and Gesture Sensor](https://www.sparkfun.com/products/12787).

Our upgrade consist to add a new feature to the little car, allow it to bypass an obstacle by itself. We used [PING)))™ ultrasonic sensor](https://www.parallax.com/product/28015), which it shoot an ultrasonic burst (well above human hearing) and then "listen" for the echo return pulse. The sensor measures the time required for the echo return,in this way we can calculate the distance between the snesor and the obstacle.

Below you can see our .ino file we have written to configure the sensor and its functionality to merge at the main project and than adding the new feature.

