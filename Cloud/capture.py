#! /usr/bin/env python3
import paho.mqtt.client as mqtt
import time
import random
import psycopg2
import sys

SERVER = "localhost"
topic = "/thingy-ef43/#" # #all under *in next folder

def storeDb(topic, msg):
	qry = "INSERT INTO test(topic, msg) VALUES ('%s', %f);" % (topic, float(msg))
	print(qry)

	conn = psycopg2.connect("dbname=airqdb user=airq password=airqpass host=localhost")
	cur = conn.cursor()
	cur.execute(qry)
	conn.commit()

	cur.close()
	conn.close()

	sys.stdout.flush()
	sys.stderr.flush()

def subscribe(client: mqtt):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")
        storeDb(msg.topic, msg.payload)

    client.subscribe(topic)
    client.on_message = on_message

def main():
	client = mqtt.Client() #create new instance
	ret = client.connect(SERVER, port=1883, keepalive=60, bind_address="")
	print("Connect:", ret)

	subscribe(client)
	client.loop_forever()

	client.disconnect()


if __name__ == '__main__':
	main()
