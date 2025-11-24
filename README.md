# CS201-Bloom-Filter
Bloom Filter
PPT Helper : 
 The idea is to not store the actual key rather store only hash values. It is mainly a probabilistic and space optimized hashing where less than 10 bits per key are required for a 1% false positive probability and is not dependent on the size of individual keys
 We can control the probability of getting a false positive by controlling the size of the Bloom filter. More space means fewer false positives. If we want to decrease probability of false positive result, we have to use more number of hash functions and larger bit array. This would add latency in addition to the item and checking membership. 
 <img width="500" height="127" alt="image" src="https://github.com/user-attachments/assets/4534e91c-5277-4d75-af9d-d94764cf2a62" />
 
<img width="496" height="98" alt="image" src="https://github.com/user-attachments/assets/219e2265-33dd-4c27-8dd2-3101937d29dc" />
Links : 
https://medium.com/@humberto521336/bloom-filters-basics-c54ed06f8f73

<img width="604" height="192" alt="image" src="https://github.com/user-attachments/assets/10c913fe-3ff9-4f12-93f1-19502ba93beb" />
