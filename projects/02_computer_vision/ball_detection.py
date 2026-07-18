import numpy as np
import math
import cv2

FOCAL_LENGTH_PX = 1120
BALL_DIAMETER_M = 0.067

low_H = 27
low_S = 30
high_H = 55
high_S = 100
window_name_result = 'Result'

window_name_unfiltered = "unfiltered"

cv2.namedWindow(window_name_result)
cv2.namedWindow(window_name_unfiltered)



cap = cv2.VideoCapture(0)


while True:
    key = cv2.waitKey(1)
    if key == ord('q'):
        break

    ret, frame = cap.read()
    if not ret: 
        break
    frame = cv2.resize(frame, (640, 360))

    blurred = cv2.GaussianBlur(frame, (11,11), 0)

    lower_bound = np.array([low_H, int(255 * low_S / 100), 50])
    upper_bound = np.array([high_H, int(255 * high_S / 100), 255])

    frame_hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
    color_filter = cv2.inRange(frame_hsv, lower_bound, upper_bound)

    kernel = np.ones(shape=(7, 7), dtype=np.uint8)
    opened_filter = cv2.morphologyEx(color_filter, cv2.MORPH_OPEN, kernel)
    final_filter = cv2.morphologyEx(opened_filter, cv2.MORPH_CLOSE, kernel)

    contours, _ = cv2.findContours(final_filter, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    scores = []
    # filter the contours for the best one
    for i, contour in enumerate(contours):
        area = cv2.contourArea(contour)

        (x, y), radius = cv2.minEnclosingCircle(contour)

        # Area if the contour is a perfect circle
        perfect_area = math.pi * (radius ** 2)

        # score contours based on how much they resemble a perfect circle
        score = area / perfect_area
        scores.append((i, score, radius))
    
    print(scores)
    # Only allow contours that at least 70% resembles a circle
    scores = [score for score in scores if score[1] > 0.7]

    frame_cpy = frame.copy()
    frame_cpy = cv2.drawContours(frame_cpy, contours, -1, (0,0,255), 3)
    cv2.imshow(window_name_unfiltered, frame_cpy)

    if len(scores) > 0:
        best_contour_i, best_score, best_radius = max(scores, key= lambda x : x[1])

        ball_diam_px = best_radius * 2

        distance = BALL_DIAMETER_M * FOCAL_LENGTH_PX / ball_diam_px
        print(f'Distance to the ball is: {distance} meters')

        frame = cv2.drawContours(frame, contours, best_contour_i, (0,0,255), 3)

    cv2.imshow(window_name_result, frame)

cap.release()
cv2.destroyAllWindows()

