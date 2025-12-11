#!/bin/bash

# Target: Localhost UDP Port 10110
HOST="127.0.0.1"
PORT="10111"

echo "Blasting NMEA data at $HOST:$PORT..."

# Loop infinite
while true
do
  # Send a valid GPGGA string via netcat (nc)
  echo "\$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47" | nc -u -w 0 $HOST $PORT
  
  # Sleep very briefly (0.1s) to simulate 10Hz GPS
  # This is 5x faster than the consumer can handle!
  sleep 0.1
done