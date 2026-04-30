#include <stdio.h> // Including standard input-output library for input and output operations
#include <stdlib.h> // Including standard library for memory allocation and other utility functions
#include <string.h> // Including string library for string manipulation functions

#define MAX_PACKETS 100 // Defining a constant for maximum number of packets

void print_error(const char *error_message) { // Function to print error messages
    printf("%s\n", error_message); // Print the error message to standard output
    exit(1); // Exit the program with status 1 (indicating error)
}

void print_outgoing_order(int num_packets, char *priority, FILE *output_file) { // Function to determine outgoing order of packets
    if (num_packets <= 0) { // If the number of packets is less than or equal to 0
        fprintf(output_file, "Wrong input: the number of packets must be greater than 0.\n"); // Print error message to output file
        return; // Exit the function
    }

    int priority_length = strlen(priority); // Calculate the length of the priority string
    if (priority_length != num_packets) { // If the length of priority string does not match the number of packets
        fprintf(output_file, "Wrong input: the number of packets is wrong.\n"); // Print error message to output file
        return; // Exit the function
    }

    int i; // Declare variable for loop iteration
    for (i = 0; i < priority_length; i++) { // Loop through each character in the priority string
        if (priority[i] != 'H' && priority[i] != 'L') { // If the character is neither 'H' nor 'L'
            fprintf(output_file, "Wrong input: the priority must be H or L.\n"); // Print error message to output file
            return; // Exit the function
        }
    }

    // Print number of incoming packets and priority string to terminal
    printf("Number of packets: %d\n", num_packets); // Print number of packets to standard output
    printf("Priority: %s\n", priority); // Print priority string to standard output

    int high_priority_count = 0; // Initialize count for high priority packets
    int low_priority_count = 0; // Initialize count for low priority packets
    int high_priority_indices[MAX_PACKETS]; // Array to store indices of high priority packets
    int low_priority_indices[MAX_PACKETS]; // Array to store indices of low priority packets

    for (i = 0; i < num_packets; i++) { // Loop through each packet
        if (priority[i] == 'H') { // If the packet has high priority
            high_priority_indices[high_priority_count++] = i; // Add its index to the array and increment the count
        } else { // If the packet has low priority
            low_priority_indices[low_priority_count++] = i; // Add its index to the array and increment the count
        }
    }

    for (i = 0; i < high_priority_count; i++) { // Loop through high priority packets
        fprintf(output_file, "%d ", high_priority_indices[i]); // Print the index to output file
    }
    for (i = 0; i < low_priority_count; i++) { // Loop through low priority packets
        fprintf(output_file, "%d ", low_priority_indices[i]); // Print the index to output file
    }
    fprintf(output_file, "\n"); // Print newline character to output file
}

int main() { // Main function
    int num_packets; // Variable to store number of packets
    char priority[MAX_PACKETS]; // Array to store priority string

    // Read input
    scanf("%d", &num_packets); // Read number of packets from standard input
    scanf("%s", priority); // Read priority string from standard input

    // Check if output file exists
    FILE *output_file = fopen("output.txt", "w"); // Open output file for writing
    if (output_file == NULL) { // If output file cannot be opened
        print_error("Error opening output file."); // Print error message and exit
    }

    // Check input validity and print outgoing order
    print_outgoing_order(num_packets, priority, output_file); // Call function to determine outgoing order of packets

    // Close output file
    fclose(output_file); // Close the output file

    return 0; // Return 0 to indicate successful execution
}
