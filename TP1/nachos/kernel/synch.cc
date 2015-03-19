/*! \file synch.cc 
//  \brief Routines for synchronizing threads.  
//
//      Three kinds of synchronization routines are defined here: 
//      semaphores, locks and condition variables.
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation. We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts. While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
*/
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.


#include "kernel/system.h"
#include "kernel/scheduler.h"
#include "kernel/synch.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
/*! 	Initializes a semaphore, so that it can be used for synchronization.
//
// \param debugName is an arbitrary name, useful for debugging only.
// \param initialValue is the initial value of the semaphore.
*/
//----------------------------------------------------------------------
Semaphore::Semaphore(char* debugName, int initialValue)
{
  name = new char[strlen(debugName)+1];
  strcpy(name,debugName);
  value = initialValue;
  queue = new Listint;
  typeId = SEMAPHORE_TYPE_ID;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
/*! 	De-allocates a semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
*/
//----------------------------------------------------------------------
Semaphore::~Semaphore()
{
  typeId = INVALID_TYPE_ID;
  ASSERT(queue->IsEmpty());
  delete [] name;
  delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
/*!
//      Decrement the value, and wait if it becomes < 0. Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void
Semaphore::P() {
  printf("**** Warning: method Semaphore::P is not implemented yet\n");
  exit(-1);
}
#endif
#ifdef ETUDIANTS_TP
void Semaphore::P() {

	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	value--;
	
	if(value < 0){

		DEBUG('s', (char *)"P() bloquant (sem %s) par le thread %s\n", name, g_current_thread->GetName());
		Thread *courant = g_current_thread;
		queue->Append(courant);
		DEBUG('s', (char *)"P() test queue vide (sem %s) après ajout du thread : %i\n", name, queue->IsEmpty());
		courant->Sleep();
	}
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif


//----------------------------------------------------------------------
// Semaphore::V
/*! 	Increment semaphore value, waking up a waiting thread if any.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that interrupts
//	are disabled when it is called.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void
Semaphore::V() {
   printf("**** Warning: method Semaphore::V is not implemented yet\n");
    exit(-1);
}
#endif
#ifdef ETUDIANTS_TP
void
Semaphore::V() {

	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	value++;
	DEBUG('s', (char *)"V() (sem %s) test queue vide par le thread %s : %i\n", name, g_current_thread->GetName(), queue->IsEmpty());
	//On relance le premier thread de la file d'attente
	if(!queue->IsEmpty()){

		Thread *temp = (Thread *)queue->Remove();
		DEBUG('s', (char *)"V() (sem %s) par le thread %s (resultat %p)\n", name, g_current_thread->GetName(), temp);
		g_scheduler->ReadyToRun(temp);
	}
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif
//----------------------------------------------------------------------
// Lock::Lock
/*! 	Initialize a Lock, so that it can be used for synchronization.
//      The lock is initialy free
//  \param "debugName" is an arbitrary name, useful for debugging.
*/
//----------------------------------------------------------------------
Lock::Lock(char* debugName) {
  name = new char[strlen(debugName)+1];
  strcpy(name,debugName);
  sleepqueue = new Listint;
  free = true;
  owner = NULL;
  typeId = LOCK_TYPE_ID;
}


//----------------------------------------------------------------------
// Lock::~Lock
/*! 	De-allocate lock, when no longer needed. Assumes that no thread
//      is waiting on the lock.
*/
//----------------------------------------------------------------------
Lock::~Lock() {
  typeId = INVALID_TYPE_ID;
  ASSERT(sleepqueue->IsEmpty());
  delete [] name;
  delete sleepqueue;
}

//----------------------------------------------------------------------
// Lock::Acquire
/*! 	Wait until the lock become free.  Checking the
//	state of the lock (free or busy) and modify it must be done
//	atomically, so we need to disable interrupts before checking
//	the value of free.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void Lock::Acquire() {
   printf("**** Warning: method Lock::Acquire is not implemented yet\n");
    exit(-1);
}
#endif
#ifdef ETUDIANTS_TP
void Lock::Acquire() {
  
  	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);

	if(free){
		free = false;
		owner = g_current_thread;
	}
	else{
	
		Thread *courant = g_current_thread;
		sleepqueue->Append(courant);
		courant->Sleep();
	}

	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif

//----------------------------------------------------------------------
// Lock::Release
/*! 	Wake up a waiter if necessary, or release it if no thread is waiting.
//      We check that the lock is held by the g_current_thread.
//	As with Acquire, this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void Lock::Release() {
    printf("**** Warning: method Lock::Release is not implemented yet\n");
    exit(-1);
}
#endif
#ifdef ETUDIANTS_TP
void Lock::Release() {
  
  	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	if (isHeldByCurrentThread()){
	
		if (sleepqueue->IsEmpty()){
			free=true;
			owner=NULL;
		}
		else{
			owner = (Thread *)sleepqueue->Remove();
			g_scheduler->ReadyToRun(owner);
		}
	}
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif

//----------------------------------------------------------------------
// Lock::isHeldByCurrentThread
/*! To check if current thread hold the lock
*/
//----------------------------------------------------------------------
bool Lock::isHeldByCurrentThread() {return (g_current_thread == owner);}	

//----------------------------------------------------------------------
// Condition::Condition
/*! 	Initializes a Condition, so that it can be used for synchronization.
//
//    \param  "debugName" is an arbitrary name, useful for debugging.
*/
//----------------------------------------------------------------------
Condition::Condition(char* debugName) { 
  name = new char[strlen(debugName)+1];
  strcpy(name,debugName);
  waitqueue = new Listint;
  typeId = CONDITION_TYPE_ID;
}

//----------------------------------------------------------------------
// Condition::~Condition
/*! 	De-allocate condition, when no longer needed.
//      Assumes that nobody is waiting on the condition.
*/
//----------------------------------------------------------------------
Condition::~Condition() {
  typeId = INVALID_TYPE_ID;
  ASSERT(waitqueue->IsEmpty());
  delete [] name;
  delete waitqueue;
}

//----------------------------------------------------------------------
// Condition::Wait
/*! Block the calling thread (put it in the wait queue).
//  This operation must be atomic, so we need to disable interrupts.
*/	
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void Condition::Wait() { 
    printf("**** Warning: method Condition::Wait is not implemented yet\n");
    exit(-1);
}
#endif
#ifdef ETUDIANTS_TP
void Condition::Wait() { 
    //Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	waitqueue->Append(g_current_thread);
	g_current_thread->Sleep();
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif
//----------------------------------------------------------------------
// Condition::Signal

/*! Wake up the first thread of the wait queue (if any). 
// This operation must be atomic, so we need to disable interrupts.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void Condition::Signal() { 
    printf("**** Warning: method Condition::Signal is not implemented yet\n");
    exit(-1);
}
#endif
#ifdef ETUDIANTS_TP
void Condition::Signal() { 
	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	if (!waitqueue->IsEmpty()){
	
		g_scheduler->ReadyToRun((Thread *)waitqueue->Remove());
	}
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif
//----------------------------------------------------------------------
/*! Condition::Broadcast
// wake up all threads waiting in the waitqueue of the condition
// This operation must be atomic, so we need to disable interrupts.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void Condition::Broadcast() { 
  printf("**** Warning: method Condition::Broadcast is not implemented yet\n");
  exit(-1);
}
#endif
#ifdef ETUDIANTS_TP
void Condition::Broadcast() { 
	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	while (!waitqueue->IsEmpty()){
		g_scheduler->ReadyToRun((Thread *)waitqueue->Remove());
	}
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif

/* Ajout des barrières */
#ifdef ETUDIANTS_TP

/*
	Crée une barrière à partir de son nom de débug et du nombre de processus entraînant la levée de la barrière
*/
Barriere::Barriere(char *debugName, int lmtBarriere){

	name = new char[strlen(debugName)+1];
	strcpy(name,debugName);
	
	mutex = new Semaphore((char*)"semBarriereMutex", 1);
	attente = new Semaphore((char *)"semBarriereAttente", 0);
	procsRelaches = new Semaphore((char *)"", 0);
	
	limiteBarriere = lmtBarriere;
	nbProcs = 0;
	
	typeId = BARRIERE_TYPE_ID;
}

Barriere::~Barriere(){

	typeId = INVALID_TYPE_ID;
	delete [] name;
	delete mutex;
	delete attente;
	delete procsRelaches;
}

/* Met un processus en attente ou relache l'ensemble des processus retenus par la barrière (si le nombre de processus en attente = nbProcs */
void Barriere::Wait(){

	int i;
	
	mutex->P();
	
	nbProcs++;
	
	if(nbProcs == limiteBarriere){
	
		//On relâche tous les processus
		for(i=0; i<(limiteBarriere-1); i++){
		
			attente->V();
		}
	
		//On attend que tous les processus soient sortis de la barrière
		for(i=0; i<(limiteBarriere-1); i++){
		
			procsRelaches->P();
		}
		
		nbProcs = 0;
		mutex->V();
	}
	//On attend qu'un processus débloque la barrière
	else{
	
		mutex->V();
		attente->P();
		procsRelaches->V();
	}
}

/* Retourne le nom de debug de la barrière*/
char *Barriere::getName(){

	return name;
}
#endif
