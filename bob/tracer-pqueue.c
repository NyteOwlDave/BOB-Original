
/*
	tracer-pqueue.c

	Priority queue

*/

#include <stdio.h>
#include <math.h>
#include "inc/bob.h"

typedef struct t_qelem {
	Flt		q_key;
	Object*	q_obj;
} Qelem;

static int		Qsize;
static Qelem    Q[PQSIZE];

void PriorityQueueNull(void) {

	Qsize = 0;
	totalQueueResets ++;

}

BOOL PriorityQueueEmpty(void) {

	return (Qsize == 0);

}

void PriorityQueueInsert(Flt key, Object* obj) {

	int			i;
	Qelem		tmp;

	totalQueues++;
 	Qsize++;
	if (Qsize > (int)maxQueueSize)
		maxQueueSize = Qsize;
	if (Qsize >= PQSIZE) {
		fprintf(stderr, "Exhausted priority queue space, dying...\n");
		last_call(1);
	}
	Q[Qsize].q_key = key;
	Q[Qsize].q_obj = obj;

	i = Qsize;
	while (i>1 && Q[i].q_key<Q[i/2].q_key) {
		tmp = Q[i];
		Q[i] = Q[i/2];
		Q[i/2] = tmp;
		i = i / 2;
	}

}

void PriorityQueueDelete(Flt* key, Object** obj) {

	Qelem	tmp;
	int	i, j;

	if (Qsize == 0) {
		printf("Priority queue is empty, dying...\n");
		last_call(1);
	}

	*key = Q[1].q_key;
	*obj = Q[1].q_obj;

	Q[1] = Q[Qsize];
	Qsize--;

	i = 1;

	while (2*i <= Qsize) {

		if (2*i == Qsize) {
			j = 2*i;
		} else if (Q[2*i].q_key < Q[2*i+1].q_key) {
			j = 2*i;
		} else {
			j = 2*i + 1;
		}

		if (Q[i].q_key > Q[j].q_key) {
			tmp = Q[i];
			Q[i] = Q[j];
			Q[j] = tmp;
			i = j;
		} else {
			break;
		}
	}

}

