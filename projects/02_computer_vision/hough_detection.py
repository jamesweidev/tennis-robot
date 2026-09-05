import numpy as np
import math
import cv2

from contour_processing import get_contours, filtered_contours, get_hough_circles

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


def get_ball_location(rad, x):
    ball_diam_px = rad * 2

    # distance to the ball
    distance = BALL_DIAMETER_M * FOCAL_LENGTH_PX / ball_diam_px

    # Get the ball's angle offset relative to the center of the camera
    x_offset_px = x - cap.get(cv2.CAP_PROP_FRAME_WIDTH) / 2
    rad_from_center = math.atan(x_offset_px / FOCAL_LENGTH_PX)
    deg_from_center = math.degrees(rad_from_center)

    return (distance, deg_from_center)

# current_command = f'f: {distance:.2f} d: {degs:.2f} \n'.encode("utf-8")
measurements = []
while True:
    key = cv2.waitKey(1)
    if key == ord('q'):
        break
    elif key == ord('c'):
        assert frame_to_cap is not None and frame_to_cap_with_contours is not None
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

    hough_frame = frame.copy()
    circles = get_hough_circles(frame)
    if circles is not None:
        circles = np.uint16(np.around(circles))
        for i in circles[0, :]:
            # Draw outer circle
            cv2.circle(hough_frame, (i[0], i[1]), i[2], (0, 255, 0), 2)
            # Draw center
            cv2.circle(hough_frame, (i[0], i[1]), 2, (0, 0, 255), 3)
            print(f'center: ({i[0]}, {i[1]}), radius: {i[2]}')

    cv2.imshow(window_name_result, frame)
    

cap.release()
cv2.destroyAllWindows()

