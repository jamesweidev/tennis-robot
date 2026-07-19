import debug_server
import time

while True:
    time.sleep(2)
    debug_server.send_debug("Sending test message")