# Ultrasonic

## Smoothing algorithm
The smoothing algorithm helps to filter out the noise readings to ensure the accuracy of calcuation of the distance.
<br>
In this code, moving average is done to callibrate the sensor data.
<br>
This is done by keeping an array of the sensor data and averaging them out afterwards.
<br><br>
Moving average works by appending the distance readings from the ultrasonic sensor into the distanceArray until the number of elements in the distanceArray is the same as the sample size
<br><br> Example:
<br>
[15, 18, 19,....]

> <b>Average distance:</b> Sum of distances in distanceArray / Sample size

From our testings, we have derived that the sample size for the smoothing algorithm is 30 as shown in the graph below

### Smoothing graphs
<table>
<tr>
<th>
Before smoothing
</th>
<th>
After smoothing (sample size 30)
</th>
</tr>
<tr>
<td>
<img src=https://user-images.githubusercontent.com/112333943/204502769-818579af-8979-4dae-8e0a-8a508e683eb0.png>
</td>
<td>
<img src=https://user-images.githubusercontent.com/112333943/204502808-e01e0147-ee1a-429f-a40e-46f328430841.png>
</td>
</tr>
</table>

# Accelerometer
Development was done on the Raspberry Pi PICO. Instead of the default I2C pins, pins 16 and 17 are used for the SDA and SCL pins respectively.

The accelerometer is used to detect if the car has gone up a hump. 

Upon initiation, a repeating timer is called to iterate the method: triggetAcc_callback. This method contains the main function of detecting the hump and calculating the height of hump. It is set to call back every 250ms. Global Boolean variables startFlag (detect if car has first started) and hump (detect hump) are also initialised with the value of False.

After initiation, the code checks the startFlag. Since startFlag is 0, it will run the code to:

<ol>
<li>Capture initial acceleration value (Ay)</li>
<li>Set the clipping range based on Ay</li>
<ul>
<li>If the acceleration value is within this range, the car is moving on flat ground.</li>
<li>The clipping range is set as 0.15. At 0.10, it is too sensitive while at 0.20, it is not really sensitive.</li>
</ul>
<li>Set startFlag as True so that code will no longer go into this if function</li>
</ol>

After obtaining the range of values for which the car is moving on flat ground, it will check for the presence of the hump (getDetectHump(hump, Ay)).

### getDetectHump(hump, Ay)

This method that returns a boolean value of onHump. Itis used to check if the car has gone up a hump or not.

<b>If a hump has been detected:</b>

<ol>
<li>Checks if car is moving within range of values that indicate it’s on the ground</li>
<ul>
<li>If it’s not within the range and Ay > 0, it indicates that car is going up the hump</li>
</ul>
<li>Set the accUp = Ay to store the acceleration value of car going up the hump</li>
<li>Set hump = 1 to indicate car is on hump</li>
<li>Sets onHump as true</li>
</ol>

<b>If car is going down the hump:</b>

<ol>
<li>Checks if hump == 1 (car is on hump)</li>
<li>Call the method (getHumpHeight(accUp))to calculate height of hump</li>
<li>Set hump = 0 to indicate it’s not on a hump</li>
<li>Set onHump as false</li>
</ol>

### getHumpHeight(float Acc)

This method returns the height of the hump as a float

> <b>Height calculation:</b><br>
height = Acc / 0.14;<br>
0.14 is the approximate change of acceleration of when the accelerometer moves 1cm
