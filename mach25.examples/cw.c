/*
 *      Beispielprogramm zum Kapitel 6
 *
 *      examples to chapter 6
 *
 *      (S. 106f., Kap. 6.4, Conway's Problem)
 *      "... es sollen Lochkarten mit 80 Zeichen gelesen werden. In einem
 *      anschliessenden Bearbeitungsschritt soll jedes Vorkommen der Zeichen-
 *      kette "**" durch ein "^"-Zeichen ersetzt werden, wobei nach jeder
 *      gelesenen Karte ein Leerzeichen eingefuegt wird. Die so bearbeiteten
 *      Zeichenketten sollen auf Lochkarten von 125 Zeichen Laenge ausgege-
 *      ben werden. Natuerlich ist es moeglich, dieses Problem mittels eines
 *      sequentiellen Programmes zu programmieren, eine elegantere Loesung
 *      besteht jedoch aus einem Programm mit drei Threads: einem Thread,
 *      der 80 Zeichen lange Eingaben liest und an jede Eingabekarte ein
 *      Leerzeichen anhaengt, einem Thread, der von der Laenge des Ein- und
 *      Ausgabeformates nicht weiss, sondern nur die Zeichensubstitution
 *      durchfuehrt, sowie einem Ausgabethread, der einfach die bearbeiteten
 *      Zeichen auf Karten von 125 Zeichen Laenge ausgibt."
 *
 *      (p. 106, chapter 6.4, Conway's Problem)
 *      Read cards with 80 chars length and replace every "**" with a "^".
 *      Add a blank after each card. Punch the resulting string on cards
 *      with 125 chars length. Of course the problem can be solved with a
 *      sequentiell program but it seems more professionell to do it with
 *      a program that consist of three threads: one reads the cards and
 *      appends a blank after each. The second is simply replacing the chars
 *      without minding about the length of the string and the third punches
 *      the string on cards with 125 chars length.
 */

/*
 *      Conway's problem mit Cthreads unter Mach   'cw.c'
 *
 *      Conway's problem using Cthreads under Mach
 */

#include <mach.h>
#include <stdio.h>
#include <string.h>
#include <cthreads.h>

#define	INPUT_CARD_SIZE		80
#define OUTPUT_CARD_SIZE	125

struct read_buf {
	mutex_t card_lock;
	condition_t non_empty, non_full;
	char read_c;
	int cnt;
} buffer0;		/* Eingabepuffer */
                        /* input buffer  */

struct write_buf {
	mutex_t buf_lock;
	condition_t non_empty, non_full;
	char process_c;
	int cnt;
} buffer1;              /* Ersetzungspuffer     */
                        /* buffer for replacing */

struct proc_buf {
	struct read_buf *rdr;
	struct write_buf *wrt;
} buffer2;      /* Bearbeitungspuffer (Zeiger auf beide obigen Puffer) */
                /* working buffer (pointer to the above buffers)       */

void reader(struct read_buf *buf)
{
	int i;
	char c;


	i = 0; /* Zaehler fuer Zeichenanzahl der einzelnen Karten */
               /* counter for number of chars on the cards        */
	while (feof(stdin) == 0) {
		if (++i % INPUT_CARD_SIZE == 0)
                        c = ' ';/* Nach 80. Zeichen Leerzeichen hinzufuegen */
                                /* add blank after the 80th char            */
		else
			c = getchar();
		if (c != '\n') {	/* CR werden ueberlesen */
                                        /* ignore CRs           */
			mutex_lock(buf->card_lock);
			buf->read_c = c;
                        buf->cnt++;     /* Es ist ein Zeichen im Puffer  */
                                        /* there is a char in the buffer */
			condition_signal(buf->non_empty);
                        while (buf->cnt > 0)   /* Auf Pufferentleerung warten */
                                               /* wait on clearing the buffer */
				condition_wait(buf->non_full, buf->card_lock);
			mutex_unlock(buf->card_lock);
		}
	}
	exit(0); 	/* Bei Eingabeende *gesamte* Task stoppen */
                        /* stop task on 'end of input'            */
}

void processor(struct proc_buf *pbuf)
{
	struct read_buf *rbuf;
	struct write_buf *wbuf;

	rbuf = pbuf->rdr;	/* Funktionsargumente auspacken */
                                /* unpak arguments              */
	wbuf = pbuf->wrt;
	for (;;) {
		mutex_lock(rbuf->card_lock);
		while (rbuf->cnt == 0) /* Auf Zeichen vom Leser warten */
                                       /* waiting on char fromm reader */
			condition_wait(rbuf->non_empty, rbuf->card_lock);
		if (rbuf->read_c != '*' ) {	/* Ist Zeichen ein '*' ? */
                                                /* '*' arrived ?         */
			mutex_lock(wbuf->buf_lock);	/* Normales Zeichen */
                                                        /* normal char      */
                        wbuf->process_c = rbuf->read_c;
                                            /* Zeichen an Puncher schreiben */
                                            /* writing char to puncher      */
			condition_signal(wbuf->non_empty);
			wbuf->cnt++;	/* Zeichen ist im Puffer */
                                        /* char is in buffer     */
			while (wbuf->cnt > 0)
				condition_wait(wbuf->non_full, wbuf->buf_lock);
		} else {
			rbuf->cnt--;	/* '*' */
			condition_signal(rbuf->non_full);
			while (rbuf->cnt == 0)	/* Weiteres Zeichen holen */
                                                /* fetching next char     */
				condition_wait(rbuf->non_empty, rbuf->card_lock);
                        if (rbuf->read_c == '*') {      /* 2. Zeichen ist '*' */
                                                        /* 2nd char is '*'    */
				mutex_lock(wbuf->buf_lock);
				wbuf->process_c = '^';	/* Zeichen wandeln */
                                                        /* switch char     */
				wbuf->cnt++;	/* Zeichen ist im Puffer */
                                                /* char is in buffer     */
				condition_signal(wbuf->non_empty);
				while (wbuf->cnt > 0)
					condition_wait(wbuf->non_full, wbuf->buf_lock);
                        } else {       /* 2. Zeichen nicht '*', beide senden */
                                       /* 2nd char is not '*', sending both  */
				mutex_lock(wbuf->buf_lock);
				wbuf->process_c = '*';
				wbuf->cnt++;
				condition_signal(wbuf->non_empty);
				while (wbuf->cnt > 0)
					condition_wait(wbuf->non_full, wbuf->buf_lock);
				wbuf->process_c = rbuf->read_c;
				condition_signal(wbuf->non_empty);
				wbuf->cnt++;
				while (wbuf->cnt > 0)
					condition_wait(wbuf->non_full, wbuf->buf_lock);
			}
		}
		mutex_unlock(wbuf->buf_lock);
                rbuf->cnt--;    /* Leser freigeben     */
                                /* setting reader free */
		condition_signal(rbuf->non_full);
		mutex_unlock(rbuf->card_lock);
	}
}

void puncher(struct write_buf *wbuf)
{
	int i;
	char c;

	i = 0;	
	for (;;) {
		mutex_lock(wbuf->buf_lock);
		while (wbuf->cnt == 0)	/* Auf Zeichen warten */
                                        /* waiting on char    */
			condition_wait(wbuf->non_empty, wbuf->buf_lock);
		c = wbuf->process_c;
		wbuf->cnt--;	/* Puffer entleeren */
                                /* clearing buffer  */
		condition_signal(wbuf->non_full);
		mutex_unlock(wbuf->buf_lock);
		putchar(c);	/* Zeichen drucken */
                                /* printing char   */
		if (++i % OUTPUT_CARD_SIZE == 0)
			putchar('\n');	/* Zeilenvorschub nach jeder Karte */
                                        /* CR following every card         */
	}
}

void main(int argc, char *argv [0])	
{
	if (argc > 1) {
                fprintf(stderr, "%s benutzt keine Argumente\n", argv [0]);
                fprintf(stderr, "%s uses no argumets\n", argv [0]);
		exit(-1);
	}	/* Beide Puffer vorbereiten */
                /* preparing both buffers   */
	buffer0.card_lock = mutex_alloc();
	buffer0.non_empty = condition_alloc();
	buffer0.non_full = condition_alloc();
	buffer0.cnt = 0;
	buffer1.buf_lock = mutex_alloc();
	buffer1.non_empty = condition_alloc();
	buffer1.non_full = condition_alloc();
	buffer1.cnt = 0;
	buffer2.rdr = &buffer0;	
                        /* Einpacken der Funktionsargumente   */
                        /* packing arguments for the function */
	buffer2.wrt = &buffer1;
			/* Threads erzeugen */
                        /* crating threads  */
	cthread_detach(cthread_fork((any_t) &reader, (any_t) 
			&buffer0));
	cthread_detach(cthread_fork((any_t) &processor, (any_t) 
			&buffer2));
	puncher((any_t) &buffer1);
}
