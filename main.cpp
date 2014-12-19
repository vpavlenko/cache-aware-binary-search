#include <iostream>
#include <cassert>
#include <cstdlib>
#include <chrono>
#include <random>
#include <string>

using namespace std;

const int SORTED_ARRAY_SIZE = 100 * 1000 * 1000;
const int NUM_ITERS = 100 * 1000;

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
    std::uniform_int_distribution random_int_generator(0, rand_max);
    for (int i = 0; i < array_size; ++i) {
        result[i] = random_int_generator(generator);
    }
    return result;
}

int binary_search_slow(int *array, int array_size, int value) {
    int left = 0, right = array_size;
    while (left + 1 < right) {
        int mid = (left + right) >> 1;
        if (array[mid] <= value) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return array[left];
}

class FastBinarySearch {
public:
    FastBinarySearch(int *sorted_array, int sorted_array_size) :
            sorted_array_(sorted_array), sorted_array_size_(sorted_array_size) {

    }

    int upper_bound(int query) const {

    }

private:
    int *sorted_array_;
    int sorted_array_size_;
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
        std::cout << name_ << ", " << num_iters_ << "iterations: " << delta << " ns";
        std::cout << name_ << ": " << delta / (double)num_iters_ << " ns per iteration";
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
    std::string name_;
    int num_iters_;
};

int main() {
    cout << "get_sorted_array BEGIN\n";
    int *sorted_array = get_sorted_array(SORTED_ARRAY_SIZE);
    cout << "get_sorted_array END\n";

    int *query = get_random_array(NUM_ITERS, sorted_array[SORTED_ARRAY_SIZE - 1]);

    int *slow_result = new int[NUM_ITERS];
    int *fast_result = new int[NUM_ITERS];

    Measurer measurer_slow("slow", NUM_ITERS);
    measurer_slow.start();
    for (int i = 0; i < NUM_ITERS; ++i) {
        slow_result[i] = binary_search_slow(sorted_array, SORTED_ARRAY_SIZE, query[i]);
    }
    measurer_slow.stop();

    Measurer measurer_fast("fast", NUM_ITERS);
    FastBinarySearch fastBinarySearch(sorted_array, SORTED_ARRAY_SIZE);
    measurer_fast.start();
    for (int i = 0; i < NUM_ITERS; ++i) {
        fast_result[i] = fastBinarySearch.upper_bound(query[i]);
    }
    measurer_fast.stop();

    // TODO: results assertion

    return 0;
}