#include <iostream>
#include <cassert>
#include <cstdlib>
#include <chrono>
#include <random>
#include <string>
#include <cmath>

using namespace std;

const int SORTED_ARRAY_SIZE = 100 * 1000 * 1000;
const int NUM_ITERS = 100 * 1000;
const int L3_INT_ARRAY_SIZE = 100 * 1000;
const int L1_INT_ARRAY_SIZE = 2 * 1000;

void show_sketch(string name, int *array, int array_size) {
    cout << name << ": [ ";
    for (int i = 0; i < 5; ++i) {
        cout << array[i] << " ";
    }
    cout << "... ";
    for (int i = array_size - 5; i < array_size; ++i) {
        cout << array[i] << " ";
    }
    cout << "]\n";
}

int *get_sorted_array(const int array_size) {
    int *result = new int[array_size];
    int delta = 10;
    result[0] = 0;
    for (int i = 1; i < array_size; ++i) {
        result[i] = result[i - 1] + rand() % delta + 1;
    }
    assert(0.2 * delta * array_size <= result[array_size - 1]);
    assert(result[array_size - 1] <= delta * array_size);
    return result;
}

int *get_random_array(int array_size, int rand_max) {
    int *result = new int[array_size];
    std::minstd_rand generator;
    std::uniform_int_distribution<> random_int_generator(0, rand_max);
    for (int i = 0; i < array_size; ++i) {
        result[i] = random_int_generator(generator);
    }
    return result;
}

class SlowBinarySearch {
public:
    SlowBinarySearch() {}

    SlowBinarySearch(int *sorted_array, int sorted_array_size) :
            sorted_array_(sorted_array), sorted_array_size_(sorted_array_size) {

    }

    int upper_bound(int query) const {
        return upper_bound(query, 0, sorted_array_size_);
    }

    int upper_bound(int query, int left, int right) const {
        while (left + 1 < right) {
            int mid = (left + right) >> 1;
            if (sorted_array_[mid] <= query) {
                left = mid;
            } else {
                right = mid;
            }
        }
        return left;
    }

private:
    int *sorted_array_;
    int sorted_array_size_;
};

class FastBinarySearch {
public:
    FastBinarySearch(int *sorted_array, int sorted_array_size) :
            sorted_array_(sorted_array), sorted_array_size_(sorted_array_size) {
        arrayBinarySearch_ = SlowBinarySearch(sorted_array, sorted_array_size);



        middle_level_chunk_size_ = sorted_array_size / L3_INT_ARRAY_SIZE;
        middle_level_array_size_ = ceil((double)sorted_array_size / middle_level_chunk_size_);
        cout << "middle_level_chunk_size: " << middle_level_chunk_size_ << "\n";
        cout << "middle_level_array_size_: " << middle_level_array_size_ << "\n";

        int *middle_level_sorted_array =
                get_slice_array_(sorted_array, sorted_array_size,
                        middle_level_array_size_, middle_level_chunk_size_);
        middleLevelBinarySearch_ = SlowBinarySearch(middle_level_sorted_array, middle_level_array_size_);

        show_sketch("middle_level_array", middle_level_sorted_array, middle_level_array_size_);



        top_level_chunk_size_ = middle_level_array_size_ / L1_INT_ARRAY_SIZE;
        int top_level_array_size = ceil((double)middle_level_array_size_ / top_level_chunk_size_);
        cout << "top_level_chunk_size: " << top_level_chunk_size_ << "\n";
        cout << "top_level_array_size: " << top_level_array_size << "\n";

        int *top_level_sorted_array =
                get_slice_array_(middle_level_sorted_array, middle_level_array_size_,
                        top_level_array_size, top_level_chunk_size_);
        topLevelBinarySearch_ = SlowBinarySearch(top_level_sorted_array, top_level_array_size);

        show_sketch("top_level_array", top_level_sorted_array, top_level_array_size);
    }

    int upper_bound(int query) const {
        int top_level_answer = topLevelBinarySearch_.upper_bound(query);
        int middle_left = top_level_answer * top_level_chunk_size_;
        int middle_right = min((top_level_answer + 1) * top_level_chunk_size_, middle_level_array_size_);

        int middle_level_answer = middleLevelBinarySearch_.upper_bound(query, middle_left, middle_right);

        int left = middle_level_answer * middle_level_chunk_size_;
        int right = min((middle_level_answer + 1) * middle_level_chunk_size_, sorted_array_size_);

        return arrayBinarySearch_.upper_bound(query, left, right);
    }

private:
    int *sorted_array_;
    int sorted_array_size_, middle_level_chunk_size_, top_level_chunk_size_, middle_level_array_size_;
    SlowBinarySearch arrayBinarySearch_, middleLevelBinarySearch_, topLevelBinarySearch_;

    int *get_slice_array_(int *source_array, int source_array_size, int dest_array_size, int dest_chunk_size) {
        int *dest_array = new int[dest_array_size];
        for (int i = 0; i < dest_array_size; ++i) {
            dest_array[i] = source_array[i * dest_chunk_size];
        }
        return dest_array;
    }
};

class Measurer {
public:
    Measurer(string name, int num_iters) : name_(name), num_iters_(num_iters) {}

    void start() {
        start_ = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        auto stop = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start_).count();
        assert((sizeof delta) == 8);
        std::cout << name_ << ", " << num_iters_ << " iterations: " << delta << " ns\n";
        std::cout << name_ << ": " << delta / (double)num_iters_ << " ns per iteration\n";
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
    std::string name_;
    int num_iters_;
};

void assertEquals(int *first_array, int *second_array, int array_size) {
    for (int i = 0; i < array_size; ++i) {
        if (first_array[i] != second_array[i]) {
            throw std::runtime_error(string("difference at ") + to_string(i) + ": "
                "first_array[i] = " + to_string(first_array[i]) + ", "
                "second_array[i] = " + to_string(second_array[i]));
        }
    }
}

int main() {
    int *sorted_array = get_sorted_array(SORTED_ARRAY_SIZE);

    int *query = get_random_array(NUM_ITERS, sorted_array[SORTED_ARRAY_SIZE - 1]);



    int *slow_result = new int[NUM_ITERS];

    show_sketch("sorted_array", sorted_array, SORTED_ARRAY_SIZE);
    show_sketch("query", query, NUM_ITERS);

    Measurer measurer_slow("slow", NUM_ITERS);
    SlowBinarySearch slowBinarySearch(sorted_array, SORTED_ARRAY_SIZE);

    measurer_slow.start();
    for (int i = 0; i < NUM_ITERS; ++i) {
        slow_result[i] = slowBinarySearch.upper_bound(query[i]);
    }
    measurer_slow.stop();

    show_sketch("slow_result", slow_result, NUM_ITERS);



    int *fast_result = new int[NUM_ITERS];

    Measurer measurer_fast("fast", NUM_ITERS);
    FastBinarySearch fastBinarySearch(sorted_array, SORTED_ARRAY_SIZE);

    // warm-up iterations
    for (int i = 0; i < NUM_ITERS; ++i) {
        fastBinarySearch.upper_bound(query[i]);
    }

    measurer_fast.start();
    for (int i = 0; i < NUM_ITERS; ++i) {
        fast_result[i] = fastBinarySearch.upper_bound(query[i]);
    }
    measurer_fast.stop();

    show_sketch("fast_result", fast_result, NUM_ITERS);



    assertEquals(slow_result, fast_result, NUM_ITERS);

    return 0;
}
