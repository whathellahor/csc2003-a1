# CSC2003 - Embedded Systems and Sensor Programming (Communications)

## M5StickC-Plus
There are two different hardware to configure for this to work.
1.	M5 Stick C Plus
2.	PICO Board

For M5 Stick C Plus, source code is under m5-UART/m5-code. There are two ways to configure the M5 Stick C Plus:
1.	VSCode with vscode-m5stack-mpy extension To configure, download M5Burner, and configure the M5 Stick C Plus as USB mode. Then connect the M5 Stick C Plus to the laptop via USB Cable. Once the extension is done, use the extension to "Add M5Stack", select the correct COM, navigate to the M5 folder. Enter the source code and run in M5Stick.
2.	Online Compiler UIFLOW To configure, use the M5Burner software to configure the M5Stick as Wi-Fi mode. Once the M5 is connected to the Wi-Fi, it will be display the API key which can be used to connect via the online compiler. Navigate to the code section, enter the code there and click run.
For the PICO, drag the code under m5-UART/pico-code and place together with the make file.
Once both hardware has been setup, the m5 will initiate the first data transfer to PICO, and it will cause an interrupt in the PICO, and a message will be sent back to the m5. The data will be read by the m5, and will publish this message to the MQTT. The web application will be constantly subscribing for data from the MQTT topic.
To view the message on the topic, run the file under mqtt-certs/mqtt_subscribe.py.

### MQTT Communication Protocol
We are using HiveMQ mqtt as our broker. With SSL enabled, the flask application require certificates to be able to publish/subscribe.
For mqtt_publish.py and mqtt_subscribe.py, it is used to test MQTT and to subscribe to topics.

## PICO to ESP-01
### Configuration
- Using UART0
-	Using PICO TX_PIN 0, RX_PIN1
-	Set debug value in headerfile to 0 for production
-	Set and get values from car_data global struct in headerfile

## PICO to HC05 (Bluetooth)
### Configuration
-	Using UART0
-	Using PICO TX_PIN 0, RX_PIN 1
-	Connect via computer Bluetooth
-	Password: 1234
-	Connect to Putty Serial to test, or, connect via serial Bluetooth application on Android to test

## Web Application
### How to run the application
-	Install python virtualenv pip install venv
-	Run python virtual environment python -m venv venv
-	Activate virtual env venv\Scripts\activate
-	Install dependencies pip install -r requirements.txt
-	Run program python main.py
-	Enter PEM password: password
-	Visit program URL at http://localhost:8080


