#pragma once

typedef enum {
  Generic = 0,
  Head = (1u << 0),
  Get = (1u << 1),
  Post = (1u << 2),
  Delete = (1u << 3)
} HttpMethod;
