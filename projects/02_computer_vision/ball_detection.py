import numpy as np
import math
import cv2
import threading

from uart_send import uart_send, open_uart
from contour_processing import get_contours, filtered_contours

FOCAL_LENGTH_PX = 1120 / 2
BALL_DIAMETER_M = 0.067

window_name_result = 'Result'
window_name_unfiltered = "unfiltered"

cv2.namedWindow(window_name_result)
cv2.namedWindow(window_name_unfiltered)


cap = cv2.VideoCapture(0, cv2.CAP_V4L2)

cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
cap.set(cv2.CAP_PROP_FPS, 30)
cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*"MJPG"))


frame_to_cap = None
frame_to_cap_with_contours = None

ser = open_uart()


current_command = b''
def send_cmd():
    global current_command
    while True:
        input("enter to go to ball")
        if len(current_command) == 0:
            print("command not ready or ball not found")
            continue

        uart_send(ser, current_command)
        print(current_command.decode())

        current_command = b''

t = threading.Thread(target=send_cmd, daemon=True)
t.start()

measurements = []
while True:
    key = cv2.waitKey(1)
    if key == ord('q'):
        break
    elif key == ord('c'):
        cv2.imwrite('test_frames/test_frame.jpg', frame_to_cap)
        cv2.imshow('captured frame', frame_to_cap_with_contours)

    ret, frame = cap.read()
    if not ret: 
        break

    # Capture a noisy frame for debugging
    frame_to_cap = frame.copy()

    contours = get_contours(frame)
    scores = filtered_contours(contours)

    # show the image with all the unfiltered contours drawn
    # for debugging and tuning the filtering
    frame_cpy = frame.copy()
    frame_cpy = cv2.drawContours(frame_cpy, contours, -1, (0,0,255), 3)
    frame_to_cap_with_contours = frame_cpy
    cv2.imshow(window_name_unfiltered, frame_cpy)

    # if at least one ball exists on the screen
    if len(scores) > 0:
        # if multiple balls, select the biggest one
        best_contour_i, best_score, best_radius, best_x, best_y = max(scores, key= lambda x : x[2])

        ball_diam_px = best_radius * 2

        # distance to the ball
        distance = BALL_DIAMETER_M * FOCAL_LENGTH_PX / ball_diam_px

        # Get the ball's angle offset relative to the center of the camera
        x_offset_px = best_x - cap.get(cv2.CAP_PROP_FRAME_WIDTH) / 2
        rad_from_center = math.atan(x_offset_px / FOCAL_LENGTH_PX)
        deg_from_center = math.degrees(rad_from_center)

        measurements.append((distance, deg_from_center))
        
        cv2.circle(frame, (best_x, best_y), int(best_radius), (0, 0, 255), 2, cv2.LINE_AA)

        # 5 consecutive ball measurements must have been captured
        # otherwise it might not be stable
        if len(measurements) >= 5:
            measurements.sort(key=(lambda x: x[0]))

            median_measurement = measurements[2]
            distance = median_measurement[0] + 0.1
            degs = median_measurement[1] * 0.9

            # uart_send(ser, f'f: {distance:.2f} d: {degs:.2f} \n'.encode("utf-8"))
            # print(f'Distance: {distance} m deg: {degs}')
            current_command = f'f: {distance:.2f} d: {degs:.2f} \n'.encode("utf-8")

            measurements = []
    else:
        # A frame without the ball was detected, reset measurements
        measurements = []

    cv2.imshow(window_name_result, frame)
    

ser.close()
cap.release()
cv2.destroyAllWindows()

