# Ultrasonic
## Code logic
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
<br><br>
<b>Average distance:</b> Sum of distances in distanceArray / Sample size
<br><br>
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
