
/* Beispielprogramm zum Kapitel 4 */
/* Example to chapter 4           */

/*
 *      Beispielprogramm fuer die Benutzung von Mach IPC
 *
 *      (S. 73, Kap. 4.5 Anwendungsbeispiel)
 *      "Das Programm besteht aus zwei Tasks: eine Task erzeugt eine
 *      zufaellige Zahlenfolge, verpackt diese in eine Nachricht und sendet
 *      diese an eine zweite Task. Diese Task sortiert die enthaltenen Werte
 *      und schickt sie wieder an die urspruengliche Task zurueck."
 *
 *
 *      This is an example for using Mach IPC
 *
 *      (p. 73, Chapter 4.5)
 *      The parent task forks a child task, which fills an array with
 *      random numbers. This array is sent back to the parent.
 *      The parent sorts the array and sends it back to the child.
 */

#include <stdio.h>
#include <mach.h>
#include <mach/message.h>
#include <servers/env_mgr.h>   /* Fuer Environment-Manager */
                               /* for environment manager  */

#define SIZE    40             /* Anzahl der Zufallszahlen */
                               /* amount of random numbers */

struct message {
        msg_header_t header;
        msg_type_t data;
        int numbers [SIZE];
};

extern qsort(void *, int, int, int (*)(void *, void *));

/*
 *      Compare-Funktion fuer qsort()
 *      compare function for qsort()
 */

int compare(int *a, int *b)
{
        return *a - *b;
}
/*
 *      Der Elter-Prozess: Feld vom Kind-Prozess empfangen,
 *      in Sende-Paket kopieren, Zahlen sortieren, und an den
 *      Kind-Prozess zurueckschicken.
 *
 *      parent process: receiving array from child, copy to
 *      send packet, sorting numbers, return to child
 */

void parent(port_t sort_port)
{
        struct message rcv, xmit;       /* Zwei Puffer */
                                        /* two buffers */
        kern_return_t ret;

        rcv.header.msg_local_port = sort_port;
        rcv.header.msg_size = sizeof(rcv);
        /* Nachricht empfangen */
        /* receiving message   */
        ret = msg_receive(&rcv.header, MSG_OPTION_NONE, 0);
        if (ret != RCV_SUCCESS) {
                fprintf(stderr, "Konnte Botschaft vom Kind-Prozess nicht empfangen, Abbruch\n");
                fprintf(stderr, "Coulnd't receive message from child; process, exiting...\n");
                exit(-1);
        }


        /* Zahlen mittels Quick-Sort sortieren */
        /* sorting numbers using quick sort    */

        qsort((void *) rcv.numbers, SIZE, sizeof(int), (int (*)
	     (void *, void *)) compare); 

        /* Nachricht in Sendepuffer kopieren */
        /* copying message to send buffer    */

        memcpy(&xmit.header, &rcv.header, sizeof(rcv));

        /* Nachrichtenkopf vorbereiten */
        /* preparing message header    */

        xmit.header.msg_remote_port = rcv.header.msg_remote_port;
        xmit.header.msg_local_port = PORT_NULL;
        xmit.header.msg_size = sizeof(xmit);
        xmit.header.msg_id = 0;
        xmit.header.msg_type = MSG_TYPE_NORMAL;
        xmit.header.msg_simple = TRUE;

        /* Typinformationen setzen  */
        /* setting type information */

        xmit.data.msg_type_name = MSG_TYPE_INTEGER_32;
        xmit.data.msg_type_size = sizeof(int) * 8;
        xmit.data.msg_type_number = SIZE;
        xmit.data.msg_type_inline = TRUE;
        xmit.data.msg_type_longform = FALSE;
        xmit.data.msg_type_deallocate = FALSE;

        /* Nachricht an Kind-Prozess zuruecksenden */
        /* sending message back to child           */

        ret = msg_send(&xmit.header, MSG_OPTION_NONE, 0);
        if (ret != SEND_SUCCESS) {
                fprintf(stderr, "Konnte Botschaft nicht an Kind-Prozess senden, Abbruch\n");
                fprintf(stderr, "Coulnd't send message to child process, exiting...\n");
                exit(-1);
        }
        /* Port loeschen */
        /* deleting port */
        port_deallocate(task_self(), sort_port);
}

void child(void)
{
        int i;
        struct message xmit;
        port_t reply_port;
        kern_return_t ret;
        port_t sort_port;

        /* Port fuer Rueckantwort anlegen */
        /* creating port for response     */
        ret = port_allocate(task_self(), &reply_port);
        if (ret != KERN_SUCCESS) {
                fprintf(stderr, "Konnte Reply-Port nicht anlegen, Abbruch\n");
                fprintf(stderr, "Couldn't allocate reply port, exiting\n");
                exit(-1);
        }

        /* Elter-Port aus Umgebung holen               */
        /* getting port of the parent from environment */

        ret = env_get_port(environment_port, "SortPort",
                &sort_port);
        if (ret != KERN_SUCCESS) {
                fprintf(stderr, "Konnte Elter-Port nicht aus der Umgebung holen, Abbruch\n");
                fprintf(stderr, "Couldn't get parent from the environment, exiting...\n");

                exit(-1);
        }

        /* Feld mit Zahlen fuellen    */
        /* filling array with numbers */

        srand((unsigned) time(NULL));
        printf("Unsortierte Nummern: \n");
        printf("Unsorted numbers: \n");
        for (i = 0; i < SIZE; i++) {
                xmit.numbers [i] = rand();
                printf("%d\t", xmit.numbers [i]);
        }
        printf("\n");

        xmit.header.msg_remote_port = sort_port;
        xmit.header.msg_local_port = reply_port;
        xmit.header.msg_size = sizeof(xmit);
        xmit.header.msg_id = 0;
        xmit.header.msg_type = MSG_TYPE_NORMAL;
        xmit.header.msg_simple = TRUE;

        xmit.data.msg_type_name = MSG_TYPE_INTEGER_32;
        xmit.data.msg_type_size = sizeof(int) * 8;
        xmit.data.msg_type_number = SIZE;
        xmit.data.msg_type_inline = TRUE;
        xmit.data.msg_type_longform = FALSE;
        xmit.data.msg_type_deallocate = FALSE;

        /* Zahlen zum Sortieren an Elter-Prozess senden */
        /* sending unsorted numbers to parent           */

        ret = msg_rpc(&xmit.header, MSG_OPTION_NONE, sizeof(xmit),
                0, 0);

        if (ret != RPC_SUCCESS) {
                fprintf(stderr, "Konnte Zahlen nicht zum Sortieren schicken, Abbruch\n");
                fprintf(stderr, "Couldn't send numbers for sorting, existing\n");
                exit(-1);
        }
        printf("Sortierte Zahlen:\n");
        printf("Sorted numbers:\n");
        for (i = 0; i < SIZE; i++)
                printf("%d\t", xmit.numbers [i]);
        printf("\n");
        port_deallocate(task_self(), reply_port);

        exit(0);

}

void main(int argc, char *argv[])
{
        kern_return_t ret;
        port_t sort_port;

        if (argc > 1) {
                fprintf(stderr, "%s benutzt keine Argumente\n", argv [1]);
                fprintf(stderr, "%s doesn't use arguements\n", argv [1]);
                exit(-1);
        }

        /* Elter-Port anlegen   */
        /* creating parent port */


        ret = port_allocate(task_self(), &sort_port);
        if (ret != KERN_SUCCESS) {
                fprintf(stderr, "Konnte Sort-Port nicht anlegen, Abbruch\n");
                fprintf(stderr, "Couldn't allocate sort port, exiting...\n");
                exit(-1);
        }

        /* Port in die Umgebung eintragen        */
        /* making port accessible in environment */

        ret = env_set_port(environment_port, "SortPort", 
                sort_port);
        if (ret != KERN_SUCCESS) {
                fprintf(stderr, "Konnte Sort-Port nicht in die, Umgebung eintragen, Abbruch\n");
                fprintf(stderr, "Couldn't register sort with the environment, exiting...\n");
                exit(-1);
        }
        /* Kind-Prozess erzeugen */
        /* creating child        */
        switch (fork()) {
                case 0 :
                        child();
                        break;
                case -1 :
                        fprintf(stderr, "Konnte keinen Kind-Prozess erzeugen, Abbruch\n");
                        fprintf(stderr, "Couldn't fork child process, exiting...\n");
                        exit(-1);
                default:
                        parent(sort_port);
                        break;
        }
        exit(0);
}
