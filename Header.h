#pragma once

#define NUM_TEST_VALUES 300
#define NUM_REG 2
#define MAX_SHIFT 7
#define MAX_PROGRAM_LENGTH 7

struct instruction {
	int opcode;
	int arg1;
	int arg2;
};
