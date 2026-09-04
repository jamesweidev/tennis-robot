import cv2

cap = cv2.VideoCapture(0, cv2.CAP_V4L2)

cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
cap.set(cv2.CAP_PROP_FPS, 30)
cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*"MJPG"))

winname = 'window'

cv2.namedWindow(winname)

while True:
    key = cv2.waitKey(1)
    if key == ord('q'):
        break

    ret, frame = cap.read()
    assert frame is not None

    cv2.imshow(winname, frame)


cv2.destroyAllWindows()