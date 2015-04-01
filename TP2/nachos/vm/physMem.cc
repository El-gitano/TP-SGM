//-----------------------------------------------------------------
/*! \file mem.cc
//  \brief Routines for the physical page management
*/
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//-----------------------------------------------------------------

#include <unistd.h>
#include "vm/physMem.h"

//-----------------------------------------------------------------
// PhysicalMemManager::PhysicalMemManager
//
/*! Constructor. It simply clears all the page flags and inserts them in the
// free_page_list to indicate that the physical pages are free
*/
//-----------------------------------------------------------------
PhysicalMemManager::PhysicalMemManager() {

  long i;

  tpr = new struct tpr_c[g_cfg->NumPhysPages];

  for (i=0;i<g_cfg->NumPhysPages;i++) {
    tpr[i].free=true;
    tpr[i].locked=false;
    tpr[i].owner=NULL;
    free_page_list.Append((void*)i);
  }
  i_clock=-1;
}

PhysicalMemManager::~PhysicalMemManager() {

  delete[] tpr;
}

//-----------------------------------------------------------------
// PhysicalMemManager::RemovePhysicalToVitualMapping
//
/*! This method releases an unused physical page by clearing the
//  corresponding bit in the page_flags bitmap structure, and adding
//  it in the free_page_list.
//
//  \param num_page is the number of the real page to free
*/
//-----------------------------------------------------------------
void PhysicalMemManager::RemovePhysicalToVirtualMapping(long num_page) {
  
  // Check that the page is not already free 
  ASSERT(!tpr[num_page].free);

  // Update the physical page table entry
  tpr[num_page].free=true;
  tpr[num_page].locked=false;
  if (tpr[num_page].owner->translationTable!=NULL) 
    tpr[num_page].owner->translationTable->clearBitValid(tpr[num_page].virtualPage);

  // Insert the page in the free list
  free_page_list.Prepend((void*)num_page);
}

//-----------------------------------------------------------------
// PhysicalMemManager::UnlockPage
//
/*! This method unlocks the page numPage, after
//  checking the page is in the locked state. Used
//  by the page fault manager to unlock at the
//  end of a page fault (the page cannot be evicted until
//  the page fault handler terminates).
//
//  \param num_page is the number of the real page to unlock
*/
//-----------------------------------------------------------------
void PhysicalMemManager::UnlockPage(long num_page) {
  ASSERT(num_page<g_cfg->NumPhysPages);
  ASSERT(tpr[num_page].locked==true);
  ASSERT(tpr[num_page].free==false);
  tpr[num_page].locked = false;
}

//-----------------------------------------------------------------
// PhysicalMemManager::ChangeOwner
//
/*! Change the owner of a page
//
//  \param owner is a pointer on new owner (Thread *)
//  \param numPage is the concerned page
*/
//-----------------------------------------------------------------
void PhysicalMemManager::ChangeOwner(long numPage, Thread* owner) {
  // Update statistics
  g_current_thread->GetProcessOwner()->stat->incrMemoryAccess();
  // Change the page owner
  tpr[numPage].owner = owner->GetProcessOwner()->addrspace;
}

//-----------------------------------------------------------------
// PhysicalMemManager::AddPhysicalToVirtualMapping 
//
/*! This method returns a new physical page number. If there is no
//  page available, it evicts one page (page replacement algorithm).
//
//  NB: this method locks the newly allocated physical page such that
//      it is not stolen during the page fault resolution. Don't forget
//      to unlock it
//
//  \param owner address space (for backlink)
//  \param virtualPage is the number of virtualPage to link with physical page
//  \return A new physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::AddPhysicalToVirtualMapping(AddrSpace* owner,int virtualPage) 
{
  int page;

  // First try to find a free page
  page=FindFreePage();  

  // If there is no free page, call the replacement algorithm
  if (page==-1) {
  	
  	DEBUG('v',(char *)"Pas de page libre, appel au voleur de pages\n", virtualPage, page);  
  	page=EvictPage();
  }  // no free page : clock

  // Update the physical page table
  tpr[page].virtualPage=virtualPage;
  tpr[page].owner=owner;
  tpr[page].locked=true; // Lock the page until it is filled in, it could then get evicted
  DEBUG('v',(char *)"AddPhysicalToVirtualMapping, virtual : %i, real : %i\n",virtualPage,page);  
  return page;
}

//-----------------------------------------------------------------
// PhysicalMemManager::FindFreePage
//
/*! This method returns a new physical page number, if it finds one
//  free. If not, return -1. Does not run the clock algorithm.
//
//  \return A new free physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::FindFreePage() {
  int64_t page;

  // Check that the free list is not empty
  if (free_page_list.IsEmpty())
    return -1;

  // Update statistics
  g_current_thread->GetProcessOwner()->stat->incrMemoryAccess();
  
  // Get a page from the free list
  page = (int64_t)free_page_list.Remove();
  
  // Check that the page is really free
  ASSERT(tpr[page].free);
  
  // Update the physical page table
  tpr[page].free = false;

  return page;
}

//-----------------------------------------------------------------
// PhysicalMemManager::EvictPage
//
/*! This method implements page replacement, using the well-known
//  clock algorithm.
//
//  \return A new free physical page number.
*/
//-----------------------------------------------------------------
#ifndef ETUDIANTS_TP
int PhysicalMemManager::EvictPage() {
  printf("**** Warning: page replacement algorithm is not implemented yet\n");
    exit(-1);
    return (0);
}
#endif
#ifdef ETUDIANTS_TP
int PhysicalMemManager::EvictPage() {

	int local_i_clock = i_clock, nbPagesParcourues = 0, pageVirtuelle, numSecteur;
	bool trouve = false;
	tpr_c pageReelle;
	TranslationTable *tableTrans;
	OpenFile *fichierMap = NULL;
	char *adressePage = NULL;
	
	// On parcourt l'ensemble des pages jusqu'à ce qu'on trouve une page libre
	while(!trouve){
	
		local_i_clock = (local_i_clock+1)%(g_cfg->NumPhysPages);
		
		pageReelle = tpr[local_i_clock];
		pageVirtuelle = pageReelle.virtualPage;
		tableTrans = pageReelle.owner->translationTable;
		
		// On a parcouru toutes les pages physiques mais on a rien trouvé -> Mise en attente
		if(nbPagesParcourues == g_cfg->NumPhysPages){
	
			i_clock = local_i_clock;
			
			g_current_thread->Yield();
			
			local_i_clock = (i_clock+1)%(g_cfg->NumPhysPages);
			nbPagesParcourues = 0;
		}
		
		if(!pageReelle.locked){
		
			// Page disponible
			if(!tableTrans->getBitU(pageVirtuelle)){
		
				trouve = true;
			}
		
			// Page référencéee récemment (2ème chance)
			else{
		
				tableTrans->clearBitU(pageVirtuelle);
			}
		}
		
		nbPagesParcourues++; // Déplacé depuis le début
	}
	
	i_clock = local_i_clock;
	pageReelle.locked = true; // Pour éviter la réquisition de cette page si on perd la main lors du traitement avec le swap
	//tableTrans->clearBitValid(pageVirtuelle);
	
	DEBUG('v', (char*)"Page physique n°%i a voler\n", local_i_clock);
		
	// Traitement sur la page avant de la rendre (recopie dans le swap en cas de modification)
	if(tableTrans->getBitM(pageVirtuelle)){
		
		adressePage = (char*)(&(g_machine->mainMemory[local_i_clock*g_cfg->PageSize]));
		
		// Cette page a déjà un secteur associé dans le swap
		if(tableTrans->getBitSwap(pageVirtuelle)){
		
			DEBUG('v', (char*)"La page a déjà une adresse dans le swap (%i)\n", tableTrans->getAddrDisk(pageVirtuelle));
			g_swap_manager->PutPageSwap(tableTrans->getAddrDisk(pageVirtuelle), adressePage);
		}
		// Page correspondant à un fichier mappé
		else if( (fichierMap = pageReelle.owner->findMappedFile(pageVirtuelle)) != NULL){
		
			fichierMap->WriteAt(adressePage, g_cfg->PageSize, tableTrans->getAddrDisk(pageVirtuelle));
		}
		else{
		
			DEBUG('v', (char*)"La page n'a pas d'adresse dans le swap\n");
			numSecteur = g_swap_manager->PutPageSwap(-1, (char*)(&(g_machine->mainMemory[local_i_clock*g_cfg->PageSize])));
			DEBUG('v', (char*)"Page swap associée : %i\n", numSecteur);
			
			tableTrans->setAddrDisk(pageVirtuelle, numSecteur);
			tableTrans->setBitSwap(pageVirtuelle);
		}
	}
	
	return local_i_clock;
}
#endif
//-----------------------------------------------------------------
// PhysicalMemManager::Print
//
/*! print the current status of the table of physical pages
//
//  \param rpage number of real page
*/
//-----------------------------------------------------------------

void PhysicalMemManager::Print(void) {
  int i;

  printf("Contents of TPR (%d pages)\n",g_cfg->NumPhysPages);
  for (i=0;i<g_cfg->NumPhysPages;i++) {
    printf("Page %d free=%d locked=%d virtpage=%d owner=%lx U=%d M=%d\n",
	   i,
	   tpr[i].free,
	   tpr[i].locked,
	   tpr[i].virtualPage,
	   (long int)tpr[i].owner,
	   (tpr[i].owner!=NULL) ? tpr[i].owner->translationTable->getBitU(tpr[i].virtualPage) : 0,
	   (tpr[i].owner!=NULL) ? tpr[i].owner->translationTable->getBitM(tpr[i].virtualPage) : 0);
  }
}
