#!/bin/python3

import serial
import os
import sys
from influxdb import InfluxDBClient
from datetime import datetime
import yaml


def main():
    # Read configuration file
    with open('config.yaml', 'r') as file:
        config = yaml.safe_load(file)

    # Set up InfluxDB connection
    influxClient = InfluxDBClient(host=config["influxdb"]["host"], port=config["influxdb"]["port"],
                                  username=config["influxdb"]["username"], password=config["influxdb"]["password"])
    influxClient.switch_database(config["influxdb"]["database"])

    # Detect the serial port (sometimes it'll be ttyACM0 or ttyACM1, gracefully handle either)
    serialPort = serial.Serial()
    for f in os.listdir("/dev"):
        if "ttyACM" in f:
            serialPort = serial.Serial("/dev/" + f, 9600)
            break

    # Main loop that reads from the serial port, parses the data, and logs it to InfluxDB
    while True:
        # Check for data in serial port
        if serialPort.in_waiting > 0:
            # Read data out of the buffer until a carriage return / new line is found
            serialString = serialPort.readline()

            # Attempt to decode the information
            # Sometimes, partial data is read and should be discarded, hence the try/except
            try:
                parts = serialString.decode('Ascii').strip("\r\n").split(',')
                print(parts)
                gallonsIn = float(parts[0].split(':')[1])
                flowRateIn = float(parts[1].split(':')[1])
                gallonsOut = float(parts[2].split(':')[1])
                flowRateOut = float(parts[3].split(':')[1])
                print("-")

                # Build a json object to send to InfluxDB
                current_time = datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%SZ')
                json_body = [
                    {
                        "measurement": "gallonsIn",
                        "tags": {
                            "direction": "in",
                            "meterID": "1"
                        },
                        "time": current_time,
                        "fields": {
                            "value": gallonsIn
                        }
                    },
                    {
                        "measurement": "flowRateIn",
                        "tags": {
                            "direction": "in",
                            "meterID": "1"
                        },
                        "time": current_time,
                        "fields": {
                            "value": flowRateIn
                        }
                    },
                    {
                        "measurement": "gallonsOut",
                        "tags": {
                            "direction": "out",
                            "meterID": "2"
                        },
                        "time": current_time,
                        "fields": {
                            "value": gallonsOut
                        }
                    },
                    {
                        "measurement": "flowRateOut",
                        "tags": {
                            "direction": "out",
                            "meterID": "2"
                        },
                        "time": current_time,
                        "fields": {
                            "value": flowRateOut
                        }
                    }
                ]

                # Send data to InfluxDB
                influxClient.write_points(json_body)

            except:
                print("Data error, skipping this reading. Error:")
                print(sys.exc_info())


if __name__ == "__main__":
    main()
