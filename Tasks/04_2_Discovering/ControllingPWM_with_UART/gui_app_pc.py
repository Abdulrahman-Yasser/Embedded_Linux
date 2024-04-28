#!/usr/bin/python3.8
from tkinter import *
from PIL import ImageTk, Image

root = Tk()
root.title('Control your PWM')

vertical = Scale(root, from_=0, to=200)
vertical.pack()

def slide():
    my_label = Label(root, text=horizontal.get()).pack()
    root.geometry(str(horizontal.get()) + "x400")
    

horizontal = Scale(root, from_=0, to=400, orient=HORIZONTAL, command=slide)
horizontal.pack()

my_label = Label(root, text=horizontal.get()).pack()

my_ptn = Button(root, text="Send", command=slide).pack()

root.mainloop()
