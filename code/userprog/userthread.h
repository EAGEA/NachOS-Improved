#ifndef USERTHREAD_H
#define USERTHREAD_H


int do_UserThreadCreate(int fun, int arg, int returnFun) ;

void do_UserThreadExit() ;

int do_UserThreadJoin(int t) ;

int do_UserThreadId() ;


#endif
