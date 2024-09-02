import matplotlib.pyplot as plt
import numpy as np
import serial
import json
from matplotlib.animation import FuncAnimation

# Set up the figure and 3D axis
fig = plt.figure()
ax = fig.add_subplot(projection='3d')

# Set initial axis limits and labels
ax.set_xlim(0, 10000)
ax.set_ylim(0, 10000)
ax.set_zlim(0, 10000)
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# Set the initial view
ax.view_init(elev=20., azim=-35, roll=0)

# Initialize an empty list to store the scatter plot
scat = ax.scatter([], [], [])


# Function to initialize the scatter plot
def init():
    scat._offsets3d = ([], [], [])
    return scat,


# Function to update the scatter plot
def update(frame):
    line = ser.readline()
    if line:
        try:
            j = json.loads(line)
            x = float(j['x'])
            y = float(j['y'])
            z = float(j['z'])
            current_offsets = np.array(scat._offsets3d).T
            new_offsets = np.vstack([current_offsets, [x, y, z]])
            scat._offsets3d = (new_offsets[:, 0], new_offsets[:, 1], new_offsets[:, 2])

            # Automatically adjust axis limits
            ax.set_xlim(min(new_offsets[:, 0]), max(new_offsets[:, 0]))
            ax.set_ylim(min(new_offsets[:, 1]), max(new_offsets[:, 1]))
            ax.set_zlim(min(new_offsets[:, 2]), max(new_offsets[:, 2]))

        except (json.JSONDecodeError, KeyError, ValueError) as e:
            print(f"Error decoding JSON: {e}")
    return scat,


# Open the serial port
ser = serial.Serial('COM7', 115200, timeout=1)

# Create the animation
ani = FuncAnimation(fig, update, init_func=init, blit=False, interval=100)

# Show the plot
plt.show()
