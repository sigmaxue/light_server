#ifndef VAR_H_
#define VAR_H_

extern const int   kIOErrorClose;
extern const int   kIOErrorTryAgain;
extern const int   kIOErrorEAGAIN;
extern const int   kIOErrorFatal;
extern const int   kReactorMaxTaskNum;
extern const char* kRsp200;

extern int kTypeLed;
// task
extern const int kTaskFinish;
extern const int kTaskContinue;
extern const int kTaskHolding;
extern const int kTaskClose;

#define kBufferSize ( 10 * 1024 * 1024 )
#endif /* VAR_H_ */
