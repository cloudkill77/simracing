import serial
import time

# Open serial port (adjust to your Arduino's port, e.g., /dev/ttyUSB0 or COM3)
ser = serial.Serial('COM5', 9600, timeout=1)
time.sleep(2)  # Give time for Arduino to reset

# Send "OS_READY" signal
ser.write(b"1\n")

# Optionally, wait for Arduino's ACK
response = ser.readline().decode('utf-8').strip()
print(f"Arduino response: {response}")

ser.close()
