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



## Optimization

**pointers were used to access structs (pointer example in the smoothing algorithim**
![image](https://user-images.githubusercontent.com/92626804/204518718-3c48ed9d-710e-4cee-aeae-75d912302af5.png)

**Use of unsigned datatypes and also the use of uint8_t instead of other ints (The max value of uint8 is 255 e.g 1byte only)**

![image](https://user-images.githubusercontent.com/92626804/204518998-77ac5db9-0415-4c36-b052-5d98d021a639.png)

**User of timers and alarms instead of using countdown forloops**

![image](https://user-images.githubusercontent.com/92626804/204519245-49a14852-2d12-49f5-866f-9a495cbea3e7.png)

## Initialization of interrupts

![image](https://user-images.githubusercontent.com/92626804/204507955-8280d242-97c0-4547-bed3-e901b42d549b.png)
![image](https://user-images.githubusercontent.com/92626804/204506744-2df2bda9-4437-48f7-9471-2c51db176dc4.png)




## initialization of variables

![image](https://user-images.githubusercontent.com/92626804/204508233-096578d3-fcf5-4621-bd23-917f26999c17.png)


## Set up repeating Timer for trigger interval

Trigger pin will be set to High ever 30 miliseconds
![image](https://user-images.githubusercontent.com/92626804/204508473-13d8dcc3-639b-4d6a-9012-99852073f88d.png)


## Add an alarm so that trigger will remain high for X amount of mins, and then set it to low after the time is over

**Previously, trigger was set to high for only 10 microseconds, but after integration, 10 microseconds caused an error, only by
changing it to 10miliseconds did it fix the problem**

![image](https://user-images.githubusercontent.com/92626804/204509572-73c332cb-2e8b-4ed4-b132-3be1720295fa.png)
![image](https://user-images.githubusercontent.com/92626804/204510819-6bb30c63-87c7-4bec-9d50-39d5691c0d27.png)

## When echo pin interupts on a rising edge, it will start an alarm timer 
**alarm timer is to set the duration on when the program should timeout the echo pin.**

![image](https://user-images.githubusercontent.com/92626804/204511189-7c658796-dc7f-4f4a-9556-457c4637a3d9.png)


## If timout alarm reached and echo is still on HIGH it will call a callback function setting timeout boolean to True

![image](https://user-images.githubusercontent.com/92626804/204517755-c12d8b79-d257-4b3c-8ca4-0d4fdc408f95.png)


## if a falling edge is detected and timeout is false, the distance is valid

![image](https://user-images.githubusercontent.com/92626804/204518000-15802d7c-757b-4aa8-9552-c84aa82b337f.png)


## Get distance function
![image](https://user-images.githubusercontent.com/92626804/204518092-24f18d30-c8b4-4482-be1e-22224ad7a707.png)

##Smoothing algorithim

![image](https://user-images.githubusercontent.com/92626804/204518151-34979618-0b30-4ec2-b7e9-6a2d41001246.png)



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
