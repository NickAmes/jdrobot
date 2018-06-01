#***************************************************************************
# Jane Hyun
# ECE 342
# Color_Test
# Description: Using OpenCV and python, camera is able to filter by color
#              and identify the circular contour of the marker, its center
#              point (centroid), as well as the centroid coordinates. These
#              will hopefully be able to be used to have the robot be able
#              to navigate  its way back to the base station successfully.
#***************************************************************************
import cv2
import numpy as np

def nothing(x):
    pass

#change number to change which camera to use 0 for laptop webcam
cap = cv2.VideoCapture(0)


while True:
    _, frame = cap.read()


    #color range
    lower_pink = np.array([131,119,33])
    upper_pink = np.array([180,255,255])

    kernel = np.ones((5,5),np.uint8)

    blurred = cv2.GaussianBlur(frame, (11, 11), 0)
    
    #converts BGR to HSV color model
    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
    
    mask = cv2.inRange(hsv, lower_pink, upper_pink)
    result = cv2.bitwise_and(frame, frame, mask = mask)

    mask = cv2.dilate(mask, kernel, iterations = 2)

    _, contours, hierarchy = cv2.findContours(mask.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    for contour in contours:
        area = cv2.contourArea(contour)
        if area > 300:
            #moments to calculate centroid x/y coordinates
            ((x,y), radius) = cv2.minEnclosingCircle(contour)
            m = cv2.moments(contour)
            cx = 0
            cy = 0
            cx = int(m['m10']/m['m00'])
            cy = int(m['m01']/m['m00'])
            center = (cx, cy)

            print(cx, cy)
            if radius > 10:
                cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 255), 2)
                #cv2.drawContours(frame, contours, -1, (0, 255, 0), 3)
                
                #display circle and 'center' text in center of marker
                cv2.circle(frame, (cx, cy), 5, (255, 255, 255), -1)
                cv2.putText(frame, "center", (cx - 20, cy - 20), cv2.FONT_HERSHEY_DUPLEX, 0.5, (255, 255,255), 2)

            #print coordinates on video frame
            cv2.putText(frame, "cx: {}, cy: {}".format(cx, cy), (10, frame.shape[0] - 10), cv2.FONT_HERSHEY_DUPLEX, 0.5, (0, 0, 255), 1)
            
    #testing to find center of 640/480 frame size
    #cv2.circle(frame, (int(frame.shape[1]/2), int(frame.shape[0]/2)), 5, (255, 255, 255), -1)


    #if cx < center high range && cx > center low range
    #move forward because it is aligned enough ot base station
        
    #if cx < int(frame.shape[1]/2)
    #turn right
    #move straight x seconds?
    #turn left to see marker again

    #if cx > int(frame.shape[1]/2)
    #turnleft
    #move straight x seconds?
    #turn right to see marker again

    
    #cam-screen windows
    cv2.imshow('frame', frame)
    cv2.imshow('mask', mask)

    #exit program with 'esc' key
    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break

cap.release()
cv2.destroyAllWindows()
