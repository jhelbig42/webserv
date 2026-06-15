#pragma once

#define MAX_REQUEST 1024 // will later be determined by config file
//#define BACKLOG 200
#define BACKLOG SOMAXCONN
#define BYTES_PER_CHUNK 1024
#define TIMEOUT 60 // seconds of inactivity before client is marked for deletion
#define CGI_TIMEOUT                                                            \
  5 // seconds of inactivity (not sending anything to buffer) before CGI process
    // is killed
