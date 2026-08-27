import serial

PORT = '/dev/ttyS0'
BAUD = 115200

def open_uart():
    return serial.Serial(PORT, BAUD, timeout=2)

def uart_send(ser, cmd: bytes):
    ser.write(cmd)


if __name__ == "__main__":
    while True:
        input("enter to run")
        forwardValue = 01.00
        degValue = 010.00

        message = f'f: {forwardValue} d: {degValue} \n'