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

#ifdef CHANGED
void
ExceptionHandler(ExceptionType which)
{

    int type = machine->ReadRegister(2);

    fprintf(stderr, "%s %d %d\n", "Here is the type", type, which);

    int totalThreads;
    //fprintf(stderr, "%s %d\n", "Here is the type", type);

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
            break;
        }
      break;
      #ifdef USE_TLB
            case PageFaultException:
      	HandleTLBFault(machine->ReadRegister(BadVAddrReg));
      	break;
      #endif

      default: 
        printf("Unexpected exception caught for user mode %d %d\n",which,type);
        interrupt->Halt();
        break;
    }
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

void sysCallHalt(){
  DEBUG('a', "Shutdown, initiated by user program.\n");
  interrupt->Halt();
}

void sysCallExit(){
  DEBUG('a', "Exit, initiated by user program.\n");
  int result = machine->ReadRegister(4);
  int threadID = currentThread->getThreadId();
  processMonitor->lock();
  if(processMonitor->setExitStatus(threadID,result)){
    processMonitor->wakeParent(threadID);
    processMonitor->unlock();
  }
  else{
    processMonitor->unlock();
    DEBUG('a',"Thread does not exist");
    ASSERT(false);
  }
  currentThread->Finish();
}

void sysCallJoin(){
  DEBUG('a', "Joining, initiated by user program.\n");
  printf("%s\n","Joining Called" );
  int result = machine->ReadRegister(4);
  int exitStatus; 
  int threadID = currentThread->getThreadId();
  processMonitor->lock();
  if(processMonitor->containsThread(threadID)){
    processMonitor->sleepParent(threadID);
    exitStatus = processMonitor->getExitStatus(threadID); 
  }
  processMonitor->unlock();
  if(exitStatus == -1){
    DEBUG('a', "exitStatus is -1");
  }
  else{
    machine->WriteRegister(2,exitStatus);
  }

  incrementPC();
}

void sysCallCreate(){
  DEBUG('a', "Create, initiated by user program.\n");
   // Grab kernel memory sufficient to hold argument string. Note that
   // this imposes a restriction on the length of the string.
  int whence;
  char *stringarg;
  stringarg = new(std::nothrow) char[128];                                         

  whence = machine->ReadRegister(4); // whence is VIRTUAL address
                                     //   of first byte of arg string.
                                     //   IN THIS CASE, virtual=physical.

// Copy the string from user-land to kernel-land.

  ASSERT(currentThread->space != NULL);
  for (int i=0; i<127; i++) {
    if ((stringarg[i]=machine->mainMemory[currentThread->space->AddrTranslation(whence)]) == '\0') break;
    whence++;
  }
  stringarg[127]='\0';              // Effectively truncates a string
                                     //   if it's too long. Better,
                                     //   get string length and error
                                       //   before copy if too long.


    bool result = fileSystem->Create(stringarg,1);

    if(result == false) {
      DEBUG('a', "%s\n", "issue creating the file");
    }
    delete [] stringarg;               // No memory leaks.
    
    incrementPC();

}

void sysCallOpen(){
  DEBUG('a', "Open, initiated by user program.\n");
  int whence;
  char *stringarg;
  stringarg = new(std::nothrow) char[128];
  int fd;                                       

    whence = machine->ReadRegister(4); // whence is VIRTUAL address
                                       //   of first byte of arg string.
                                       //   IN THIS CASE, virtual=physical.


  // Copy the string from user-land to kernel-land.

  ASSERT(currentThread->space != NULL);
  for (int i=0; i<127; i++) {
    if ((stringarg[i]=machine->mainMemory[currentThread->space->AddrTranslation(whence)]) == '\0') break;
    whence++;
  }
  stringarg[127]='\0';              // Effectively truncates a string
                                       //   if it's too long. Better,
                                       //   get string length and error
                                       //   before copy if too long.


    OpenFile *file = fileSystem->Open(stringarg);

    if(file == NULL) {
      DEBUG('a', "%s\n", "no file found");
      fd = -1;
    }
    else {
      fd = currentThread->AddFile(file);
      if (fd == -1){
        DEBUG('a', "%s\n", "Failed to add file to openfile array");
      }
    }

    machine->WriteRegister(2, fd);

    incrementPC();

    

    delete [] stringarg;               // No memory leaks.
    
}

void sysCallRead(){
  DEBUG('a', "Read, initiated by user program.\n");
  int bufStart = machine->ReadRegister(4);
  int size = machine->ReadRegister(5);
  OpenFileId id = machine->ReadRegister(6);
  char *stringarg;
  stringarg = new(std::nothrow) char[size];  
  int result = -1;

  if (id == 1) {
    DEBUG('a', "%s\n", "Can't read from stdout");
  }
  else if (id == 0) {
      result = synchcon->Read(stringarg, size);
      if (result == 0) {
        DEBUG('a', "%s\n", "read failed");
        result = -1;
      }
      for(int i = 0; i<size; i++) {
        machine->mainMemory[bufStart+i] = stringarg[i];
      }

  }
  else {
    OpenFile* file = currentThread->GetFile(id);
    if (file != NULL) {
      result = file->Read(stringarg, size);
      for(int i = 0; i<size; i++) {
          machine->mainMemory[bufStart+i] = stringarg[i];
      }
    }
    
  }

  if (result == -1) {
    DEBUG('a', "%s\n", "Error reading");
  }

  machine->WriteRegister(2, result);

  incrementPC();
  delete [] stringarg; 
}

void sysCallWrite(){
  DEBUG('a', "Write, initiated by user program.\n");
  int bufStart = machine->ReadRegister(4);
  int size = machine->ReadRegister(5);
  OpenFileId id = machine->ReadRegister(6);
  char *stringarg;
  stringarg = new(std::nothrow) char[128];  

  ASSERT(currentThread->space != NULL);
  for (int i=0; i<size; i++) {
    if ((stringarg[i]=machine->mainMemory[currentThread->space->AddrTranslation(bufStart)]) == '\0') break;
    bufStart++;
  }
  stringarg[127]='\0'; 

 // fprintf(stderr, "%s %s\n", "this is what I get from memory", stringarg);


  if (id == 0) {
    DEBUG('a', "%s\n", "Can't write to stdin");
  }
  else if (id == 1) {
    for (int j = 0; j<size; j++) {
      synchcon->Write(stringarg[j], 1);
    }
  }
  else {
    OpenFile* file = currentThread->GetFile(id);
    file->Write(stringarg, size);
  }


  incrementPC();
}

void sysCallClose(){
  DEBUG('a', "Close, initiated by user program.\n");
  int fd;

  fd = machine->ReadRegister(4);

  OpenFile* result = currentThread->RemoveFile(fd);
  if(result == NULL) {
    DEBUG('a', "%s\n", "error closing a file");
  }
  else {
    delete result;    
  }

  incrementPC();
}
void runMachine(){
  printf("%s\n","Child Called" );
  currentThread->RestoreUserState();
  //currentThread->space->RestoreState();  
  machine->Run();
}
void sysCallFork(){
  DEBUG('a', "Fork, initiated by user program.\n");
  printf("%s\n","Forking Called" );
  Thread *forkedThread = new Thread("Forked Thread");
  //To do copy the parents address space and open files.

  forkedThread->space = new(std::nothrow) AddrSpace(currentThread->space);

  //Todo: What to do with the space id
  //int arg = machine->ReadRegister(4);
  processMonitor->lock();
  int spaceId = processMonitor->addThread(forkedThread, currentThread);
  processMonitor->unlock();

  if(spaceId ==-1){
    DEBUG('a',"CREATION FAILURE");
    return;
  }
  incrementPC();
  currentThread->SaveUserState();

  machine->WriteRegister(2,0);

  forkedThread->SaveUserState();

  forkedThread->Fork((VoidFunctionPtr) runMachine,0);
  printf("%s\n","Parent sleeping" );
  processMonitor->sleepParent(spaceId);

  //Return to parent process
  printf("%s\n","Parent returning" );
  machine->WriteRegister(2,spaceId);


}

//Extra info needed for the system!
// Number of children
// The parent-process id
// The parent is waiting to join?
//Resources 

void sysCallExec(){
  DEBUG('a', "Execute, initiated by user program.\n");

  char *fileName;
  int argStart = machine->ReadRegister(4);
  //AddrSpace *space;
  fileName = new(std::nothrow) char[128];
  int i;
  //Get Filename to open for User Program
  // for (int i=0; i<128; i++)
  //   if ((fileName[i]=machine->mainMemory[argStart++]) == '\0') break;
  // fileName[127]='\0'; 

  fprintf(stderr, "%s %d\n", "doing some stuff", argStart);

  // i = 0;
  // while (fileName[i]=machine->mainMemory[currentThread->space->AddrTranslation(argStart)] != '\0' && i < 127) {
  //     i++;
  //     argStart++;
  //     fprintf(stderr, "%c\n", fileName[i]);
  // }

  ASSERT(currentThread->space != NULL);
  for (int i=0; i<127; i++) {
    if ((fileName[i]=machine->mainMemory[currentThread->space->AddrTranslation(argStart)]) == '\0') break;
    argStart++;
  }

  //Initialize its registers
  fprintf(stderr, "%s %s\n", "about to do the open", fileName);
  OpenFile *exec = fileSystem->Open(fileName);

  //Invoke it through machine running.
  incrementPC();
  if(exec != NULL){

    fprintf(stderr, "%s\n", "yes?");
    currentThread->space->ExecFunc(exec);

    delete exec;    //delete the executable

    currentThread->space->InitRegisters();   // set the initial register values
    currentThread->space->RestoreState();    // load page table register


    // //Create a new Thread
    // Thread *newProcess = new Thread("Executed Program Thread");
    // //Allocate a new address space object for the new Thread.
    // space = new(std::nothrow) AddrSpace(exec);
    // newProcess->space = space;

    // //return the address space identifier to the calling process
    // machine->WriteRegister(2,newProcess->getThreadId());

    // //saveUser state just in case of failure (Maybe we dont need this?)
    // currentThread->SaveUserState();

    // //Prep registers to look like just starting
    // currentThread->space->InitRegisters();
    // currentThread->space->RestoreState();
    // //Run the Process
    // newProcess->Fork(machine->Run(),0);
    // //Inheriting open files from former execution????

    // //Should not have reached here so return failure.
    // currentThread->RestoreUserState();
    // DEBUG('a', "%s\n", "Forking has failed somehow");
    // machine->WriteRegister(2,-1);



  }
  else{
    DEBUG('a', "%s\n", "Error Opening File");
    fprintf(stderr, "%s\n", "oh my god");
    machine->WriteRegister(2,-1);
  }
}
#endif