import socket
import threading

HOST = ''
PORT = 50007

debug_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
debug_socket.bind((HOST, PORT))
debug_socket.listen()

conn = None
addr = None

conn_lock = threading.Lock()

def send_debug(msg: str) -> None:
    global conn, addr

    if not conn:
        return

    try:
        conn.sendall((msg + "\n").encode())
    except BrokenPipeError as e:
        print(f"Debug client disconnected: {e}")

        conn.close()
        conn = None
        addr = None


def accept_client() -> None:
    global conn, addr

    while True:
        new_conn, new_addr = debug_socket.accept()

        print(f'new client connected from: {new_addr}')

        with conn_lock:
            conn = new_conn
            addr = new_addr


accept_thread = threading.Thread(
    target=accept_client,
    daemon=True
)

accept_thread.start()



s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect(("8.8.8.8", 80))
print(f'Debug server running at {s.getsockname()[0]}:{PORT} ')
s.close()
