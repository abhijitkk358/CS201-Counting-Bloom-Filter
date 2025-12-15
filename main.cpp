// Counting Bloom Filter

#include <iostream>
#include <cstdint>
#include <cmath>
using namespace std;

typedef uint32_t U32;
typedef uint8_t U8;

template <typename T>
T *allocate_array(U32 size)
{
    T *arr = new T[size];
    for (U32 i = 0; i < size; i++)
    {
        arr[i] = 0;
    }
    return arr;
}

template <typename T>
void deallocate_array(T *arr)
{
    delete[] arr;
}

U32 str_length(const char *str)
{
    U32 len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

inline U32 rotl32(U32 x, int8_t r)
{
    U32 s = static_cast<U32>(r) & 31U;
    return (x << s) | (x >> (32u - s));
}

inline U32 avalanche32(U32 h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

U32 murmur3_hash(const void *key, U32 len, U32 seed)
{
    const U8 *data = (const U8 *)key;
    const U32 nblocks = len / 4;

    U32 h1 = seed;
    const U32 c1 = 0xcc9e2d51;
    const U32 c2 = 0x1b873593;

    const U32 *blocks = (const U32 *)(data);

    for (U32 i = 0; i < nblocks; i++)
    {
        U32 k1 = blocks[i];

        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = rotl32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    const U8 *tail = (const U8 *)(data + nblocks * 4);
    U32 k1 = 0;

    switch (len & 3)
    {
    case 3:
        k1 ^= tail[2] << 16;
    case 2:
        k1 ^= tail[1] << 8;
    case 1:
        k1 ^= tail[0];
        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;
        h1 ^= k1;
    }

    h1 ^= len;
    h1 = avalanche32(h1);

    return h1;
}

class CountingBloomFilter
{
private:
    U8 *counters;
    U32 m;
    U32 k;
    U32 num_items;

    U32 get_hash(const char *item, U32 hash_num)
    {
        U32 len = str_length(item);
        U32 hash1 = murmur3_hash(item, len, 0);
        U32 hash2 = murmur3_hash(item, len, hash1);

        return (hash1 + hash_num * hash2) % m;
    }

public:
    CountingBloomFilter(U32 array_size, U32 num_hashes)
    {
        m = array_size;
        k = num_hashes;
        num_items = 0;
        counters = allocate_array<U8>(m);
    }

    ~CountingBloomFilter()
    {
        deallocate_array(counters);
    }

    void insert(const char *item)
    {
         cout << "\n[INSERT] Item: '" << item << "'\n";

        for (U32 i = 0; i < k; i++)
        {
            U32 index = get_hash(item, i);
             cout << "  Hash " << (i + 1) << ": position " << index
                      << " (counter: " << (int)counters[index]
                      << " -> " << (int)(counters[index] + 1) << ")\n";

            if (counters[index] < 255)
            {
                counters[index]++;
            }
        }
        num_items++;

         cout << "  Current state: n= " << num_items
                  << ", m=" << m << ", k= " << k
                  << ", Flase positive rate = " << (get_false_positive_rate() * 100.0)
                  << " %\n";
    }

    bool query(const char *item)
    {
        return query_verbose(item, false);
    }

    void remove(const char *item)
    {
         cout << "\n[REMOVE] Item: '" << item << "'\n";

        if (!query(item))
        {
             cout << "  Item not found in filter (all counters must be > 0)\n";
            return;
        }

        for (U32 i = 0; i < k; i++)
        {
            U32 index = get_hash(item, i);
             cout << "  Hash " << (i + 1) << ": position " << index
                      << " (counter: " << (int)counters[index]
                      << " -> " << (int)(counters[index] - 1) << ")\n";

            if (counters[index] > 0)
            {
                counters[index]--;
            }
        }
        if (num_items > 0)
        {
            num_items--;
        }

         cout << "  Current state: n=" << num_items
                  << ", m=" << m << ", k=" << k
                  << ", P(fp)=" << (get_false_positive_rate() * 100.0)
                  << " %\n";
    }

    bool query_verbose(const char *item, bool verbose = false)
    {
        if (verbose)
        {
             cout << "\n[QUERY] Item: '" << item << "'\n";
        }

        for (U32 i = 0; i < k; i++)
        {
            U32 index = get_hash(item, i);

            if (verbose)
            {
                 cout << "  Hash " << (i + 1) << ": position " << index
                          << " (counter = " << (int)counters[index] << ")" << '\n';
            }

            if (counters[index] == 0)
            {
                if (verbose)
                {
                     cout << "  Result: NOT FOUND (definitely not in filter)\n";
                }
                return false;
            }
            if (verbose)
            {
                 cout << "\n";
            }
        }

        if (verbose)
        {
             cout << "  Result: may be present \n";
             cout << "  Current n=" << num_items
                      << ", m=" << m << ", k=" << k
                      << ", P(fp) = " << (get_false_positive_rate() * 100.0)
                      << "%\n";
        }

        return true;
    }

    double get_false_positive_rate()
    {
        if (num_items == 0)
            return 0.0;

        // Formula: FPR = (1 - e^(-k*n/m))^k
        double exponent = -(double)k * (double)num_items / (double)m;
        double prob = 1.0 - exp(exponent);
        double result = pow(prob, (double)k);

        return result;
    }

    static U32 calculate_optimal_k(U32 m, U32 expected_n)
    {
        if (expected_n == 0)
            return 1;

        double k_optimal = ((double)m / (double)expected_n) * 0.6931;

        U32 k = (U32)(k_optimal + 0.5);
        return (k < 1) ? 1 : k;
    }

    void print_stats()
    {

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

         cout << "False posiive rate = " << (fp_rate * 100.0) << " %\n\n";

         cout << "OPTIMAL k :\n";

        U32 optimal_k = calculate_optimal_k((U32)m, (U32)num_items);
         cout << optimal_k << '\n';
    }
};

int main()
{
     cout << "Counting Bloom Filter\n";

    U32 m = 1000;
    U32 expected_n = 100;
    U32 k = CountingBloomFilter::calculate_optimal_k(m, expected_n);
     cout << "Optimal k for m = " << m << ", n = " << expected_n << ": " << k << "\n\n";
    CountingBloomFilter filter(m, k);

    while (true)
    {
         string op, item;
         cout << "\nEnter operation (add/query/remove/stats/exit): ";
         cin >> op;

        if (op == "exit")
            break;
        if (op == "stats")
        {
            filter.print_stats();
            continue;
        }

         cout << "Enter item: ";
         cin >> item;

        if (op == "add")
        {
            filter.insert(item.c_str());
             cout << "Inserted \"" << item << "\".\n";
        }
        else if (op == "remove")
        {
            filter.remove(item.c_str());
             cout << "Removed \"" << item << "\".\n";
        }
        else if (op == "query")
        {
            bool found = filter.query_verbose(item.c_str(), 1);
             cout << "\"" << item << "\" is "
                      << (found ? "probably present." : "definitely not present.") << '\n';
        }
        else
        {
             cout << "Unknown operation. Use add/query/remove/stats/exit.\n";
        }

         cout << "Current false positive rate: "
                  << filter.get_false_positive_rate() * 100.0 << " %\n";
    }

    return 0;
}
