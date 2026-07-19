import serial

def uart_send(message: bytes) -> None:

    with serial.Serial("/dev/ttyS0", 115200) as ser:
        ser.write(message)

