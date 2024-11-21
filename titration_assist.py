
import cv2
import numpy as np
import matplotlib as plt


import matplotlib.pyplot as plt

def on_click(event):
    if event.inaxes is not None:
        print(event.xdata, event.ydata)
    else:
        print('Clicked ouside axes bounds but inside plot window')

fig, ax = plt.subplots()
fig.canvas.callbacks.connect('button_press_event', on_click)
