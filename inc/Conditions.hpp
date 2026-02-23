#pragma once

typedef enum {
	Unconditional = 0,
  SockRead = (1u << 0),
  SockWrite = (1u << 1),
  FSockRead = (1u << 2),
  FSockWrite = (1u << 3)
} Conditions;
