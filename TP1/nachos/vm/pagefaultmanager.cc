/*! \file pagefaultmanager.cc
Routines for the page fault managerPage Fault Manager
*/
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//

#include "kernel/thread.h"
#include "vm/swapManager.h"
#include "vm/physMem.h"
#include "vm/pagefaultmanager.h"

PageFaultManager::PageFaultManager() {
}

// PageFaultManager::~PageFaultManager()
/*! Nothing for now
*/
PageFaultManager::~PageFaultManager() {
}

// ExceptionType PageFault(int virtualPage)
/*! 	
//	This method is called by the Memory Management Unit when there is a 
//      page fault. This method loads the page from :
//      - read-only sections (text,rodata) $\Rightarrow$ executive
//        file
//      - read/write sections (data,...) $\Rightarrow$ executive
//        file (1st time only), or swap file
//      - anonymous mappings (stack/bss) $\Rightarrow$ new
//        page from the MemoryManager (1st time only), or swap file
//
//	\param virtualPage the virtual page subject to the page fault
//	  (supposed to be between 0 and the
//        size of the address space, and supposed to correspond to a
//        page mapped to something [code/data/bss/...])
//	\return the exception (generally the NO_EXCEPTION constant)
*/  
/*
	Penser à gérer l'écriture dans une page en lecture seule !!!!!!!!!!!!!!!!!!!!!!!!!!
*/
ExceptionType PageFaultManager::PageFault(int virtualPage) 
{

#ifndef ETUDIANTS_TP

  	printf("**** Warning: page fault manager is not implemented yet\n");
    exit(-1);
    return ((ExceptionType)0);
#endif
#ifdef ETUDIANTS_TP

	/* Chargement à effectuer suite à un défaut de page */
	
	Process *process = g_current_thread->GetProcessOwner();
	AddrSpace *addrspace = process->addrspace;
	TranslationTable *tableTrad = addrspace->translationTable;

	int diskAddr = tableTrad->getAddrDisk(virtualPage), taillePages = g_cfg->PageSize;
	char tmpPage[taillePages];
	int addrPhys; // Adresse où on chargera la page en RAM
	
	DEBUG('m', (char*)"Demande de la page %i à l'adresse virtuelle %x.\n", virtualPage, virtualPage*taillePages);
	
	// Gestion du bit IO
	if(tableTrad->getBitIo(virtualPage)){
	
		while(tableTrad->getBitIo(virtualPage)){
	
			g_current_thread->Yield();
		}
		
		return (NO_EXCEPTION); // Faux
	}
	
	tableTrad->setBitIo(virtualPage);
	
	// Page pas dans le swap
	if (!tableTrad->getBitSwap(virtualPage)){
	
		// Page anonyme 	=> Mise à 0 de la page temporaire
		if(diskAddr == -1){
			
			DEBUG('m', (char*)"Allocation et mise à 0 de %d octets d'une page anonyme.\n", taillePages);
			memset(tmpPage, 0x0, taillePages);
		}
	
		// Page sur disque 	=> Chargement depuis l'exécutable
		else{
		
			// Problème I/O ?
			if(process->exec_file->ReadAt(tmpPage, taillePages, diskAddr) != taillePages){
			
				DEBUG('m', (char*)"Erreur lors de la lecture dans l'exécutable.\n");
				return (PAGEFAULT_EXCEPTION);
			}
			else{
			
				DEBUG('m', (char*)"Lecture d'une page de %d octets dans l'exécutable depuis l'adresse disque 0x%x\n", taillePages, diskAddr);
			}
		}
	}
	// Page dans le swap
	else {

		g_swap_manager->GetPageSwap(diskAddr, tmpPage);  
	}
	
	// Récupération d'une page physique libre
	addrPhys = g_physical_mem_manager->AddPhysicalToVirtualMapping(addrspace, virtualPage);
	
	// Pas de retour d'erreur possible avec memcpy
	memcpy(&(g_machine->mainMemory[addrPhys*taillePages]), tmpPage, taillePages);
	
	// Déverrouillage de la page physique + Page virtuelle valide et située en RAM à addrPhys + I/O à 0
	tableTrad->setPhysicalPage(virtualPage, addrPhys);
	tableTrad->setBitValid(virtualPage);
	g_physical_mem_manager->UnlockPage(addrPhys);
	tableTrad->clearBitIo(virtualPage);
	
	return (NO_EXCEPTION);
#endif
}
