/*
 *      Beispielprogramm zum Kapitel 7
 *      examples to chapter 7
 *
 *      (S. 116, Kap. 7.3)
 *      "Am Schluss dieses Kapitels ueber das VM-System soll ein kurzes
 *      Beispielprogramm stehen, welches einen beschraenkten Puffer ver-
 *      waltet (...). Das Programm besteht aus zwei Teilen: einem Produ-
 *      zenten, der die Daten erzeugt, sowie einem Konsumenten, der diese
 *      verbraucht, in diesem Fall am Bildschirm anzeigt."
 *
 *      (p. 116, chapter 7.3)
 *      An example for the discussed VM-system: the program manages a
 *      limited buffer. The two parts of the program are: first a producer,
 *      which generates data and second a consumer, which takes the data
 *      prints it to the screen.
 *
 */

/*
 *      Beispielprogramm fuer den Einsatz eines VM-Objektes.  'vm.c'
 *
 *      Example for using vm-objects
 *
 */

#include <stdio.h>
#include <mach.h>
#include <stdlib.h>
#include <string.h>

#define	TRUE	1
#define	FALSE	0
#define	ITEMS	30

#define	CONSUMER_WAIT	0
#define	PRODUCER_WAIT	1

typedef struct {
	char string [256];
	int nummer;
} DATA;


int *lock;		/* Waechter fuer kritischen Bereich */
                        /* guard for critical section       */
int *cnt;		/* Zaehler fuer Anzahl der Puffereintraege */
                        /* counter for number of entries in buffer */
DATA *buffer;


void consumer(void)
{
	int i, rcv_cnt;

	rcv_cnt = 0;

	for (i = 0; i < ITEMS; i++) {
		while (*lock == CONSUMER_WAIT) ; /* Auf Pufferfreigabe warten */
                                                 /* waiting for the buffer    */
		if (*cnt > 0) {
			printf("Nummer %d: %s\n", buffer [rcv_cnt].nummer,
				buffer [rcv_cnt].string);
                        printf("number %d: %s\n", buffer [rcv_cnt].nummer,
				buffer [rcv_cnt].string);
			if (++rcv_cnt == ITEMS) rcv_cnt = 0;
			(*cnt)--;
		}
		*lock = CONSUMER_WAIT;
	}
	printf("Konsumentenprozess beendet\n");
        printf("exiting process 'consumer'\n");
    exit(0);
}

void producer(void)
{
	int i, sen_cnt, counter;

	sen_cnt = 0;
	counter = 0;

	for (i = 0; i < ITEMS; i++) {
		while (*lock == PRODUCER_WAIT) ; 
				/* Auf Pufferfreigabe warten */
                                /* waiting for the buffer    */
		*lock = CONSUMER_WAIT;
		if (*cnt < ITEMS) {
			strcpy(buffer [sen_cnt].string,
				"Testdaten fuer Konsument");
			strcpy(buffer [sen_cnt].string,
                                "test data for consumer");
			buffer [sen_cnt].nummer = ++counter;
			if (++sen_cnt == ITEMS) sen_cnt = 0;
			(*cnt)++;
		}
		*lock = PRODUCER_WAIT;
	}
	printf("Produzentenprozess beendet\n");
        printf("exiting process 'producer'\n");
}

void main(int argc, char **argv)
{
	int i;

	if (argc > 1) {
		fprintf(stderr, "%s hat keine Argumente\n", argv [0]);
                fprintf(stderr, "%s uses no arguments\n", argv [0]);
		exit(-1);
	}
        /* VM-Objekt anlegen  */
        /* creating vm-object */
	if (vm_allocate(task_self(), (vm_address_t *) &buffer,
		sizeof(DATA) * (ITEMS / 2), TRUE) != KERN_SUCCESS) {
		fprintf(stderr, "BUFFER allocate fehlgeschlagen\n");
                fprintf(stderr, "BUFFER allocate failed\n");
		exit(-1);
	}
	if (vm_allocate(task_self(), (vm_address_t *) &lock,
		sizeof(int), TRUE) != KERN_SUCCESS) {
		fprintf(stderr, "LOCK allocate fehlgeschlagen\n");
                fprintf(stderr, "LOCK allocate failed\n");
		exit(-1);
	}
	if (vm_allocate(task_self(), (vm_address_t *) &cnt,
		sizeof(int), TRUE) != KERN_SUCCESS) {
		fprintf(stderr, "CNT allocate fehlgeschlagen\n");
                fprintf(stderr, "CNT allocate failed\n");
		exit(-1);
	}
	/* Vererbung festlegen */
        /* fixing inheritence  */
	if (vm_inherit(task_self(), (vm_address_t) buffer,
		sizeof(DATA) * (ITEMS / 2), VM_INHERIT_SHARE) != 
		KERN_SUCCESS) {
		fprintf(stderr, "BUFFER inherit fehlgeschlagen\n");
                fprintf(stderr, "BUFFER inherit failed\n");
		exit(-1);
	}
	if (vm_inherit(task_self(), (vm_address_t) lock,
		sizeof(int), VM_INHERIT_SHARE) != KERN_SUCCESS) {
		fprintf(stderr, "LOCK inherit fehlgeschlagen\n");
                fprintf(stderr, "LOCK inherit failed\n");
		exit(-1);
	}
	if (vm_inherit(task_self(), (vm_address_t) cnt,
		sizeof(int), VM_INHERIT_SHARE) != KERN_SUCCESS) {
		fprintf(stderr, "CNT inherit fehlgeschlagen\n");
                fprintf(stderr, "CNT inherit failed\n");
		exit(-1);
	}
	*cnt = 0;
	*lock = CONSUMER_WAIT;
				/* Kindprozess erzeugen */
                                /* creating child       */
	switch (i = fork()) {
		case 0:
			consumer();	/* Kind-Prozess wird Konsument */
                                        /* child becomes consumer      */
			break;
		case -1:
			fprintf(stderr, "%d: fork nicht moeglich\n", i);
                        fprintf(stderr, "%d: fork impossible\n", i);
			exit(-1);
		default:	
			producer();	/* Elter-Prozess wird Produzent */
                                        /* parent becomes producer      */
	}
	/* Auf das Ende des Kind-Prozesses warten */
        /* waiting for the child to terminate     */
	wait(NULL);
	/* VM-Objekt freigeben */
        /* releasing vm-object */
	vm_deallocate(task_self(), (vm_address_t) buffer,
		sizeof(DATA) * (ITEMS / 2));
	vm_deallocate(task_self(), (vm_address_t) lock, 
		sizeof(int));
	vm_deallocate(task_self(), (vm_address_t) cnt, 
		sizeof(int));
}

