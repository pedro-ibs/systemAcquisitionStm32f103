#!/bin/python3.8

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
 
df = pd.read_csv("./1khz.csv")

print (df.head())
print (df.info())

df = df.cumsum()
plt.figure()
df.plot()
