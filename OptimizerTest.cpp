#include <iostream>
#include "Header.h"
#include <math.h>

void add(int src, int dst, int regs[]) {
	regs[dst] += regs[src];
}

void sub(int src, int dst, int regs[]) {
	regs[dst] -= regs[src];
}

void move(int src, int dst, int regs[]) {
	regs[dst] = regs[src];
}

void lsl(int shift, int dst, int regs[]) {
	regs[dst] <<= shift;
}

int num_functions = 4;
int num_shifts = 1;
void (*opcode_to_function[])(int arg1, int arg2, int regs[]) = { lsl, move, add, sub };
const char* names[4] = { "lsl", "move", "add", "sub" };

// Runs target program to populate the expect input and output for testing
void populate_test_values(int input[], int output[], int mult) {
	int halfway_point = NUM_TEST_VALUES / 2;
	for (int i = 0; i < NUM_TEST_VALUES; i++) {
		input[i] = i - halfway_point;
		output[i] = input[i] * mult;
	}
}

// Tests input program against the expected input/output
// Returns false as soon as the program fails, to save runtime
bool test_program(instruction program[], int len, int input[], int output[]) {
	for (int i = 0; i < NUM_TEST_VALUES; i++) {
		// Only reset the value for the first input
		int reg_values[NUM_REG];
		reg_values[0] = input[i];

		for (int i = 0; i < len; i++) {
			instruction cur = program[i];
			opcode_to_function[cur.opcode](cur.arg1, cur.arg2, reg_values);
		}

		if (reg_values[0] != output[i]) {
			return false;
		}
	}

	return true;
}

// Prints a program in a nice, human readable way
void print_program(instruction program[], int len) {
	for (int i = 0; i < len; i++) {
		instruction cur = program[i];
		if (cur.opcode >= num_shifts) {
			printf("%s\td%d,d%d\n", names[cur.opcode], cur.arg1, cur.arg2);
		}
		else {
			printf("%s\t#%d,d%d\n", names[cur.opcode], cur.arg1, cur.arg2);
		}
	}
}

// Returns a list of all possible instructions, so we can brute-force search using them
instruction* generate_all_instructions(int* num_instructions) {
	int num_regular = num_functions - num_shifts;
	*num_instructions = num_regular * NUM_REG * NUM_REG + num_shifts * NUM_REG  * (MAX_SHIFT - 1);
	instruction* all_inst = (instruction*)malloc(*num_instructions * sizeof(instruction));
	int cur = 0;

	// Populate all of the shifting instructions, since their arguments are constructed differently
	for (int i = 0; i < num_shifts; i++) {
		for (int j = 1; j < MAX_SHIFT; j++) {
			for (int k = 0; k < NUM_REG; k++) {
				all_inst[cur] = { i, j, k };
				cur++;
			}
		}
	}

	// Populate all of the normal instructions
	for (int i = num_shifts; i < num_functions; i++) {
		for (int j = 0; j < NUM_REG; j++) {
			for (int k = 0; k < NUM_REG; k++) {
				all_inst[cur] = { i, j, k };
				cur++;
			}
		}
	}

	return all_inst;
}

// Generates a program
// Returns the value used to generate the following program
long generate_next_program(long next, instruction* all_inst, int num_instructions, instruction* buffer, int len) {
	long value_remaining = next;
	for (int i = 0; i < len; i++) {
		buffer[i] = all_inst[value_remaining % num_instructions];
		value_remaining /= num_instructions;
	}

	next += 1;
	if (next >= pow(num_instructions, len)) {
		next = -1;
	}

	return next;
}

int main()
{
	int test_input[NUM_TEST_VALUES];
	int test_output[NUM_TEST_VALUES];
	
	int num_instructions;
	instruction* all_inst = generate_all_instructions(&num_instructions);
	printf("Iterating using %d total instructions...\n", num_instructions);

	instruction new_program[MAX_PROGRAM_LENGTH];

	// Iterate over a variety of values to multiply
	for (int i = 105; i < 500; i++) {
		populate_test_values(test_input, test_output, i);

		// Check each program length
		bool found = false;
		for (int program_len = 0; program_len < MAX_PROGRAM_LENGTH && !found; program_len++) {
			long next = 0;
			while (next != -1) {
				next = generate_next_program(next, all_inst, num_instructions, new_program, program_len);

				if (test_program(new_program, program_len, test_input, test_output)) {
					//print_program(new_program, program_len);
					printf("%d %d\n", i, program_len);
					found = true;
					break;
				}
			}
		}

		if (!found) {
			printf("%d %s\n", i, "?");
		}
	}
}
