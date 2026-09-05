import numpy as np
import math
import cv2

from contour_processing import color_filter

FOCAL_LENGTH_PX = 1120 / 2
BALL_DIAMETER_M = 0.067

window_name_result = 'Result'
window_name_unfiltered = 'Unfiltered'

cv2.namedWindow(window_name_result)
cv2.namedWindow(window_name_unfiltered)

frame = cv2.imread('test_frames/test_frame.jpg')
assert frame is not None

color_filtered = color_filter(frame)

hough_frame = frame.copy()
hough_input = cv2.GaussianBlur(color_filtered, (9, 9), 2)
circles = cv2.HoughCircles(
    hough_input,
    cv2.HOUGH_GRADIENT,
    dp=1,
    minDist=50,
    param1=100,
    param2=20,
    minRadius=10,
    maxRadius=200
)

if circles is not None:
    circles = np.uint16(np.around(circles))
    for i in circles[0, :]:
        # Draw outer circle
        cv2.circle(hough_frame, (i[0], i[1]), i[2], (0, 255, 0), 2)
        # Draw center
        cv2.circle(hough_frame, (i[0], i[1]), 2, (0, 0, 255), 3)
        print(f'center: ({i[0]}, {i[1]}), radius: {i[2]}')
cv2.imshow(window_name_result, hough_frame)

cv2.imshow(window_name_unfiltered, frame)

def get_hsv_callback(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        print(f'HSV of the clicked pixel: {frame_hsv[y, x]}')

cv2.setMouseCallback(window_name_unfiltered, get_hsv_callback)

while True:
    key = cv2.waitKey(1)
    if key == ord('q'):
        break

cv2.destroyAllWindows()

