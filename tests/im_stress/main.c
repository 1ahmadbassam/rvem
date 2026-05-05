#include <stdio.h>

int source_data[16] = {
    45, -12, 1024, 0, 77, -99, 256, 11,
    8, 333, -1, 42, 888, 17, 55, 3
};

int sorted_data[16];
unsigned int final_hash;
int stats_mean;

// ---------------------------------------------------------
// RECURSION & BRANCHING STRESS TEST
// ---------------------------------------------------------
void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

// ---------------------------------------------------------
// MULTIPLICATION & BYTE-ALIGNMENT STRESS TEST
// ---------------------------------------------------------
unsigned int calculate_hash(int arr[], int length) {
    unsigned int hash = 0x811C9DC5;
    unsigned char *bytes = (unsigned char *)arr;
    int byte_length = length * 4;

    for (int i = 0; i < byte_length; i++) {
        hash ^= bytes[i];
        hash *= 0x01000193;
    }
    return hash;
}

// ---------------------------------------------------------
// DIVISION STRESS TEST
// ---------------------------------------------------------
int calculate_mean(int arr[], int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += arr[i]; 
    }
    return sum / length;
}

// ---------------------------------------------------------
// MAIN ENTRY
// ---------------------------------------------------------
int main(void) {
    for (int i = 0; i < 16; i++) {
        sorted_data[i] = source_data[i];
    }

    quicksort(sorted_data, 0, 15);

    final_hash = calculate_hash(sorted_data, 16);
    stats_mean = calculate_mean(sorted_data, 16);

    printf("Source Data:\n");
    for (int i = 0; i < 16; i++) {
        printf("%d%s", source_data[i], (i == 15) ? "" : ", ");
    }

    printf("\n\nSorted Data:\n");
    for (int i = 0; i < 16; i++) {
        printf("%d%s", sorted_data[i], (i == 15) ? "" : ", ");
    }

    printf("\n\nFinal Hash: %u\n", final_hash);
    printf("Stats Mean: %d\n", stats_mean);

    return 0;
}