import matplotlib.pyplot as plt
import numpy as np
import csv
import sys
import os

# Initialize lists to store data
k_vals = []
runtime_vals = []
time_per_op_vals = []

# Smart file detection
if os.path.exists('output/Runtime_data.csv'):
    csv_filename = 'output/Runtime_data.csv'
elif os.path.exists('Runtime_data.csv'):
    csv_filename = 'Runtime_data.csv'
else:
    print("No file")
    sys.exit(1)

# Read data from the CSV file
with open(csv_filename, 'r') as file:
    reader = csv.reader(file)
    next(reader)  # Skip the header row
    
    for row in reader:
        if len(row) == 3:  # Make sure we don't read empty lines
            k_vals.append(float(row[0]))
            runtime_vals.append(float(row[1]))
            time_per_op_vals.append(float(row[2]))

# Convert lists to NumPy arrays for plotting
k = np.array(k_vals)
runtime_ms = np.array(runtime_vals)
time_per_op = np.array(time_per_op_vals)

# Create figure
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

# Graph 1: Total Runtime
ax1.plot(k, runtime_ms, 'o-', linewidth=2.5, markersize=8, color='#0066cc')
ax1.fill_between(k, runtime_ms, alpha=0.2, color='#0066cc')
ax1.set_xlabel('k - Number of Hash Functions', fontsize=12, fontweight='bold')
ax1.set_ylabel('Total Runtime (ms)', fontsize=12, fontweight='bold')
ax1.set_title('Bloom Filter Insert Runtime vs k', fontsize=13, fontweight='bold')
ax1.grid(True, alpha=0.3, linestyle='--')
ax1.set_xticks(range(1, int(max(k)) + 1)) 

# Graph 2: Time per Operation
ax2.plot(k, time_per_op, 's-', linewidth=2.5, markersize=8, color='#cc0000')
ax2.fill_between(k, time_per_op, alpha=0.2, color='#cc0000')
ax2.set_xlabel('k - Number of Hash Functions', fontsize=12, fontweight='bold')
ax2.set_ylabel('Time per Operation (µs)', fontsize=12, fontweight='bold')
ax2.set_title('Time per Operation vs k (Proves O(k))', fontsize=13, fontweight='bold')
ax2.grid(True, alpha=0.3, linestyle='--')
ax2.set_xticks(range(1, int(max(k)) + 1))

plt.tight_layout()
plt.savefig('bloom_filter_runtime.png', dpi=300, bbox_inches='tight')
print("Graph saved as 'bloom_filter_runtime.png'")
plt.show()
