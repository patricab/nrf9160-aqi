# Matplotlib
# %%
import numpy as np
from matplotlib import pyplot as plt

x = np.arange(1, 11)
y = 3 * x ^5
plt.title("Test")
plt.xlabel("x-axis")
plt.ylabel("y-axis")
plt.plot(x, y, "*b")
plt.show()
print("Test\t", x[1])