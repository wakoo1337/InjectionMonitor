#pragma once
struct LogMessageStruct {
	unsigned int size;
	unsigned int pid;
	unsigned int function;
	uint8_t content[];
};