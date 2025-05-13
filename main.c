#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define ALPHABET_SIZE 26
#define TOP_N 3   // for the top 3 shifts

// -------------------------------------------------------
// 1. READ ENGLISH LETTER DISTRIBUTION
// -------------------------------------------------------
void read_distribution(const char *filename, double distribution[ALPHABET_SIZE]) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (fscanf(fp, "%lf", &distribution[i]) != 1) {
            fprintf(stderr, "Error: Invalid format in %s\n", filename);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);
}

// -------------------------------------------------------
// 2. COMPUTE HISTOGRAM (FREQUENCY) OF TEXT
// -------------------------------------------------------
void compute_histogram(const char *text, double histogram[ALPHABET_SIZE]) {
    // First initialize
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        histogram[i] = 0.0;
    }

    int total_count = 0;
    // Count occurrences
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        if (isalpha((unsigned char)c)) {
            c = (char)tolower((unsigned char)c);
            histogram[c - 'a'] += 1.0;
            total_count++;
        }
    }

    // Normalize
    if (total_count > 0) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            histogram[i] /= (double)total_count;
        }
    }
}

// -------------------------------------------------------
// 3A. CHI-SQUARED DISTANCE
// -------------------------------------------------------
double chi_squared_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double distance = 0.0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        // expected count is hist2[i], observed is hist1[i]
        // Because we stored them as normalized frequencies, we can interpret
        // them as probabilities. 
        if (hist2[i] > 1e-15) {
            double diff = hist1[i] - hist2[i];
            distance += (diff * diff) / hist2[i];
        }
        else {
            // If the expected frequency is extremely small, you might skip or handle carefully
            // to avoid dividing by zero. For a simple approach, do nothing or treat it as is.
        }
    }
    return distance;
}

// -------------------------------------------------------
// 3B. EUCLIDEAN DISTANCE
// -------------------------------------------------------
double euclidean_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double sum_sq = 0.0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        double diff = hist1[i] - hist2[i];
        sum_sq += diff * diff;
    }
    // We could either return sqrt(sum_sq) or just sum_sq
    // Typically Euclidean distance is sqrt of the sum, but either is fine as
    // long as you're consistent.
    return sqrt(sum_sq);
}

// -------------------------------------------------------
// 3C. COSINE DISTANCE
// -------------------------------------------------------
double cosine_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double dot = 0.0;
    double normA = 0.0;
    double normB = 0.0;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        dot   += hist1[i] * hist2[i];
        normA += hist1[i] * hist1[i];
        normB += hist2[i] * hist2[i];
    }

    if (normA < 1e-15 || normB < 1e-15) {
        return 1.0;  // If one vector is essentially zero, define distance as max (1).
    }

    double similarity = dot / (sqrt(normA) * sqrt(normB));
    return 1.0 - similarity;
}

// -------------------------------------------------------
// SHIFT A STRING BY 'shift' for Caesar cipher
// (positive for encryption, negative for decryption).
// -------------------------------------------------------
void caesar_shift(const char *input, char *output, int shift) {
    // Use shift mod 26 to handle shifts > 26 or negative
    int effectiveShift = shift % ALPHABET_SIZE;
    if (effectiveShift < 0) {
        effectiveShift += ALPHABET_SIZE;
    }

    int i = 0;
    while (input[i] != '\0') {
        char c = input[i];
        if (isalpha((unsigned char)c)) {
            int isUpper = isupper((unsigned char)c);
            char base = isUpper ? 'A' : 'a';
            int alphaIndex = (c - base + effectiveShift) % ALPHABET_SIZE;
            output[i] = (char)(base + alphaIndex);
        } else {
            output[i] = c;  // leave non-alpha alone
        }
        i++;
    }
    output[i] = '\0';
}

// -------------------------------------------------------
// 4. BREAK CAESAR CIPHER BY TRYING ALL SHIFTS
//    distance_function can be one of: chi_squared_distance,
//    euclidean_distance, or cosine_distance.
// -------------------------------------------------------
void break_caesar_cipher(const char* text,int top_shifts[TOP_N],double top_distances[TOP_N],double (*distance_function)(const double[], const double[]),const double english_dist[ALPHABET_SIZE]) {

    // Initialize top distances to large values
    for (int i = 0; i < TOP_N; i++) {
        top_shifts[i] = 0;
        top_distances[i] = 1e15;  // Some large number
    }

    // We will try all shifts from 0 to 25
    for (int s = 0; s < ALPHABET_SIZE; s++) {
        // Shift text by -s to decrypt with guess s
        // or we can shift by +((26 - s) mod 26).
        // Then compute the histogram of that result
        char shiftedText[1024*10];
        caesar_shift(text, shiftedText, -s);

        // Compute histogram
        double hist[ALPHABET_SIZE];
        compute_histogram(shiftedText, hist);

        // Compare with standard English distribution
        double dist = distance_function(hist, english_dist);

        // Insert into the top 3 if appropriate
        // (descending or ascending order depends on definition,
        // we want the smallest distances, so we check if dist < top_distances[j])
        for (int j = 0; j < TOP_N; j++) {
            if (dist < top_distances[j]) {
                // shift elements down
                for (int k = TOP_N - 1; k > j; k--) {
                    top_distances[k] = top_distances[k-1];
                    top_shifts[k] = top_shifts[k-1];
                }
                // Insert new
                top_distances[j] = dist;
                top_shifts[j] = s;
                break;
            }
        }
    }
}

// -------------------------------------------------------
// 5. MENU 
// -------------------------------------------------------
int main(void) {
    // distribution.txt is a file 
    // with each of the 26 lines containing the distribution for 'a'..'z'.
    double englishDist[ALPHABET_SIZE];
    read_distribution("distribution.txt", englishDist);

    while (1) {
        printf("\n--- Caesar Cipher Menu ---\n");
        printf("1) Encrypt a text with Caesar shift\n");
        printf("2) Decrypt a text with known shift\n");
        printf("3) Compute and display letter frequency of text\n");
        printf("4) Break Caesar cipher using frequency analysis\n");
        printf("5) Exit\n");

        int choice;
        scanf("%d", &choice);
        getchar(); 

        if (choice == 5) {
            printf("Exiting...\n");
            break;
        }
        else if (choice == 1) {
            printf("Enter text to encrypt:\n");
            char text[1024*10];
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = '\0';

            printf("Enter shift:\n");
            int shift;
            scanf("%d", &shift);
            getchar();

            char encrypted[1024*10];
            caesar_shift(text, encrypted, shift);
            printf("Encrypted text: %s\n", encrypted);
        }
        else if (choice == 2) {
            printf("Enter text to decrypt:\n");
            char text[1024*10];
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = '\0';

            printf("Enter shift (the original encryption shift):\n");
            int shift;
            scanf("%d", &shift);
            getchar();

            char decrypted[1024*10];
            // To decrypt an encryption shift of +shift, we apply -shift:
            caesar_shift(text, decrypted, -shift);
            printf("Decrypted text: %s\n", decrypted);
        }
        else if (choice == 3) {
            printf("Enter text to analyze:\n");
            char text[1024*10];
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = '\0';

            double hist[ALPHABET_SIZE];
            compute_histogram(text, hist);

            // Display as a percentage
            printf("Letter frequency:\n");
            for (int i = 0; i < ALPHABET_SIZE; i++) {
                printf("%c: %.4f\n", 'a' + i, hist[i] * 100.0);
            }
        }
        else if (choice == 4) {
            printf("Enter text to break:\n");
            char text[1024*10];
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = '\0';

            printf("Choose distance metric:\n");
            printf("   1) Chi-squared\n");
            printf("   2) Euclidean\n");
            printf("   3) Cosine\n");
            int metricChoice;
            scanf("%d", &metricChoice);
            getchar();

            double (*distance_func)(const double[], const double[]) = NULL;
            if (metricChoice == 1) distance_func = chi_squared_distance;
            else if (metricChoice == 2) distance_func = euclidean_distance;
            else distance_func = cosine_distance; // default if invalid: pick cosine

            int shifts[TOP_N];
            double distances[TOP_N];
            break_caesar_cipher(text, shifts, distances, distance_func, englishDist);

            printf("Top %d probable shifts:\n", TOP_N);
            for (int i = 0; i < TOP_N; i++) {
                int shiftVal = shifts[i];
                double distVal = distances[i];
                printf(" Shift=%d, Distance=%.6f\n", shiftVal, distVal);

                // Optionally show what the text looks like with this shift
                char candidate[1024*10];
                caesar_shift(text, candidate, -shiftVal);
                printf("   Decryption guess: %s\n\n", candidate);
            }
        }
        else {
            printf("Unknown choice.\n");
        }
    }

    return 0;
}
