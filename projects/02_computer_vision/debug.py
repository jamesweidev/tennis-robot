import serial
import threading
from uart_send import uart_send, open_uart
import sys

import matplotlib
matplotlib.use('TkAgg')  # or 'GTK3Agg'
import matplotlib.pyplot as plt
from matplotlib.widgets import Button, TextBox

import json
import time
from typing import TypedDict

class MotorData(TypedDict):
    l_rpm: float
    r_rpm: float
    l_ctarget: int
    r_ctarget: int
    l_ftarget: int
    r_ftarget: int
    r_tick_elapsed: int
    l_tick_elapsed: int



def uart_receive(ser):
    t = threading.Thread(target=receive_msg, args=(ser,), daemon=True) 
    t.start()

r_rpms = []
l_rpms = []
r_ctargets = []
l_ctargets = []
def process_data(data: MotorData):
    global r_rpms, l_rpms
    r_rpms.append(data['r_rpm'])
    l_rpms.append(data['l_rpm'])
    r_ctargets.append(data['r_ctarget'])
    l_ctargets.append(data['l_ctarget'])


graph_ready = False
def receive_msg(ser: serial.Serial):
    global graph_ready
    received_data = False
    last_data_t = time.perf_counter()
    while True:
        line = ser.readline()
        if not line: 
            if received_data and (last_data_t + 1 < time.perf_counter()):
                # more than 1 second has passed, the previous command has likely finished
                # signal to main thread that graph needs updating
                graph_ready = True
                received_data = False
                    
            continue

        received_data = True
        last_data_t = time.perf_counter()
        try:
            data: MotorData = json.loads(line.decode())
            process_data(data)
            print(data)
        except:
            print(line.decode())


ser = open_uart()
def main():
    uart_receive(ser)

    timer = fig1.canvas.new_timer(interval=50)
    timer.add_callback(check_graph)
    timer.start()

    plt.show()


def check_graph():
    global graph_ready, r_rpms, l_rpms, r_ctargets, l_ctargets

    if graph_ready:
        update_graph([r_rpms, l_rpms, r_ctargets, l_ctargets])

        graph_ready = False
        r_rpms = []
        l_rpms = []
        r_ctargets = []
        l_ctargets = []


fig1 = plt.figure()

# Main graph
ax1 = fig1.add_axes([0.1, 0.3, 0.8, 0.65])
line1, = ax1.plot([], [], label="Right RPM")
line2, = ax1.plot([], [], label="Left RPM")
ax1.legend()

# Secondary graph
fig2 = plt.figure()
ax2 = fig2.add_axes([0.1, 0.3, 0.8, 0.65])
line3, = ax2.plot([], [], label="Right Target")
line4, = ax2.plot([], [], label="Left Target")
ax2.legend()

# Text boxes
forward_ax = fig1.add_axes([0.15, 0.18, 0.2, 0.06])
degree_ax = fig1.add_axes([0.45, 0.18, 0.2, 0.06])
forward_box = TextBox(forward_ax, "Forward: ")
degree_box = TextBox(degree_ax, "Degrees: ")

# Button
button_ax = fig1.add_axes([0.7, 0.18, 0.15, 0.06])
send_button = Button(button_ax, "Send")

def update_graph(data: list):
    x_vals = [i * 0.1 for i in range(len(data[0]))]

    line1.set_data(x_vals, data[0])
    line2.set_data(x_vals, data[1])

    ax1.relim()
    ax1.autoscale_view()

    line3.set_data(x_vals, data[2])
    line4.set_data(x_vals, data[3])

    ax2.relim()
    ax2.autoscale_view()

    fig1.canvas.draw_idle()
    fig2.canvas.draw_idle()


def send_command(event):
    forward = float(forward_box.text)
    degrees = float(degree_box.text)

    message = f'f: {forward} d: {degrees} \n'
    uart_send(ser, message.encode())

send_button.on_clicked(send_command)

if __name__ == "__main__":
    main()