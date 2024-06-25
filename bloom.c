#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <openssl/sha.h>

#define ARRAY_SIZE 1e12
#define NUM_HASH_FUNCTIONS 3
#define MAX_WORD_LENGTH 100


typedef struct {
    size_t *bit_array;
    size_t array_size;
    size_t num_hash_functions;
} BloomFilter;

BloomFilter* create_bloom_filter(size_t array_size, size_t num_hash_functions);
void destroy_bloom_filter(BloomFilter *filter);
void add_to_bloom_filter(BloomFilter *filter, const void *element, size_t element_size);
bool contains(const BloomFilter *filter, const void *element, size_t element_size);
size_t hash(const void *data, size_t len, int seed);
void sha256_hash_with_seed(const char *string, unsigned int seed, unsigned char hash[SHA256_DIGEST_LENGTH]);


BloomFilter* create_bloom_filter(size_t array_size, size_t num_hash_functions){
    BloomFilter *filter = malloc(sizeof(BloomFilter));
    if (!filter) return NULL;

    filter -> bit_array = calloc((array_size + 7) / 8, sizeof(size_t));
    if (!filter -> bit_array){
        free(filter);
        return NULL;
    }

    filter -> array_size = array_size;
    filter -> num_hash_functions = num_hash_functions;
    return filter;
}


void destroy_bloom_filter(BloomFilter *filter){
    if (filter){
        free(filter -> bit_array);
        free(filter);
    }
}


void add_to_bloom_filter(BloomFilter *filter, const void *element, size_t element_size){
    for (size_t i = 0; i < filter -> num_hash_functions; i++){
        size_t hash_value;
        switch (i) {
            case 0: hash_value = hash(element, element_size, 1); break;
            case 1: hash_value = hash(element, element_size, 2); break;
            case 2: hash_value = hash(element, element_size, 3); break;
        }
        // reduce bit index to the size of the array have
        size_t bit_index = hash_value % filter -> array_size;
        // Set the Bloom filter's bit arrays index to 1
        filter -> bit_array[bit_index / 8] |= (1 << (bit_index % 8));
    }
}


bool contains(const BloomFilter *filter, const void *element, size_t element_size){
    for (size_t i = 0; i < filter -> num_hash_functions; i++){
        size_t hash_value;
        switch(i) {
            case 0: hash_value = hash(element, element_size, 1); break;
            case 1: hash_value = hash(element, element_size, 2); break;
            case 2: hash_value = hash(element, element_size, 3); break;
        }
        size_t bit_index = hash_value % filter -> array_size;
        // If any value is 0 where 1 is expected return false
        if (!(filter->bit_array[bit_index / 8] & (1 << (bit_index % 8)))) {
            return false;
        }
    }
    // otherwise all 1s collided, which means filter might contain the element
    return true;
}


size_t hash(const void *data, size_t len, int seed){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    sha256_hash_with_seed(data, seed, hash);
    return *(size_t*)hash;
}


void sha256_hash_with_seed(const char *string, unsigned int seed, unsigned char hash[SHA256_DIGEST_LENGTH]){
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    unsigned char seed_bytes[4];
    seed_bytes[0] = (seed >> 24) & 0xFF;
    seed_bytes[1] = (seed >> 16) & 0xFF;
    seed_bytes[2] = (seed >> 8) & 0xFF;
    seed_bytes[3] = seed & 0xFF;

    SHA256_Update(&sha256, &seed, sizeof(seed));
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(hash, &sha256);
}


void add_words_from_file(BloomFilter *filter, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }
    char word[MAX_WORD_LENGTH];
    while (fscanf(file, "%99s", word) == 1) {
        int len = strlen(word);
        while (len > 0 && !isalnum((unsigned char)word[len-1])) {
            word[--len] = '\0';
        }

        if (len > 0) {
            add_to_bloom_filter(filter, word, strlen(word));
        }
    }
    fclose(file);
}


int main() {
    BloomFilter *filter = create_bloom_filter(ARRAY_SIZE, NUM_HASH_FUNCTIONS);
    if (!filter) {
        printf("Failed to create bloom filter!\n");
        return 1;
    }

    const char *filename = "input.txt";
    add_words_from_file(filter, filename);

    const char *test_words[] = {"requirement", "apple", "kafka", "bloom", "filter", "over", "banana", "fwefwe"};
    int num_test_words = sizeof(test_words) / sizeof(test_words[0]);

    for (int i = 0; i < num_test_words; i++) {
        bool may_exist = contains(filter, test_words[i], strlen(test_words[i]));
        printf("'%s' might be in the filter: %s\n", test_words[i], may_exist ? "true" : "false");
    }

    destroy_bloom_filter(filter);
    return 0;
}
