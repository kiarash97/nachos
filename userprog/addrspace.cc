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
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader(NoffHeader *noffH)
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

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the ion from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int i, size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    // how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize; // we need to increase the size
                                                                                          // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
    numPages++;

    ASSERT(numPages <= NumPhysPages); // check we're not trying
                                      // to run anything too big --
                                      // at least until we have
                                      // virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
          numPages, size);
    // first, set up the translation
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++)
    {
        pageTable[i].virtualPage = i; // for now, virtual page # = phys page #

        int pgNumber = machine->GetFreePhysicalPageNumber();
        ASSERT(pgNumber != -1);
        pageTable[i].physicalPage = pgNumber;

        pageTable[i].valid = TRUE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE; // if the code segment was entirely on
                                       // a separate page, we could set its
                                       // pages to be read-only
    }

    CleanAddrSpace();

    CopySegmentToMemory(noffH.code, executable);
    CopySegmentToMemory(noffH.initData, executable);
}

AddrSpace::AddrSpace(AddrSpace *space)
{
    numPages = space->numPages;
    pageTable = new TranslationEntry[numPages];
    for (int i = 0; i < numPages; i++)
    {
        pageTable[i].virtualPage = i; // for now, virtual page # = phys page #

        int pgNumber = machine->GetFreePhysicalPageNumber();
        ASSERT(pgNumber != -1);
        pageTable[i].physicalPage = pgNumber;

        pageTable[i].valid = TRUE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;
    }

    CleanAddrSpace();
    CopyAddrSpace(space);
}

void AddrSpace::CopyAddrSpace(AddrSpace *space)
{
    for (int i = 0; i < space->numPages; ++i)
    {
        char *src = &machine->mainMemory[space->pageTable[i].physicalPage * PageSize];
        char *dst = &machine->mainMemory[pageTable[i].physicalPage * PageSize];

        memcpy(dst, src, PageSize);
    }
}

void AddrSpace::CopySegmentToMemory(Segment seg, OpenFile *file)
{
    if (seg.size <= 0)
        return;

    int numRequiredPages = divRoundUp(seg.size, PageSize);
    for (int i = 0; i < numRequiredPages - 1; ++i)
    {
        unsigned int virtualAddr = (unsigned)seg.virtualAddr + i * PageSize;
        unsigned int vpn = virtualAddr / PageSize;
        unsigned int offset = virtualAddr % PageSize;

        int physicalAddr = pageTable[vpn].physicalPage * PageSize + offset;
        file->ReadAt(&(machine->mainMemory[physicalAddr]), PageSize, seg.inFileAddr + i * PageSize);
    }

    int i = numRequiredPages - 1;
    unsigned int virtualAddr = (unsigned)seg.virtualAddr + i * PageSize;
    unsigned int vpn = virtualAddr / PageSize;
    unsigned int offset = virtualAddr % PageSize;

    int physicalAddr = pageTable[vpn].physicalPage * PageSize + offset;
    file->ReadAt(&(machine->mainMemory[physicalAddr]), seg.size - i * PageSize, seg.inFileAddr + i * PageSize);
}

void AddrSpace::CleanAddrSpace()
{
    for (int i = 0; i < numPages; ++i)
    {
        int physicalAddr = pageTable[i].physicalPage * PageSize;
        bzero(&machine->mainMemory[physicalAddr], PageSize);
    }
}

void AddrSpace::DumpAddrSpace()
{
    printf("Address Space for thread %s: \n", currentThread->getName());
    for (int i = 0; i < numPages; ++i)
    {
        for (int j = 0; j < PageSize; ++j)
        {
            int virtualAddr = pageTable[i].virtualPage * PageSize + j;
            int addr = pageTable[i].physicalPage * PageSize + j;
            printf("%d -> %d: %.2x\n", virtualAddr, addr, (unsigned char)machine->mainMemory[addr]);
        }
    }
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    FreePhysicalMemory();
    delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::Iniaegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void AddrSpace::InitRegisters()
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

void AddrSpace::InitRegisters(int *registers)
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
        registers[i] = 0;

    // Initial program counter -- must be location of "Start"
    registers[PCReg] = 0;

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    registers[NextPCReg] = 4;

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    registers[StackReg] = numPages * PageSize - 16;
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
{
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

void AddrSpace::FreePhysicalMemory()
{
    for (int i = 0; i < numPages; ++i)
        if (pageTable[i].valid)
        {
            machine->AddFreePhysicalPageNumber(pageTable[i].physicalPage);
            pageTable[i].valid = FALSE;
        }
}

void AddrSpace::PrintPageTable()
{
    printf("%s addrsps\n", currentThread->getName());
    for (int i = 0; i < numPages; ++i)
    {
        printf("v: %d -> %d %d \t", pageTable[i].virtualPage, pageTable[i].physicalPage, pageTable[i].valid);
    }
    printf("\n");
}