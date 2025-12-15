// Counting Bloom Filter Implementation - No STL
// Uses MurmurHash3 for fast, uniform hashing
// Configurable m (array size) and k (hash functions)


#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <chrono>  // ADDED: For runtime measurement
using namespace std;
typedef uint32_t unsign32;
typedef uint8_t unsign8;




// no STL
template<typename T>
T* allocate_array(unsign32 size) {
    T* arr = new T[size];
    for (unsign32 i = 0; i < size; i++) {
        arr[i] = 0;
    }
    return arr;
}

//memory deallocator
template<typename T>
void deallocate_array(T* arr) {
    delete[] arr;
}


// this gives string length without string header
unsign32 str_length(const char* str) {
    unsign32 len = 0;
    while (str[len] != '\0') len++;
    return len;
}


//MURMURHASH3 IMPLEMENTATION 


inline unsign32 rotl32(unsign32 x, int8_t r) {
    unsign32 s = static_cast<unsign32>(r) & 31U;
    return (x << s) | (x >> (32u - s));
}


inline unsign32 avalanche32(unsign32 h) {
    h ^= h >> 16;
    h *= 0x85ebca6b; // random nums 
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}


//32-bit implementation
unsign32 murmur3_hash(const void* key, unsign32 len, unsign32 seed) {
    const unsign8* data = (const unsign8*)key;
    const unsign32 nblocks = len / 4;
    
    unsign32 h1 = seed;
    const unsign32 c1 = 0xcc9e2d51;
    const unsign32 c2 = 0x1b873593;

    //Head of data
    // process 4 bytes at a time
    const unsign32* blocks = (const unsign32*)(data);


    for (unsign32 i = 0; i < nblocks; i++) {
        unsign32 k1 = blocks[i];
        
        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;
        
        h1 ^= k1;
        h1 = rotl32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }


    // Tail - process remaining bytes
    const unsign8* tail = (const unsign8*)(data + nblocks * 4);
    unsign32 k1 = 0;


    switch (len & 3) {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
                k1 *= c1;
                k1 = rotl32(k1, 15);
                k1 *= c2;
                h1 ^= k1;
    }


    // Finalization
    h1 ^= len;
    h1 = avalanche32(h1);
    
    return h1;
}


//CBF


class CountingBloomFilter {
private:
    unsign8* counters;     // Array of counters (not bits)
    unsign32 m;            // Size of counter array
    unsign32 k;            // Number of hash functions
    unsign32 num_items;    


    // Generates k different hash values using double hashing
    // h_i(x) = (hash1(x) + i * hash2(x)) mod m
    unsign32 get_hash(const char* item, unsign32 hash_num) {
        unsign32 len = str_length(item);
        unsign32 hash1 = murmur3_hash(item, len, 0);
        unsign32 hash2 = murmur3_hash(item, len, hash1);
        
        // Double hashing technique to generate k independent hashes
        return (hash1 + hash_num * hash2) % m;
    }


public:
    // Constructor: initialize with m (array size) and k (hash functions)
    CountingBloomFilter(unsign32 array_size, unsign32 num_hashes) {
        m = array_size;
        k = num_hashes; // number of hashes 
        num_items = 0;
        counters = allocate_array<unsign8>(m);
    }


    // Destructor
    ~CountingBloomFilter() {
        deallocate_array(counters);
    }


  
    void insert(const char* item) {
           cout << "\n[INSERT] Item: '" << item << "'\n";
        
        for (unsign32 i = 0; i < k; i++) {
            unsign32 index = get_hash(item, i);
               cout << "  Hash " << (i+1) << ": position " << index 
                      << " (counter: " << (int)counters[index] 
                      << " -> " << (int)(counters[index]+1) << ")\n";
            
            if (counters[index] < 255) {  // To Prevent overflow
                counters[index]++;
            }
        }
        num_items++;
        
        // Show current parameters and false positive rate
           cout << "  Current state: n=" << num_items 
                  << ", m=" << m << ", k=" << k 
                  << ", False positive rate =" << (get_false_positive_rate() * 100.0) 
                  << "%\n";
    }


    void insert_silent(const char* item) {
        for (unsign32 i = 0; i < k; i++) {
            unsign32 index = get_hash(item, i);
            if (counters[index] < 255) {
                counters[index]++;
            }
        }
        num_items++;
    }


    // Query if item exists (check all counters > 0)
    bool query(const char* item) {
        return query_verbose(item, false);
    }


    // Remove an item (decrement counters by 1 each)
    // This is the key advantage of counting Bloom filters it can perform deletion
    void remove(const char* item) {
           cout << "\n[REMOVE] Item: '" << item << "'\n";
        
        // First check if item might be in the filter
        if (!query(item)) {
               cout << "  Item not found in filter (all counters must be > 0)\n";
            return;  // Item definitely not in filter
        }


        for (unsign32 i = 0; i < k; i++) {
            unsign32 index = get_hash(item, i);
               cout << "  Hash " << (i+1) << ": position " << index 
                      << " (counter: " << (int)counters[index] 
                      << " -> " << (int)(counters[index]-1) << ")\n";
            
            if (counters[index] > 0) {
                counters[index]--;
            }
        }
        if (num_items > 0) {
            num_items--;
        }
        
        // Show current parameters and false positive rate
           cout << "  Current state: n=" << num_items 
                  << ", m=" << m << ", k=" << k 
                  << ", P(fp)=" << (get_false_positive_rate() * 100.0) 
                  << " %\n";
    }


    
    
    // Query with optional details:: output
    bool query_verbose(const char* item, bool verbose = false) {
        if (verbose) {
               cout << "\n[QUERY] Item: '" << item << "'\n";
        }
        
        for (unsign32 i = 0; i < k; i++) {
            unsign32 index = get_hash(item, i);
            
            if (verbose) {
                   cout << "  Hash " << (i+1) << ": position " << index 
                          << " (counter = " << (int)counters[index] << ")" << '\n';
            }
            
            if (counters[index] == 0) {
                if (verbose) {
                       cout << "  Result: NOT FOUND (definitely not in filter)\n";
                }
                return false;  // Item Definitely not in filter
            }
            if (verbose) {
                cout << "\n";
            }
        }
        
        if (verbose) {
               cout << "  Result: FOUND (probably in filter)\n";
               cout << "  Current state: n=" << num_items 
                      << ", m=" << m << ", k=" << k 
                      << ", P(fp)=" << (get_false_positive_rate() * 100.0) 
                      << "%\n";
        }
        
        return true;  // Probably in filter
    }


    // Calculate false positive probability
    // P(fp) ≈ (1 - e^(-k*n/m))^k
    double get_false_positive_rate() {
        if (num_items == 0) return 0.0;
        
        // Formula: FPR = (1 - e^(-k*n/m))^k
        double exponent = -(double)k * (double)num_items / (double)m;
        double prob = 1.0 - exp(exponent);
        double result = pow(prob, (double)k);
        
        return result;
    }


    // Calculate optimal k for given m and expected n
    // k_optimal = (m/n) * ln(2)
    static unsign32 calculate_optimal_k(unsign32 m, unsign32 expected_n) {
        if (expected_n == 0) return 1;
        
        // ln(2) ≈ 0.693147
        double k_optimal = ((double)m / (double)expected_n) * 0.6931;
        
        unsign32 k = (unsign32)(k_optimal + 0.5);  // Round to nearest int
        return (k < 1) ? 1 : k;
    }


    // Print statistics with formulas
    void print_stats() {
       
           cout << "    BLOOM FILTER STATISTICS        \n";
        
           cout << "PARAMETERS:\n";
           cout << "  m (array size)      = " << m << "\n";
           cout << "  k (hash functions)  = " << k << "\n";
           cout << "  n (items inserted)  = " << num_items << "\n\n";
        
        double load_factor = (double)num_items / (double)m;
        double fp_rate = get_false_positive_rate();
        
           cout << " CALCULATED VALUES:\n";
           cout << "Load factor (n/m)   = " << load_factor << "\n";
           cout << "False positive rate = " << (fp_rate * 100.0) << " %\n\n";
        
        double exponent = -(double)k * (double)num_items / (double)m;
       
           cout << "False positive rate = " << (fp_rate * 100.0) << " %\n\n";
        
           cout << "OPTIMAL k :\n";
        
        unsign32 optimal_k = calculate_optimal_k((unsign32)m, (unsign32)num_items);
           cout << optimal_k << '\n';
    }
};


// Runtime measurement function
void measure_runtime_vs_k() {
       cout << "\n========================================\n";
       cout << "    RUNTIME vs K MEASUREMENT\n";
       cout << "========================================\n\n";
    
    unsign32 m = 100000;          // Fixed filter size
    unsign32 num_inserts = 50000; // Number of operations
    
       cout << "Configuration:\n";
       cout << "  Filter size (m): " << m << "\n";
       cout << "  Number of inserts: " << num_inserts << "\n";
       cout << "  Testing k from 1 to 15\n\n";
    
       cout << "Results (CSV format):\n";
       cout << "k,runtime_ms,time_per_op_us\n";
       cout << "-----------------------------------\n";
    
    for (unsign32 k = 1; k <= 15; k++) {
        CountingBloomFilter filter(m, k);
        
        // Start timing
        auto start =    chrono::high_resolution_clock::now();
        
        // Perform inserts (silent, no output)
        for (unsign32 i = 0; i < num_inserts; i++) {
            char item[32];
            snprintf(item, sizeof(item), "item_%u", i);
            filter.insert_silent(item);
        }
        
        // End timing
        auto end =    chrono::high_resolution_clock::now();
        
        // Calculate duration
        double runtime_ms =    chrono::duration<double,    milli>(end - start).count();
        double time_per_op_us = (runtime_ms * 1000.0) / num_inserts;
        
        // Output results
           cout << k << "," << runtime_ms << "," << time_per_op_us << "\n";
    }
    
       cout << "\n========================================\n";
       cout << "Analysis:\n";
       cout << "  If runtime grows linearly with k,\n";
       cout << "  then complexity is O(k) ✓\n";
       cout << "========================================\n\n";
}





int main() {
       cout << "Counting Bloom Filter \n";

    // Run runtime measurement
    measure_runtime_vs_k();

       cout << "\n\n========================================\n";
       cout << "    REGULAR DEMONSTRATION\n";
       cout << "========================================\n\n";

    // Configurable parameters
    unsign32 m = 1000;        // Size of counter array
    unsign32 expected_n = 100; // Expected number of elements
    
    // Calculate optimal k
    unsign32 k = CountingBloomFilter::calculate_optimal_k(m, expected_n);
       cout << "Optimal k for m = " << m << ", n=" << expected_n 
              << ": " << k << "\n\n";


    // Create filter - YOU CAN CHANGE m AND k HERE
    CountingBloomFilter filter(m, k);


    // Insert items
       cout << "Inserting items...\n";
    filter.insert("apple");
    filter.insert("banana");
    filter.insert("cherry");
    filter.insert("date");
    filter.insert("elderberry");


   


       cout << "QUERYING ITEMS\n";


    
    filter.query_verbose("apple");
    filter.query_verbose("banana");
    filter.query_verbose("grape");
    filter.query_verbose("watermelon");


    // Remove an item (counting bloom filter feature!)


       cout << "TESTING REMOVAL\n";
 
    
    filter.remove("banana");
    filter.query_verbose("banana");


    
    filter.print_stats();


       cout << "\nTesting with different parameters \n";

    // Change m and k 
    CountingBloomFilter small_filter(100, 3); 
    for (int i = 0; i < 10; i++) {
        char item[20];
        for (int j = 0; j < 5; j++) {
            item[j] = 'a' + (i + j) % 26;
        }
        item[5] = '\0';
        small_filter.insert(item);
    }
    small_filter.print_stats();


    return 0;
}

