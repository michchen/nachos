// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"
#include <new>

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

void
StartProcess(char *filename)
{
    fprintf(stderr, "%s\n", "Attempting to start the process");
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;
    fprintf(stderr, "%s %s\n", "woop", filename);
    int argvAddr[2]; // part of the hack

    if (executable == NULL) {
	   printf("Unable to open file %s\n", filename);
	return;
    }
    fprintf(stderr, "%s\n", "attempting to create an addrspace");
    space = new(std::nothrow) AddrSpace(executable);    
    currentThread->space = space;
    fprintf(stderr, "%s\n","ok?" );

    fprintf(stderr, "%s\n", "step");
    delete executable;			// close file

    space->InitRegisters();		// set the initial register values
    space->RestoreState();		// load page table register

    int sp = machine->ReadRegister(StackReg);

// Push argv[0] (i.e filename) onto the stack.  Save its address for the 
// array of pointers.

    int len =  strlen(filename) + 1;   // Length including NULL terminator 
    sp -= len; 
    for (int i = 0; i < len; i++) 
        machine->mainMemory[sp + i] = filename[i]; 
    argvAddr[0] = sp;

// Push "Hello World\n"  onto the stack.  Save its address for the 
// array of pointers.

    len = strlen("Hello World\n") + 1; 
    sp -= len; 
    for (int i = 0; i < len; i++) 
        machine->mainMemory[sp + i] = "Hello World\n"[i]; 
    argvAddr[1] = sp;

    sp = sp  & ~3;        // Align stack on 4 byte boundrary for integer values. 
  
// Allocate and fill in the argv array.  Note that the pointers should 
// be in the simulated machine's endian format.

    sp -= sizeof(int) * 2;

    for (int i = 0; i < 2; i++) 
        *(unsigned int *) &machine->mainMemory[sp + i*4] 
                = WordToMachine((unsigned int) argvAddr[i]); 
// Put argc into R4 and the argv pointer in R5

    machine->WriteRegister(4, 2); 
    machine->WriteRegister(5, sp);

// Update the stack pointer so the process starts below the argc/argv stuff.

    fprintf(stderr, "%s %d\n", "here is what I've written", sp);

    machine->WriteRegister(StackReg, sp - 8); 

    printf("%s\n", "here we go");
    machine->Run();			// jump to the user progam
    ASSERT(false);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int) { readAvail->V(); }
static void WriteDone(int) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new(std::nothrow) Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new(std::nothrow) Semaphore("read avail", 0);
    writeDone = new(std::nothrow) Semaphore("write done", 0);
    
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }
}
