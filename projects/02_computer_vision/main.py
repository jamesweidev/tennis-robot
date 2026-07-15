import numpy as np
import matplotlib.pyplot as plt
from PIL import Image


pic = Image.open('Computer-Vision-with-Python/DATA/00-puppy.jpg')

pic_arr = np.asarray(pic)

print(type(pic_arr))
print(pic_arr.shape)


pic_red = pic_arr.copy()
pic_red = pic_red[:, :, 0]

plt.imshow(pic_red)
plt.show()
