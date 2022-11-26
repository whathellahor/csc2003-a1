There are two different hardware to configure for this to work. 

1. M5 Stick C Plus
2. PICO Board

For M5 Stick C Plus, source code is under m5-UART/m5-code. There are two ways to configure the M5 Stick C Plus: 

1. VSCode with vscode-m5stack-mpy extension
To configure, download M5Burner, and configure the M5 Stick C Plus as USB mode. Then connect the M5 Stick C Plus to the laptop via USB Cable. Once the extension is done, use the extension to "Add M5Stack", select the correct COM, navigate to the M5 folder. Enter the source code and run in M5Stick. 

2. Online Compiler UIFLOW
To configure, use the M5Burner software to configure the M5Stick as Wi-Fi mode. Once the M5 is connected to the Wi-Fi, it will be display the API key which can be used to connect via the online compiler. Navigate to the code section, enter the code there and click run. 

For the pico, drag the code under m5-UART/pico-code and place together with the make file. 

Once both hardware has been setup, the m5 will initiate the first data transfer to pico, and it will cause an interrupt in the pico, and a message will be sent back to the m5. The data will be read by the m5, and will publish this message to the MQTT. The web application will be constantly subscribing for data from the MQTT topic. 

To view the message on the topic, run the file under mqtt-certs/mqtt_subscribe.py. 