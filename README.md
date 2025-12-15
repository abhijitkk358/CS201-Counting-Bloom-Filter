# CS201-Bloom-Filter
Bloom Filter
Helper : 
 The idea is to not store the actual key rather store only hash values. It is mainly a probabilistic and space optimized hashing where less than 10 bits per key are required for a 1% false positive probability and is not dependent on the size of individual keys
 We can control the probability of getting a false positive by controlling the size of the Bloom filter. More space means fewer false positives. If we want to decrease probability of false positive result, we have to use more number of hash functions and larger bit array. This would add latency in addition to the item and checking membership. 
 <img width="500" height="127" alt="image" src="https://github.com/user-attachments/assets/4534e91c-5277-4d75-af9d-d94764cf2a62" />
 
<img width="496" height="98" alt="image" src="https://github.com/user-attachments/assets/219e2265-33dd-4c27-8dd2-3101937d29dc" />

Counting Bloom Filter (CS201)
About

This project implements a Counting Bloom Filter in C++ from scratch.
No STL containers are used(Course requirements : used some STL after Final Submission in Main Code).
Hashing is done using MurmurHash3.

A Counting Bloom Filter is a probabilistic data structure used to check whether an element is present or not.
It can give false positives but never false negatives.
Unlike a normal Bloom Filter, this version supports deletion.

What is implemented
Counting Bloom Filter using byte counters
Insert operation
Query operation
Remove operation

False positive rate calculation

Optimal number of hash functions calculation
Runtime analysis showing O(k) behavior
Graph plotting for runtime vs number of hash functions
Key concepts used
Bit / counter based data structures
Double hashing technique
MurmurHash3 (32 bit)
Probabilistic analysis
Time complexity measurement

Files
bloom_filter.cpp
Main implementation and interactive demo
runtime.cpp
Runtime measurement for different values of k
Runtime_data.csv
Collected runtime data
plot_graph.py
Python script to plot runtime graphs

How to compile and run

Compile the C++ code:
g++ bloom_filter.cpp -o bloom
./bloom

For runtime analysis:

g++ runtime.cpp -o runtime
./runtime


To plot the graph:
python plot_graph.py

Operations supported

add
Inserts an element into the filter

query
Checks if an element is probably present or definitely not present

remove
Deletes an element using counter decrement

stats
Shows current filter statistics and false positive rate

False positive rate

The false positive probability is calculated using:

P(fp) = (1 - e^(-k * n / m))^k


where
m = filter size
k = number of hash functions
n = number of inserted elements

Runtime observation
Insertion time increases linearly with k.
This confirms time complexity is O(k).


Applications :

Checking if a username or email is already taken
Web crawlers to avoid visiting the same URL again
Cache systems to quickly check if data may be in cache
Databases to avoid unnecessary disk lookups
Spell checkers to test if a word exists in dictionary
Network systems to detect duplicate packets
Security systems to check blacklisted IPs or URLs


Purpose
This project was done as part of CS201 to understand:
Bloom Filters in depth
Tradeoff between space and accuracy
Real performance behavior of probabilistic data structures



Author
Abhijit Kamble



