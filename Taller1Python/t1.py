import cv2
import numpy as np
import matplotlib.pyplot as plt
from skimage import data
from scipy.ndimage.filters import median_filter

image = cv2.imread('1.jpg').astype('uint16')

kernel_sharpening = np.array([[-1,-1,-1], 
                              [-1, 8,-1],
                              [-1,-1,-1]])

sharpened = cv2.filter2D(image, -1, kernel_sharpening)
plt.subplot(121),plt.imshow(image)
plt.subplot(122),plt.imshow(sharpened)

plt.show()


