import serial


PORT = '/dev/ttyS0'
BAUD = 115200

def open_uart():
    return serial.Serial(PORT, BAUD, timeout=2)

def uart_send(ser, cmd: bytes):
    ser.write(cmd)

