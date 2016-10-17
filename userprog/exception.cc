// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

void sysCallExec();
void sysCallHalt();
void sysCallExit();
void sysCallFork();
void sysCallJoin();
void sysCallCreate();
void sysCallOpen();
void sysCallClose();
void sysCallRead();
void sysCallWrite();
void incrementPC();

#ifdef USE_TLB

//----------------------------------------------------------------------
// HandleTLBFault
//      Called on TLB fault. Note that this is not necessarily a page
//      fault. Referenced page may be in memory.
//
//      If free slot in TLB, fill in translation info for page referenced.
//
//      Otherwise, select TLB slot at random and overwrite with translation
//      info for page referenced.
//
//----------------------------------------------------------------------


void
HandleTLBFault(int vaddr)
{
  int vpn = vaddr / PageSize;
  int victim = Random() % TLBSize;
  int i;

  stats->numTLBFaults++;

  // First, see if free TLB slot
  for (i=0; i<TLBSize; i++)
    if (machine->tlb[i].valid == false) {
      victim = i;
      break;
    }

  // Otherwise clobber random slot in TLB

  machine->tlb[victim].virtualPage = vpn;
  machine->tlb[victim].physicalPage = vpn; // Explicitly assumes 1-1 mapping
  machine->tlb[victim].valid = true;
  machine->tlb[victim].dirty = false;
  machine->tlb[victim].use = false;
  machine->tlb[victim].readOnly = false;
}

#endif

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which) {
      case SyscallException:
      	switch (type) {
      	  case SC_Halt:
            sysCallHalt();
            break;
          case SC_Exit:
            sysCallExit();
            break;
          case SC_Exec:
            sysCallExec();
            break;
          case SC_Join:
            sysCallJoin();
            break;
          case SC_Create:
            sysCallCreate();
            break;
          case SC_Open:
            sysCallOpen();
            break;
          case SC_Read:
            sysCallRead();
            break;
          case SC_Write:
            sysCallWrite();
            break;
          case SC_Close:
            sysCallClose();
            break;
          case SC_Fork:
            sysCallFork();
            break;
          default:
      	    printf("Undefined SYSCALL %d\n", type);
      	    ASSERT(false);
      	}

      #ifdef USE_TLB
            case PageFaultException:
      	HandleTLBFault(machine->ReadRegister(BadVAddrReg));
      	break;
      #endif

      default: 
        printf("Unexpected exception caught for user mode %d %d\n",which,type);
        break;
    }
}


void sysCallHalt(){
  DEBUG('a', "Shutdown, initiated by user program.\n");
  interrupt->Halt();
}

void sysCallExit(){
  DEBUG('a', "Exit, initiated by user program.\n");
}

void sysCallJoin(){
  DEBUG('a', "Joining, initiated by user program.\n");
}

void sysCallCreate(){
  DEBUG('a', "Create, initiated by user program.\n");
   // Grab kernel memory sufficient to hold argument string. Note that
   // this imposes a restriction on the length of the string.
  int whence;
  char *stringarg;
  stringarg = new(std::nothrow) char[128];                                         

  // TOTALLY BOGUS. Just want to extract argument string from
  // user-mode address space under the assumption that the
  // memory map is the identity mapping and all pages are
  // RAM-resident.

    whence = machine->ReadRegister(4); // whence is VIRTUAL address
                                       //   of first byte of arg string.
                                       //   IN THIS CASE, virtual=physical.

    fprintf(stderr,"String starts at address %d in user VAS\n", whence);

  // Copy the string from user-land to kernel-land.

    for (int i=0; i<127; i++)
      if ((stringarg[i]=machine->mainMemory[whence++]) == '\0') break;
    stringarg[127]='\0';               // Effectively truncates a string
                                       //   if it's too long. Better,
                                       //   get string length and error
                                       //   before copy if too long.

    fprintf(stderr, "Argument string is <%s>\n",stringarg);

    delete [] stringarg;               // No memory leaks.

  // Not returning, so no PC patch-up needed.

    interrupt->Halt();
}

void sysCallOpen(){
  DEBUG('a', "Open, initiated by user program.\n");
  int whence;
  char *stringarg;
  stringarg = new(std::nothrow) char[128];                                         

  // TOTALLY BOGUS. Just want to extract argument string from
  // user-mode address space under the assumption that the
  // memory map is the identity mapping and all pages are
  // RAM-resident.

    whence = machine->ReadRegister(4); // whence is VIRTUAL address
                                       //   of first byte of arg string.
                                       //   IN THIS CASE, virtual=physical.

    fprintf(stderr,"String starts at address %d in user VAS\n", whence);

  // Copy the string from user-land to kernel-land.

    for (int i=0; i<127; i++)
      if ((stringarg[i]=machine->mainMemory[whence++]) == '\0') break;
    stringarg[127]='\0';               // Effectively truncates a string
                                       //   if it's too long. Better,
                                       //   get string length and error
                                       //   before copy if too long.

    fprintf(stderr, "Argument string is <%s>\n",stringarg);

    OpenFile *file = fileSystem->Open(stringarg);

    if(file == NULL) {
      fprintf(stderr, "%s\n", "no file found");
      interrupt->Halt();
    }
    else {
      fprintf(stderr, "%s\n", "Looks like it worked");
    }

    //machine->WriteRegister(2, file);

    delete [] stringarg;               // No memory leaks.

  // Not returning, so no PC patch-up needed.

    
}

void sysCallRead(){
  DEBUG('a', "Read, initiated by user program.\n");
  char *buffer = (char *)machine->ReadRegister(4);
  int size = machine->ReadRegister(5);
  OpenFileId id = machine->ReadRegister(6);

  incrementPC();
}

void sysCallWrite(){
  DEBUG('a', "Write, initiated by user program.\n");
  char *buffer = (char *)machine->ReadRegister(4);
  int size = machine->ReadRegister(5);
  OpenFileId id = machine->ReadRegister(6);

  incrementPC();
}

void sysCallClose(){
  DEBUG('a', "Close, initiated by user program.\n");
  int fd;

  fd = machine->ReadRegister(4);


}

void sysCallFork(){
  DEBUG('a', "Fork, initiated by user program.\n");
}

void sysCallExec(){
  DEBUG('a', "Execute, initiated by user program.\n");
}

void incrementPC()
{
  int tmp;
  tmp = machine->ReadRegister(PCReg);
  machine->WriteRegister(PrevPCReg, tmp);
  tmp = machine->ReadRegister(NextPCReg);
  machine->WriteRegister(PCReg, tmp);
  tmp += 4;
  machine->WriteRegister(NextPCReg, tmp);
}