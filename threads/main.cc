// main.cc 
//	Bootstrap code to initialize the operating system kernel.
//
//	Allows direct calls into internal operating system functions,
//	to simplify debugging and testing.  In practice, the
//	bootstrap code would just initialize data structures,
//	and start a user program to print the login prompt.
//
// 	Most of this file is not needed until later assignments.
//
// Usage: nachos -d <debugflags> -rs <random seed #>
//		-s -x <nachos file> -c <consoleIn> <consoleOut>
//		-f -cp <unix file> <nachos file>
//		-p <nachos file> -r <nachos file> -l -D -t
//              -n <network reliability> -m <machine id>
//              -o <other machine id>
//              -z
//
//    -d causes certain debugging messages to be printed (cf. utility.h)
//    -rs causes Yield to occur at random (but repeatable) spots
//    -z prints the copyright message
//
//  USER_PROGRAM
//    -s causes user programs to be executed in single-step mode
//    -x runs a user program
//    -c tests the console
//
//  FILESYS
//    -f causes the physical disk to be formatted
//    -cp copies a file from UNIX to Nachos
//    -p prints a Nachos file to stdout
//    -r removes a Nachos file from the file system
//    -l lists the contents of the Nachos directory
//    -D prints the contents of the entire file system 
//    -t tests the performance of the Nachos file system
//
//  NETWORK
//    -n sets the network reliability
//    -m sets this machine's host id (needed for the network)
//    -o runs a simple test of the Nachos network software
//
//  NOTE -- flags are ignored until the relevant assignment.
//  Some of the flags are interpreted here; some in system.cc.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#define MAIN
#include "copyright.h"
#undef MAIN

#include "utility.h"
#include "system.h"

#ifdef THREADS
// external arguments defined in threadtest.cc
extern int testnum;
extern int threadnum;    // 【添加】线程数量
extern int oprnum;      // 【添加】单线程list操作数个数
extern bool yield_flag; // 【添加】线程切换开关
extern int S; // 错误场景选择show: 1~4
#endif

// External functions used by this file

extern void ThreadTest(void), Copy(char *unixFile, char *nachosFile);
extern void Print(char *file), PerformanceTest(void);
extern void StartProcess(char *file), ConsoleTest(char *in, char *out);
extern void MailTest(int networkID);

//----------------------------------------------------------------------
// main
// 	Bootstrap the operating system kernel.  
//	
//	Check command line arguments
//	Initialize data structures
//	(optionally) Call test procedure
//
//	"argc" is the number of arguments 
//	"argv" is an array of strings
//----------------------------------------------------------------------

int
main(int argc, char **argv)
{
    int argCount;			

    DEBUG('t', "Entering main");
    (void) Initialize(argc, argv);

#ifdef THREADS
    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
        argCount = 1;
        switch (argv[0][1]) {
            case 'q': // 指定测试编号
                testnum = atoi(argv[1]);
                argCount++;
                break;
            case 't': // 指定创建线程数量
                threadnum = atoi(argv[1]);
                argCount++;
                break;
            case 'n': // 指定每个线程链表元素操作次数
                oprnum = atoi(argv[1]);
                argCount++;
                break;
            case 'y': // 开启/关闭强制线程切换
                yield_flag = (bool)atoi(argv[1]);
                argCount++;
                break;
            case 's':
            case 'S':    // 大小写都支持
                S = atoi(argv[1]);
                argCount++;
                break;
            default:    // 错误场景选择 1~4
                testnum = 0;
                threadnum = 2;
                oprnum = 5;
                yield_flag = false;
                S = 0;
                break;
        }
    }
    ThreadTest();
#endif

    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
        argCount = 1;
        if (!strcmp(*argv, "-z"))               
            printf (copyright);
#ifdef USER_PROGRAM
        if (!strcmp(*argv, "-x")) {        	
            ASSERT(argc > 1);
            StartProcess(*(argv + 1));
            argCount = 2;
        } else if (!strcmp(*argv, "-c")) {      
            if (argc == 1)
                ConsoleTest(NULL, NULL);
            else {
                ASSERT(argc > 2);
                ConsoleTest(*(argv + 1), *(argv + 2));
                argCount = 3;
            }
            interrupt->Halt();		
        }
#endif 
#ifdef FILESYS
        if (!strcmp(*argv, "-cp")) { 		
            ASSERT(argc > 2);
            Copy(*(argv + 1), *(argv + 2));
            argCount = 3;
        } else if (!strcmp(*argv, "-p")) {	
            ASSERT(argc > 1);
            Print(*(argv + 1));
            argCount = 2;
        } else if (!strcmp(*argv, "-r")) {	
            ASSERT(argc > 1);
            fileSystem->Remove(*(argv + 1));
            argCount = 2;
        } else if (!strcmp(*argv, "-l")) {	
            fileSystem->List();
        } else if (!strcmp(*argv, "-D")) {	
            fileSystem->Print();
        } else if (!strcmp(*argv, "-t")) {	
            PerformanceTest();
        }
#endif 
#ifdef NETWORK
        if (!strcmp(*argv, "-o")) {
            ASSERT(argc > 1);
            Delay(2); 			 
            MailTest(atoi(*(argv + 1)));
            argCount = 2;
        }
#endif 
    }

    currentThread->Finish();	
    return(0);			
}


