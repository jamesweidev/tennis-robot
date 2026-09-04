import cv2
import numpy as np
import math

low_H = 25
low_S = 60
high_H = 90
high_S = 255

def get_contours(frame):
    # process the image then return all external contours
    blurred = cv2.GaussianBlur(frame, (3,3), 0)

    lower_bound = np.array([low_H, low_S, 80])
    upper_bound = np.array([high_H, high_S, 255])

    frame_hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
    color_filter = cv2.inRange(frame_hsv, lower_bound, upper_bound)

    kernel = np.ones(shape=(5, 5), dtype=np.uint8)
    opened_filter = cv2.morphologyEx(color_filter, cv2.MORPH_OPEN, kernel)
    final_filter = cv2.morphologyEx(opened_filter, cv2.MORPH_CLOSE, kernel)

    contours, _ = cv2.findContours(final_filter, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    return contours

def filtered_contours(contours) -> list:
    # filter contours based on how much they resemble a perfect circle
    scores = []
    for i, contour in enumerate(contours):
        area = cv2.contourArea(contour)

        (x, y), radius = cv2.minEnclosingCircle(contour)

        # Area if the contour is a perfect circle
        perfect_area = math.pi * (radius ** 2)

        # score contours based on how much they resemble a perfect circle
        score = area / perfect_area
        scores.append((i, score, radius, int(x), int(y)))

    # Only allow contours that at least 70% resembles a circle
    scores = [score for score in scores if score[1] > 0.75]

    return scores
