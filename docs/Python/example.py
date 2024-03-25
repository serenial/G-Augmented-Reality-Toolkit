import numpy as np
import numpy.typing as npt
import cv2

def say_hello(image: npt.NDArray):

    font = cv2.FONT_HERSHEY_SIMPLEX 
    org = (10, 30)  
    fontScale = 1
    color = (255, 255, 255) 
    thickness = 2

    image = cv2.putText(image, 'Hello from Python', org, font,  
                   fontScale, color, thickness, cv2.LINE_AA) 
    return image