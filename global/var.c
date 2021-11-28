#include "global/var.h"

const int kIOErrorClose    = -1000;
const int kIOErrorTryAgain = -1001;
const int kIOErrorEAGAIN   = -1002;
const int kIOErrorFatal    = -1003;

const int   kBufferSize        = 10 * 1024 * 1024;
const int   kReactorMaxTaskNum = 1024;
const char* kRsp200            = "HTTP/1.1 200 OK\r\n\r\n";

const int kTypeLed = 0;
