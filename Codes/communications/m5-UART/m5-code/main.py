from m5stack import *
from m5ui import *
from uiflow import *
import time
from m5mqtt import M5mqtt
from random import randint
# import binascii

setScreenColor(0x111111)

# Screen configuration for displaying data
label0 = M5TextBox(0, 23, "label0", lcd.FONT_Default, 0xFFFFFF, rotate=0)
label1 = M5TextBox(0, 83, "label0", lcd.FONT_Default, 0xFFFFFF, rotate=0)
label2 = M5TextBox(0, 143, "label0", lcd.FONT_Default, 0xFFFFFF, rotate=0)

# Configuration for MQTT server details
PASSW = 'password'
CLIENTID = 'm5stack-test'
USER = 'user-2'
HOST = '3eb3b23922da49208766dd4507ecd30c.s1.eu.hivemq.cloud'
payload = None
msg = None

# Function call when a message is subscribed
def fun__(topic_data):
  global payload, uart1
  payload = topic_data
  uart1.write(str(payload))
  pass

# Initiate the first publish message
def initiate_UART():
  m5mqtt.publish(str('testtopic/1'), "START", 0)

# Configuration for MQTT and UART connection
m5mqtt = M5mqtt(CLIENTID,HOST,8883,USER,PASSW,300,ssl=True,ssl_params={'server_hostname':HOST})
m5mqtt.subscribe(str('testtopic/1'), fun__)
m5mqtt.start()
initiate_UART()
uart1 = machine.UART(1, tx=32, rx=33)
uart1.init(115200, bits=8, parity=None, stop=1)

# Prompt message for M5 to display MQTT is connected
label0.setText("MQTT is connected.")

# Endless loop to poll for data recevied via RX 
while True:
  if uart1.any():
    msg = (uart1.readline()).decode()                 # Read the data sent, decode it and store in msg
    label1.setText("Recieving message: " + str(msg))  # Prompt message to show the message recieved via RX
    label2.setText("Sending message to MQTT")         # Prompt message that data is being sent to MQTT
    m5mqtt.publish(str('testtopic/1'), str(msg), 0)   # Publish the message to MQTT
  wait(2)                                             # 2 seconds delay