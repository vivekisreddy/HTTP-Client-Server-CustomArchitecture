Author: Vivek Reddy Kasireddy 
Assignment: Packet Scheduling Program 
Date: 2/22/24 

This README file provides instructions on compiling and testing the Packet Scheduling program. The program is designed to determine the order of outgoing packets based on their priorities.

Compilation

1.1 C Program Compilation

To compile the C program (`packet_scheduling.c`), follow these steps:

1. Open your terminal or command prompt.
2. Navigate to the directory containing the source code (`packet_scheduling.c`).

    
    cd /path/to/your/directory
    

3. Compile the program using the `gcc` compiler:
    using makefile: 
    make 
    gcc packet_scheduling.c -o packet_scheduling
   
    This command compiles the source code and produces an executable file named `packet_scheduling`.

1.2 Makefile Compilation

Alternatively, you can compile the program using the provided Makefile. To compile using the Makefile, follow these steps:

1. Open your terminal or command prompt.
2. Navigate to the directory containing the Makefile and the source code (`packet_scheduling.c`).
3. Run the `make` command:
    make
    This command will compile the program according to the rules specified in the Makefile.

______________________________________________________________________________________________________________________________________________________
2. Testing
2.1 Testing Input Validity

The program has several input validity checks. To test these checks, 
we'll create input files with different scenarios and observe the program's behavior.

Case 1: Correct input
Create an input file named `sample1.in` with correct input:
    5 HLHLH

Run the program with this input file:
    ./packet_scheduling < sample1.in

Expected output:
    0 2 1 4 3


Case 2: Wrong length of priority string
Create an input file named `sample2.in` with a mismatch between the number of packets and the length of the priority string:

    4 HLL

Run the program with this input file:

    ./packet_scheduling < sample2.in

Expected output:
    Wrong input: the number of packets is wrong.


 Case 3: Wrong number of packets

Create an input file named `sample3.in` with an incorrect number of packets:
    0 HLHLH

Run the program with this input file:
    ./packet_scheduling < sample3.in

Expected output:
    Wrong input: the number of packets must be greater than 0.


Case 4: Wrong priority
Create an input file named `sample4` with incorrect priority characters:
    4 HXHH

Run the program with this input file:
    ./packet_scheduling < sample4.in

Expected output:
    Wrong input: the priority must be H or L.


2.2 Testing Output File

After running the program with each input file, check the `output.txt` file for the generated output. Ensure that the output matches the expected output for each test case.
