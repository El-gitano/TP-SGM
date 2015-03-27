/* \file drvACIA.cc
   \brief Routines of the ACIA device driver
//
//      The ACIA is an asynchronous device (requests return 
//      immediately, and an interrupt happens later on).  
//      This is a layer on top of the ACIA.
//      Two working modes are to be implemented in assignment 2:
//      a Busy Waiting mode and an Interrupt mode. The Busy Waiting
//      mode implements a synchronous IO whereas IOs are asynchronous
//      IOs are implemented in the Interrupt mode (see the Nachos
//      roadmap for further details).
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//
*/

/* Includes */

#include "kernel/system.h"         // for the ACIA object
#include "kernel/synch.h"
#include "machine/ACIA.h"
#include "drivers/drvACIA.h"

//-------------------------------------------------------------------------
// DriverACIA::DriverACIA()
/*! Constructor.
  Initialize the ACIA driver. In the ACIA Interrupt mode, 
  initialize the reception index and semaphores and allow 
  reception and emission interrupts.
  In the ACIA Busy Waiting mode, simply inittialize the ACIA 
  working mode and create the semaphore.
  */
//-------------------------------------------------------------------------
#ifndef ETUDIANTS_TP
DriverACIA::DriverACIA()
{
  printf("**** Warning: contructor of the ACIA driver not implemented yet\n");
  exit(-1);
}
#endif
#ifdef ETUDIANTS_TP
DriverACIA::DriverACIA()
{
	send_sema = new Semaphore((char*)"ACIASemSend", 1);
	
	
	if ( (g_cfg->ACIA) == ACIA_BUSY_WAITING ){
	
		DEBUG('d', (char*)"Activation de l'attente active.\n");
		receive_sema = new Semaphore((char*)"ACIASemReceive", 1);
		g_machine->acia->SetWorkingMode(BUSY_WAITING);
	}
	else if( (g_cfg->ACIA) == ACIA_INTERRUPT ){
	
		DEBUG('d', (char*)"Activation du mode interruption.\n");
		receive_sema = new Semaphore((char*)"ACIASemReceive", 0);
		g_machine->acia->SetWorkingMode(SEND_INTERRUPT|REC_INTERRUPT);
		ind_rec = 0;
	} 
}	
#endif
//-------------------------------------------------------------------------
// DriverACIA::TtySend(char* buff)
/*! Routine to send a message through the ACIA (Busy Waiting or Interrupt mode)
  */
//-------------------------------------------------------------------------
#ifndef ETUDIANTS_TP
int DriverACIA::TtySend(char* buff)
{ 
	
	printf("**** Warning: method Tty_Send of the ACIA driver not implemented yet\n");
	exit(-1);
	return 0;
}
#endif
#ifdef ETUDIANTS_TP
int DriverACIA::TtySend(char* buff)
{ 

	int i;
	
	//On envoie notre chaîne puis on libère pour les autres processus en attente
	DEBUG('d', (char*)"Un thread souhaite envoyer une chaîne.\n");
	
	send_sema->P();
	
	i = 0;
	
	//Envoi via attente active
  	if(g_machine->acia->GetWorkingMode() == BUSY_WAITING){
	
		DEBUG('d', (char*)"Attente active, envoi d'une chaîne.\n");

		do{
		
			//Attente active
			while(g_machine->acia->GetOutputStateReg() != EMPTY){
			
				DEBUG('d', (char*)"Attente active.\n");
			}
			
			g_machine->acia->PutChar(buff[i]);
			i++;
		
		}while(buff[i-1] != '\0');
		
		send_sema->V();
		return i;
  	}
  	//Envoi via les interruptions
  	else if(g_machine->acia->GetWorkingMode()==(REC_INTERRUPT | SEND_INTERRUPT)){
  	
		DEBUG('d', (char*)"Attente passive, envoi d'une chaîne.\n");
		
		ind_send = 0;
		
		//Copie de la chaîne dans le buffer d'envoi
		do{
		
			send_buffer[i] = buff[i];
			i++;
			
		}while(buff[i-1] != '\0' && i<BUFFER_SIZE);
		
		send_buffer[i-1] = '\0';
		
		//Envoi du premier caractère pour lancer les interruptions
		g_machine->acia->PutChar(send_buffer[ind_send]);
		ind_send++;
		
		return i;
  	}
}
#endif
//-------------------------------------------------------------------------
// DriverACIA::TtyReceive(char* buff,int length)
/*! Routine to reveive a message through the ACIA 
//  (Busy Waiting and Interrupt mode).
  */
//-------------------------------------------------------------------------
#ifndef ETUDIANTS_TP
int DriverACIA::TtyReceive(char* buff,int lg)
{
   printf("**** Warning: method Tty_Receive of the ACIA driver not implemented yet\n");
  exit(-1);
  return 0;
}
#endif
#ifdef ETUDIANTS_TP
int DriverACIA::TtyReceive(char* buff, int lg)
{
	/* Précond. */
	if(lg <= 0){
	
		return -1;
	}
	
	int i;
	
	//On recupère notre chaîne puis on libère pour les autres processus en attente
	DEBUG('d', (char*)"Un thread souhaite recevoir une chaîne.\n");
	
	receive_sema->P();
	
	i = 0;
	
	//Reception via attente active	
	if(g_machine->acia->GetWorkingMode() == BUSY_WAITING){
		
		DEBUG('d', (char*)"Attente active, reception d'une chaîne.\n");

		do{
		
			while(g_machine->acia->GetInputStateReg() == EMPTY);
			
			buff[i] = g_machine->acia->GetChar();
			i++;
			
		}while( (buff[i-1] != '\0') && (i < lg) );
		
		buff[i-1] = '\0';
		
		receive_sema->V();
		return i;
	}
	//Reception via interruptions (On compare le mode de travail à SEND_INTERRUPT car la routine d'interruption à normalement désactivé les interruptions en reception (voir le commentaire de InterruptReceive() ))
	else if(g_machine->acia->GetWorkingMode() == SEND_INTERRUPT){
  	
		DEBUG('d', (char*)"Attente passive, reception d'une chaîne.\n");
		
		//Copie de la chaîne dans le buffer de réception
		do{
		
			buff[i] = receive_buffer[i];
			i++;
			
		}while(receive_buffer[i-1] != '\0' && i<lg);
		buff[i-1] = '\0';
		
		ind_rec = 0;
		g_machine->acia->SetWorkingMode(SEND_INTERRUPT|REC_INTERRUPT);
			
		return i;
  	}
}
#endif
//-------------------------------------------------------------------------
// DriverACIA::InterruptSend()
/*! Emission interrupt handler.
  Used in the ACIA Interrupt mode only. 
  Detects when it's the end of the message (if so, releases the send_sema semaphore), else sends the next character according to index ind_send.
  */
//-------------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void DriverACIA::InterruptSend()
{
	printf("**** Warning: send interrupt handler not implemented yet\n");
  	exit(-1);
}
#endif
#ifdef ETUDIANTS_TP
void DriverACIA::InterruptSend()
{	
	//On a terminé l'envoi
  	if(send_buffer[ind_send-1] == '\0'){
  	
  		send_sema->V();
  	}
  	//On continue d'envoyer notre chaîne
  	else{
  	
  		g_machine->acia->PutChar(send_buffer[ind_send]);
  		ind_send++;
  	}
}
#endif

//-------------------------------------------------------------------------
// DriverACIA::Interrupt_receive()
/*! Reception interrupt handler.
  Used in the ACIA Interrupt mode only. Reveices a character through the ACIA. 
  Releases the receive_sema semaphore and disables reception 
  interrupts when the last character of the message is received 
  (character '\0').
  */
//-------------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void DriverACIA::InterruptReceive()
{
  printf("**** Warning: receive interrupt handler not implemented yet\n");
  exit(-1);
}
#endif
#ifdef ETUDIANTS_TP
void DriverACIA::InterruptReceive()
{

	char c = g_machine->acia->GetChar();
	
	//On finit, ou on doit finir la réception
	if( (ind_rec == (BUFFER_SIZE-1)) || (c == '\0')){
	
		receive_buffer[ind_rec] = '\0';
		g_machine->acia->SetWorkingMode(g_machine->acia->GetWorkingMode()^REC_INTERRUPT);
		receive_sema->V();
	}
	//On continue de recevoir la chaîne
	else{
	
		receive_buffer[ind_rec] = c;
		ind_rec++;
	}
}
#endif
