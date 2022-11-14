**Motor Control (PWM + PID)**<br>
The further the car can go straight and better the marks (across the lab)<br>
The more accurate the car can turn at various angles, the better the marks<br>
Explain how your PID works with illustrations/demonstrations<br>
Show and explain how you go about choosing your P, I and D.<br>
Show how P, I and D can vary for long-distance vs short-distance travel<br>
Show sufficient testing is done on this sub-module (black box/white box, etc)<br>
The video should include all of the above. :)<br>

**Infrared (Encoder + Barcode)**<br>
The design of the wheel encoder to measure speed/distance with accuracy in mind<br>
The design of the a barcode algorithm for Code 39 that can decode various lengths of barcode.<br>
At least 2 (preferably 3) different "filtering" algorithms for the 2 sensors<br>
Plot charts to illustrate the different performances (e.g. the one shown in Lecture)<br>
Grades will vary depending on how well it can scan the various barcode lengths.<br>
If the algorithm can handle various scan speeds, non-straight scanning (diagonal), etc... The better the grades.<br>
The video should include all of the above. :)<br>
Show sufficient testing is done on this sub-module (black box/white box, etc)<br>

**Ultrasonic + Accelerometer (Hump Detection)**<br>
Improvements to the ultrasonic driver's responsiveness<br>
Software design to detect hump height using accelerometer.<br>
Grades will depend on the sensor's accuracy to measure the distance of the object and the height of the hump.<br>
At least 2 (preferably 3) different "filtering" algorithms for the 2 sensors<br>
Plot charts to illustrate the different performances (e.g. the one shown in Lecture)<br>
The video should be a demonstration to explain the various algorithms on each sensor and how you went on with your analyzing its accuracy and other differences.<br>
Show sufficient testing is done on this sub-module (black box/white box, etc)<br>

**Communications**<br>
Show MSP/Pico connected to M5Stick via UART vs I2C<br>
Show MSP/Pico connected to ESP01/HC05 via UART<br>
You need to show at least 2 preferably 3 configurations.<br>
Show performance in terms of throughput and latency.<br>
Plot charts to illustrate the difference in performance (put it in GitHub)<br>
The video should be a demonstration to explain the various configurations you have chosen and how you went on with your performance testing and analysing the differences.<br>
Show sufficient testing is done on this sub-module (black box/white box, etc)<br>

**Mapping & Navigation**<br>
How much memory used to represent the map on the MSP or Pico<br>
Mapping algorithm is not limited by the boundaries (better grades)<br>
Navigation (Choose one: speed or distance)<br>
Have a working version and an optimised version. Show proof that the code is optimised for speed or less resources used.<br>
The video should be a demonstration how the algorithm works with some simulations. Also, explain how resources were reduced and performance gain were achieved.<br>
Show sufficient testing is done on this sub-module (black box/white box, etc)<br>

**Team Work**<br>
Overall Design<br>
Integration (level of completeness)<br>
Show sufficient testing is done on the entire project<br>
Demonstration<br>
The final sub-modules chosen must meet the project requirements (it can be the simplest ones)<br>
All code, charts and explanation must be in a single GitHub project.<br>
There should be 6 folders in the GitHub (5 for each sub-module and 1 for the integrated one)<br>
Code should be well commented and have sufficient explanation (README) on how it works.<br>
All charts and diagrams (i.e. flowcharts, etc) should be in GitHub.<br>
Video shows how the car maps the maze and shows the final map on the pc/mobile with the other data. Also include how the car navigates from point A to B.<br>