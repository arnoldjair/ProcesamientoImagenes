import numpy as np
import cv2
from matplotlib import pyplot as plt

image = cv2.imread('3.jpg')

dst = cv2.fastNlMeansDenoising(image, None,15, 21, 1)

plt.subplot(121),plt.imshow(image,'gray')
plt.subplot(122),plt.imshow(dst,'gray')

plt.show()
