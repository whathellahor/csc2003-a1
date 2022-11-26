import ssl
import paho.mqtt.client as paho
import paho.mqtt.subscribe as subscribe
from paho import mqtt
import os
from config import *

# Configuring terminal color
START = '\33[42m'
END = '\33[0m'

# Dallback to print a message once it arrives
def print_msg(client, userdata, message):
    msg = message.payload.decode("utf-8")
    print("%s : %s" % (message.topic, msg))

# Set the SSL protocols
sslSettings = ssl.SSLContext(mqtt.client.ssl.PROTOCOL_TLS)

# put in your cluster credentials and hostname
auth = {'username': username, 'password': password}

# Clear console
clear = lambda:os.system('cls')
clear()

print(START + "Incoming message " + END)

try:
    subscribe.callback(print_msg, topic, hostname=broker, port=port, auth=auth, tls=sslSettings, protocol=paho.MQTTv31)
except:
    print("Program exited!")