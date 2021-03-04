#! /usr/bin/env python3
import paho.mqtt.client as mqtt
import time
import random

SERVER = "localhost"
topics = ["/thingy-ef43/1/data/temperature",
	"/thingy-ef43/1/data/humidity"]

client_id = 'thingy1'

def on_publish(client,userdata,result):             #create function for callback
    print("data published", result)
    pass

def on_disconnect(client, userdata, rc):
	print("Disconnect:", rc)

def main():
	client = mqtt.Client(client_id, userdata="thingy1") #create new instance
	client.on_publish = on_publish
	client.on_disconnect = on_disconnect

	while (True):
		temperature = random.randrange(180,220,1)/10
		humidity = random.randrange(200,400,1)/10
		msgs = ["%f" % temperature, "%f" % humidity]

		ret = client.connect(SERVER, port=1883, keepalive=60, bind_address="")
		print("Connect:", ret)
		for topic, msg in zip(topics, msgs):
			ret = client.publish(topic,msg)
			client.loop(timeout=1.0, max_packets=1)
			time.sleep(0.2)
			print("Publish:", topic, msg)

		time.sleep(60*5)
		client.disconnect()
		client.loop(timeout=1.0, max_packets=1)


if __name__ == '__main__':
	main()
