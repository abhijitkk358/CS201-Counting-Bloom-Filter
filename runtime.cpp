#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <fstream>

using namespace std;
typedef uint32_t unsign32;
typedef uint8_t unsign8;

template<typename T>
T* allocate_array(unsign32 size) {
    T* arr = new T[size];
    for (unsign32 i = 0; i < size; i++) {
        arr[i] = 0;
    }
    return arr;
}

template<typename T>
void deallocate_array(T* arr) {
    delete[] arr;
}

unsign32 str_length(const char* str) {
    unsign32 len = 0;
    while (str[len] != '\0') len++;
    return len;
}

inline unsign32 rotl32(unsign32 x, int8_t r) {
    unsign32 s = static_cast<unsign32>(r) & 31U;
    return (x << s) | (x >> (32u - s));
}

inline unsign32 avalanche32(unsign32 h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

unsign32 murmur3_hash(const void* key, unsign32 len, unsign32 seed) {
    const unsign8* data = (const unsign8*)key;
    const unsign32 nblocks = len / 4;
    
    unsign32 h1 = seed;
    const unsign32 c1 = 0xcc9e2d51;
    const unsign32 c2 = 0x1b873593;
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

    h1 ^= len;
    h1 = avalanche32(h1);
    
    return h1;
}

class CountingBloomFilter {
private:
    unsign8* counters;
    unsign32 m;
    unsign32 k;
    unsign32 num_items;

    unsign32 get_hash(const char* item, unsign32 hash_num) {
        unsign32 len = str_length(item);
        unsign32 hash1 = murmur3_hash(item, len, 0);
        unsign32 hash2 = murmur3_hash(item, len, hash1);
        return (hash1 + hash_num * hash2) % m;
    }

public:
    CountingBloomFilter(unsign32 array_size, unsign32 num_hashes) {
        m = array_size;
        k = num_hashes;
        num_items = 0;
        counters = allocate_array<unsign8>(m);
    }

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
            
            if (counters[index] < 255) {
                counters[index]++;
            }
        }
        num_items++;
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

    bool query(const char* item) {
        return query_verbose(item, false);
    }

    void remove(const char* item) {
        cout << "\n[REMOVE] Item: '" << item << "'\n";
        if (!query(item)) {
            cout << "  Item not found in filter (all counters must be > 0)\n";
            return;
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
        cout << "  Current state: n=" << num_items 
             << ", m=" << m << ", k=" << k 
             << ", P(fp)=" << (get_false_positive_rate() * 100.0) 
             << " %\n";
    }

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
                return false;
            }
            if (verbose) cout << "\n";
        }
        
        if (verbose) {
            cout << "  Result: FOUND (probably in filter)\n";
            cout << "  Current state: n=" << num_items 
                 << ", m=" << m << ", k=" << k 
                 << ", P(fp)=" << (get_false_positive_rate() * 100.0) 
                 << "%\n";
        }
        return true;
    }

    double get_false_positive_rate() {
        if (num_items == 0) return 0.0;
        double exponent = -(double)k * (double)num_items / (double)m;
        double prob = 1.0 - exp(exponent);
        return pow(prob, (double)k);
    }

    static unsign32 calculate_optimal_k(unsign32 m, unsign32 expected_n) {
        if (expected_n == 0) return 1;
        double k_optimal = ((double)m / (double)expected_n) * 0.6931;
        unsign32 k = (unsign32)(k_optimal + 0.5);
        return (k < 1) ? 1 : k;
    }

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
        cout << "OPTIMAL k :\n";
        cout << calculate_optimal_k((unsign32)m, (unsign32)num_items) << '\n';
    }
};

void measure_runtime_vs_k() {
    unsign32 m = 100000;          
    unsign32 num_inserts = 50000; 
    
    ofstream csv_file("Runtime_data.csv");
    
    if (!csv_file.is_open()) {
        cerr << "Failed to open Runtime_data.csv for writing.\n";
        return;
    }

    csv_file << "k,runtime_ms,time_per_op_us\n";

    for (unsign32 k = 1; k <= 15; k++) {
        CountingBloomFilter filter(m, k);
        auto start = chrono::high_resolution_clock::now();
        
        for (unsign32 i = 0; i < num_inserts; i++) {
            char item[32];
            snprintf(item, sizeof(item), "item_%u", i);
            filter.insert_silent(item);
        }
        
        auto end = chrono::high_resolution_clock::now();
        double runtime_ms = chrono::duration<double, milli>(end - start).count();
        double time_per_op_us = (runtime_ms * 1000.0) / num_inserts;
        
        csv_file << k << "," << runtime_ms << "," << time_per_op_us << "\n";
        cout << "Completed benchmark for k=" << k << "\n";
    }
    
    csv_file.close();
    cout << "\n✓ Benchmark complete. Data saved to Runtime_data.csv\n";
}

int main() {
    cout << "Counting Bloom Filter \n";

    measure_runtime_vs_k();

    cout << "\n\n========================================\n";
    cout << "    REGULAR DEMONSTRATION\n";
    cout << "========================================\n\n";

    unsign32 m = 1000;
    unsign32 expected_n = 1000000;
    unsign32 k = CountingBloomFilter::calculate_optimal_k(m, expected_n);
    
    cout << "Optimal k for m = " << m << ", n=" << expected_n << ": " << k << "\n\n";

    CountingBloomFilter filter(m, k);

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

    cout << "TESTING REMOVAL\n";
    filter.remove("banana");
    filter.query_verbose("banana");

    filter.print_stats();

    cout << "\nTesting with different parameters \n";
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
