import matplotlib.pyplot as plt
import numpy as np

# Your actual data from running the C++ program
data_text = """k,runtime_ms,time_per_op_us
1,18.1302,0.362604
2,34.8234,0.696468
3,38.3468,0.766936
4,47.4446,0.948892
5,75.9821,1.51964
6,76.0727,1.52145
7,84.4219,1.68844
8,82.3492,1.64698
9,86.4555,1.72911
10,101.721,2.03442
11,110.668,2.21336
12,136.783,2.73565
13,181.755,3.6351
14,219.597,4.39194
15,171.703,3.43407"""

# Parse data
lines = data_text.strip().split('\n')[1:]  # Skip header
k_vals = []
runtime_vals = []
time_per_op_vals = []

for line in lines:
    parts = line.split(',')
    k_vals.append(float(parts[0]))
    runtime_vals.append(float(parts[1]))
    time_per_op_vals.append(float(parts[2]))

# Convert to numpy arrays
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
ax1.set_xticks(range(1, 16))

# Graph 2: Time per Operation
ax2.plot(k, time_per_op, 's-', linewidth=2.5, markersize=8, color='#cc0000')
ax2.fill_between(k, time_per_op, alpha=0.2, color='#cc0000')
ax2.set_xlabel('k - Number of Hash Functions', fontsize=12, fontweight='bold')
ax2.set_ylabel('Time per Operation (µs)', fontsize=12, fontweight='bold')
ax2.set_title('Time per Operation vs k (Proves O(k))', fontsize=13, fontweight='bold')
ax2.grid(True, alpha=0.3, linestyle='--')
ax2.set_xticks(range(1, 16))

plt.tight_layout()
plt.savefig('bloom_filter_runtime.png', dpi=300, bbox_inches='tight')
print("✓ Graph saved as 'bloom_filter_runtime.png'")
plt.show()
