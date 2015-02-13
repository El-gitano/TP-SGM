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
	
	
	// chargement de la page manquante
	char tmpPage[g_cfg->PageSize];
	int tmpAddr = process->addrspace->translationTable->getAddrDisk(virtualPage);
	
	// Page pas dans le swap
	if (!process->addrspace->translationTable->getBitSwap(virtualPage)){
	
		// Page anonyme
		if( tmpAddr ==-1){
			
			memset(tmpPage, 0, g_cfg->PageSize);
		}
	
		// Page sur disque
		else{
		
			// Problème I/O ?
			if(g_current_thread->process->exec_file->ReadAt(tmpPage, g_cfg->PageSize, tmpAddr) != g_cfg->PageSize){
			
				DEBUG('m', (char*)"Erreur lors de la lecture de la page.\n");
				return (PAGEFAULT_EXCEPTION);
			}
		}
	}
	// Page dans le swap
	else {
		g_swap_manager->GetPageSwap(tmpAddr,tmpPage);  
	}
		
	// Recherche d'une page libre 
	// Valeur de retour a vérifier, acces objet !
	int addrPhys = AddPhysicalToVirtualMapping(process->addrspace,virtualPage);
	
	// Pas de retour d'erreur possible
	memcpy(addrPhys,tmpPage, g_cfg->PageSize);
	
	// bit de verrouillage ?
	process->addrspace->translationTable->setBitValid(virtualPage);
	process->addrspace->translationTable->setPhysicalPage(virtualPage, addrPhys);
	// vérifier redondance de AddPhysicalToVirtualMapping
	
	return (NO_EXCEPTION);
#endif
}
