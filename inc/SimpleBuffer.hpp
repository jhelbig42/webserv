#pragma once

struct SimpleBuffer {
	char *section[1024];
	SimpleBuffer *next;
};
