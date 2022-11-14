**Motor Control (PWM + PID)**
The further the car can go straight and better the marks (across the lab)
The more accurate the car can turn at various angles, the better the marks
Explain how your PID works with illustrations/demonstrations
Show and explain how you go about choosing your P, I and D.
Show how P, I and D can vary for long-distance vs short-distance travel
Show sufficient testing is done on this sub-module (black box/white box, etc)
The video should include all of the above. :)

**Infrared (Encoder + Barcode)**
The design of the wheel encoder to measure speed/distance with accuracy in mind
The design of the a barcode algorithm for Code 39 that can decode various lengths of barcode.
At least 2 (preferably 3) different "filtering" algorithms for the 2 sensors
Plot charts to illustrate the different performances (e.g. the one shown in Lecture)
Grades will vary depending on how well it can scan the various barcode lengths.
If the algorithm can handle various scan speeds, non-straight scanning (diagonal), etc... The better the grades.
The video should include all of the above. :)
Show sufficient testing is done on this sub-module (black box/white box, etc)

**Ultrasonic + Accelerometer (Hump Detection)**
Improvements to the ultrasonic driver's responsiveness
Software design to detect hump height using accelerometer.
Grades will depend on the sensor's accuracy to measure the distance of the object and the height of the hump.
At least 2 (preferably 3) different "filtering" algorithms for the 2 sensors
Plot charts to illustrate the different performances (e.g. the one shown in Lecture)
The video should be a demonstration to explain the various algorithms on each sensor and how you went on with your analyzing its accuracy and other differences.
Show sufficient testing is done on this sub-module (black box/white box, etc)

**Communications**
Show MSP/Pico connected to M5Stick via UART vs I2C
Show MSP/Pico connected to ESP01/HC05 via UART
You need to show at least 2 preferably 3 configurations.
Show performance in terms of throughput and latency.
Plot charts to illustrate the difference in performance (put it in GitHub)
The video should be a demonstration to explain the various configurations you have chosen and how you went on with your performance testing and analysing the differences.
Show sufficient testing is done on this sub-module (black box/white box, etc)

**Mapping & Navigation**
How much memory used to represent the map on the MSP or Pico
Mapping algorithm is not limited by the boundaries (better grades)
Navigation (Choose one: speed or distance)
Have a working version and an optimised version. Show proof that the code is optimised for speed or less resources used.
The video should be a demonstration how the algorithm works with some simulations. Also, explain how resources were reduced and performance gain were achieved.
Show sufficient testing is done on this sub-module (black box/white box, etc)

**Team Work**
Overall Design
Integration (level of completeness)
Show sufficient testing is done on the entire project
Demonstration
The final sub-modules chosen must meet the project requirements (it can be the simplest ones)
All code, charts and explanation must be in a single GitHub project.
There should be 6 folders in the GitHub (5 for each sub-module and 1 for the integrated one)
Code should be well commented and have sufficient explanation (README) on how it works.
All charts and diagrams (i.e. flowcharts, etc) should be in GitHub.
Video shows how the car maps the maze and shows the final map on the pc/mobile with the other data. Also include how the car navigates from point A to B.