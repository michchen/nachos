// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include <new>

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

AddrSpace::AddrSpace(AddrSpace *parentData){
    numPages = parentData->numPages;
    int size = parentData->getNumPages() * PageSize;
    int bitmapAddr;
    pageTable = new(std::nothrow) TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;   // for now, virtual page # = phys page #
        bitmapAddr = pagemap->Find();
        ASSERT(bitmapAddr != -1);
        pageTable[i].physicalPage = bitmapAddr;
        pageTable[i].valid = true;
        pageTable[i].use = false;
        pageTable[i].dirty = false;
        pageTable[i].readOnly = false;  // if the code segment was entirely on 
                        // a separate page, we could set its 
                        // pages to be read-only
    }

    int virtaddr;
    int pvirtaddr;
    int addrtrans;
    TranslationEntry *parentTable = parentData->getPageTable();    
    
    for (int j = 0; j < size; j++ ) {
        virtaddr = pageTable[j].virtualPage;
        pvirtaddr = parentTable[j].virtualPage;
        machine->mainMemory[AddrTranslation(virtaddr)] = machine->mainMemory[AddrTranslation(pvirtaddr)];
    }
    //inherit openfiles as well

    

}
//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int size;
#ifndef USE_TLB
    unsigned int i;
#endif

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
    int bitmapAddr;
#ifndef USE_TLB
// first, set up the translation 
    pageTable = new(std::nothrow) TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
    	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
        bitmapAddr = pagemap->Find();
        ASSERT(bitmapAddr != -1);
    	pageTable[i].physicalPage = bitmapAddr;
    	pageTable[i].valid = true;
    	pageTable[i].use = false;
    	pageTable[i].dirty = false;
    	pageTable[i].readOnly = false;  // if the code segment was entirely on 
    					// a separate page, we could set its 
    					// pages to be read-only
    }
#endif    

// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    //bzero(machine->mainMemory, size);
    for (int j = 0; j < numPages; j++ ) {
        bzero(&(machine->mainMemory[pageTable[i].physicalPage * PageSize]), PageSize);
    }

    int virtaddr;
    int addrtrans;
// then, copy in the code and data segments into memory
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
        virtaddr = noffH.code.virtualAddr;
        while (virtaddr < (noffH.code.size + noffH.code.virtualAddr)) {
            addrtrans = AddrTranslation(virtaddr);
            executable->ReadAt(&(machine->mainMemory[addrtrans]),
			     1, noffH.code.inFileAddr + (virtaddr - noffH.code.virtualAddr));
            virtaddr++;
        }
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
        virtaddr = noffH.code.virtualAddr;
        while (virtaddr < (noffH.initData.size + noffH.initData.virtualAddr)) {
            addrtrans = AddrTranslation(virtaddr);
            executable->ReadAt(&(machine->mainMemory[addrtrans]),
			1, noffH.initData.inFileAddr + (virtaddr-noffH.initData.virtualAddr));
            virtaddr++;
        }
    }

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
#ifndef USE_TLB
   delete pageTable;
#endif
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table,
//      IF address translation is done with a page table instead
//      of a hardware TLB.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
#ifndef USE_TLB
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
#endif
}


unsigned int AddrSpace::AddrTranslation(int virtAddr)
{
    unsigned int vpn, offset;
    TranslationEntry *entry;
    unsigned int pageFrame;
    unsigned int physaddr;

    vpn = (unsigned) virtAddr / PageSize;
    offset = (unsigned) virtAddr % PageSize;

    if (vpn >= machine->pageTableSize) {
        DEBUG('a', "virtual page # %d too large for page table size %d!\n", 
            virtAddr, machine->pageTableSize);
        return -1;
    } else if (!pageTable[vpn].valid) {
        DEBUG('a', "Page table miss, virtual address  %d!\n", 
            virtAddr);
        return -1;
    }
    entry = &pageTable[vpn];

    pageFrame = entry->physicalPage;

    if (pageFrame >= NumPhysPages) { 
        DEBUG('a', "*** frame %d > %d!\n", pageFrame, NumPhysPages);
        return -1;
    }

    physaddr = pageFrame * PageSize + offset;
    return physaddr;
}

void
AddrSpace::ExecFunc(OpenFile *executable) {
    for (int i = 0; i < numPages; i++) {
        pagemap->Clear(pageTable[i].physicalPage);
    }

    NoffHeader noffH;
    unsigned int size;
#ifndef USE_TLB
    unsigned int i;
#endif

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
        (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
            + UserStackSize;    // we need to increase the size
                        // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    ASSERT(numPages <= NumPhysPages);       // check we're not trying
                        // to run anything too big --
                        // at least until we have
                        // virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
                    numPages, size);
    int bitmapAddr;

    for (i = 0; i < numPages; i++) {  // for now, virtual page # = phys page #
        bitmapAddr = pagemap->Find();
        ASSERT(bitmapAddr != -1);
        pageTable[i].physicalPage = bitmapAddr;
    }

// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    //bzero(machine->mainMemory, size);
    for (int j = 0; j < numPages; j++ ) {
        bzero(&(machine->mainMemory[pageTable[i].physicalPage * PageSize]), PageSize);
    }

    int virtaddr;
    int addrtrans;
// then, copy in the code and data segments into memory
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
            noffH.code.virtualAddr, noffH.code.size);
        virtaddr = noffH.code.virtualAddr;
        while (virtaddr < (noffH.code.size + noffH.code.virtualAddr)) {
            addrtrans = AddrTranslation(virtaddr);
            executable->ReadAt(&(machine->mainMemory[addrtrans]),
                 1, noffH.code.inFileAddr + (virtaddr - noffH.code.virtualAddr));
            virtaddr++;
        }
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
            noffH.initData.virtualAddr, noffH.initData.size);
        virtaddr = noffH.code.virtualAddr;
        while (virtaddr < (noffH.initData.size + noffH.initData.virtualAddr)) {
            addrtrans = AddrTranslation(virtaddr);
            executable->ReadAt(&(machine->mainMemory[addrtrans]),
            1, noffH.initData.inFileAddr + (virtaddr-noffH.initData.virtualAddr));
            virtaddr++;
        }
    }


}