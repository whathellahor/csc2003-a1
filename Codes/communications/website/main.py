# DEFINE IMPORTS
from flask import Flask, render_template, request, jsonify
from flask_cors import CORS
import ssl
from flask_mqtt import Mqtt

# DEFINE FLASK
app = Flask(__name__)
cors = CORS(app)

# APP CONFIG
app.config["TEMPLATES_AUTO_RELOAD"] = True
app.config['MQTT_BROKER_URL'] = '3eb3b23922da49208766dd4507ecd30c.s1.eu.hivemq.cloud'
app.config['MQTT_BROKER_PORT'] = 8883
app.config['MQTT_USERNAME'] = 'user-3'  # Set this item when you need to verify username and password
app.config['MQTT_PASSWORD'] = 'password'  # Set this item when you need to verify username and password
app.config['MQTT_TLS_ENABLED'] = True  # If your broker supports TLS, set it True
app.config['MQTT_TLS_CA_CERTS'] = 'mqtt\python-mqtt\isrgrootx1.pem'
app.config['MQTT_TLS_CERTFILE'] = 'mqtt\python-mqtt\mqtt-client-cert.pem'
app.config['MQTT_TLS_KEYFILE'] = 'mqtt\python-mqtt\mqtt-client-key.pem'
app.config['MQTT_TLS_INSECURE'] = False
app.config['MQTT_TLS_VERSION'] = ssl.PROTOCOL_TLSv1_2
app.config['MQTT_TLS_CIPHERS'] = 'DEFAULT:!aNULL:!eNULL:!MD5:!3DES:!DES:!RC4:!IDEA:!SEED:!aDSS:!SRP:!PSK'
topic = 'testtopic/1'

# MQTT WRAPPER
mqtt_client = Mqtt(app)

# DATA QUEUE
all_data = []

# MQTT ENDPOINT FOR CONNECTION
@mqtt_client.on_connect()
def handle_connect(client, userdata, flags, rc):
    if rc == 0:
        print('Connected successfully')     # Prompt if successfully connected to MQTT
        mqtt_client.subscribe(topic)        # Subscribe topic 
    else:
        print('Bad connection. Code:', rc)  # Prompt if did not connected to MQTT

# MQTT ENDPOINT FOR INCOMING MESSAGE
@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, message):
    data = dict(
        topic=message.topic,
        payload=message.payload.decode()
    )
    all_data.append(data)

# INDEX ENPOINT
@app.route('/index',methods = ['GET'])
def handle_mqtt_message():
        return render_template("index.html")

# MAIN ENDPOINT
@app.route('/')
def index():
    return render_template("index.html")

# GET DATA FROM DATA STACK ENDPOINT
@app.route('/get_data')
def get_data():
    # IF DATA IN DATA QUEUE MORE THAN 0
    if len(all_data) > 0:
        # DEQUEUE DATA
        ret = dict(all_data[0])['payload']
        del all_data[0]
        # RETURN TOP OF QUEUE
        return jsonify(message={"value":ret})
    else:
        return jsonify(message=None)


# MAIN PROGRAM ENDPOINT
if __name__ == "__main__":
    app.run(port=8080, debug=False)