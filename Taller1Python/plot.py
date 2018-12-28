import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import cv2
from skimage import data
from scipy.ndimage.filters import median_filter

image = cv2.imread('1.jpg', 0)
# create the x and y coordinate arrays (here we just use pixel indices)
xx, yy = np.mgrid[0:image.shape[0], 0:image.shape[1]]

# create the figure
fig = plt.figure()
ax = fig.gca(projection='3d')
ax.plot_surface(xx, yy, image ,rstride=1, cstride=1, cmap=plt.cm.gray,
                linewidth=0)

# show it
plt.show()
