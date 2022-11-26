 from m5stack import *
from m5ui import *
from uiflow import *
import time
from m5mqtt import M5mqtt
from random import randint

setScreenColor(0x111111)

label0 = M5TextBox(0, 23, "label0", lcd.FONT_Default, 0xFFFFFF, rotate=0)
label1 = M5TextBox(0, 83, "label0", lcd.FONT_Default, 0xFFFFFF, rotate=0)
label2 = M5TextBox(0, 143, "label0", lcd.FONT_Default, 0xFFFFFF, rotate=0)

PASSW = 'password'
CLIENTID = 'm5stack-test'
USER = 'user-2'
HOST = '3eb3b23922da49208766dd4507ecd30c.s1.eu.hivemq.cloud'
payload = None

def fun__(topic_data):
  global payload
  M5Led.on()
  wait_ms(3)
  M5Led.off()
  payload = topic_data
  pass

def send_text_mqtt(msg):
  m5mqtt.publish(str('testtopic/1'), str(msg + str(randint(0,100))), 0)
  label1.setText('Message sent!')
  wait(1)
  label1.setText('')

m5mqtt = M5mqtt(CLIENTID,HOST,8883,USER,PASSW,300,ssl=True,ssl_params={'server_hostname':HOST})
m5mqtt.subscribe(str('testtopic/1'), fun__)

uart1 = machine.UART(1, tx=32, rx=33)
uart1.init(115200, bits=8, parity=None, stop=0)


while True:
  counter = counter + 1
  wait_ms(5000)
  uart1.write("counter :" +str(counter))
  msg = str(uart1.read())[2:-1]
  send_text_mqtt(msg)