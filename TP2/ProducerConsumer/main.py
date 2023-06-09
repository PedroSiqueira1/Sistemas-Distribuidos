import subprocess
import matplotlib.pyplot as plt
import sys

# Path to the compiled file
file_path = "/home/a239277/Documents/Projetos/Trabalho/TP2/ProducerConsumer/semap"

# Command-line arguments for the file
arguments = sys.argv[1:]  # Exclude the first argument, which is the script name

# Dict to store the buffer_tracker
buffer_tracker = {}


run_iterations = 0  
# function to read buffer_tracker file
def read_buffer_tracker(buffer_tracker):
    global run_iterations
    buffer = []
    with open("buffer_tracker.txt", "r") as f:
        for line in f:
            line = line.strip()
            if line:
                buffer.append(float(line))

    buffer_tracker[run_iterations] = buffer
    run_iterations += 1          


# Execute the compiled file
for _ in range(10):
    try:
        subprocess.run([file_path] + arguments)
        read_buffer_tracker(buffer_tracker)
    except FileNotFoundError:
        print("File not found.")
    except subprocess.CalledProcessError as e:
        print("Error executing the file:")
        print(e)

# Get mean time from each iteration
total_time = 0
for key, value in buffer_tracker.items():
    total_time += buffer_tracker[key].pop()

# Function to plot the buffer_tracker
def plot_buffer_tracker(buffer_tracker):
    plt.plot(buffer_tracker[0])
    plt.xlabel("Produced/Consumed Buffers")
    plt.ylabel("Full Buffers")
    plt.show()

# Plot the mean time
print("Mean time: {}".format(total_time/10))

# Plot the buffer_tracker
plot_buffer_tracker(buffer_tracker)