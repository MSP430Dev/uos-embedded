#ifndef __CAN_H_
#define __CAN_H_

/*
 * Stack size for the task of CAN interrupt handler.
*/
#ifndef CAN_STACKSZ
#   define CAN_STACKSZ		1000
#endif

/*
 * CAN data packet.
 */
typedef struct __attribute__((__packed__)) _can_frame_t {
	unsigned EID       : 18;	/* extended identifier */
	unsigned SID       : 11;	/* standard identifier */
	unsigned reserved1 : 3;
	unsigned DLC       : 4;		/* length in bytes */
	unsigned reserved2 : 4;
	unsigned RTR       : 1;		/* request for reply */
	unsigned reserved3 : 3;
	unsigned IDE       : 1;		/* extended format */
	unsigned reserved4 : 19;
	unsigned char data [8];		/* up to 8 bytes of data */
} can_frame_t;

#include <milandr/can-queue.h>

/*
 * Data structure for CAN channel.
 */
typedef struct _can_t {
	mutex_t lock;			/* interrupt goes here */

	unsigned port;			/* port number */
	unsigned kbitsec;		/* data rate kbit/sec */
	can_queue_t inq;		/* queue of received packets */
	can_queue_t outq;		/* queue of packets to transmit */

	unsigned long intr;		/* interrupt counter */

	int (*callback_receive) (struct _can_t *, can_frame_t *);
	int (*callback_transmit) (struct _can_t *, can_frame_t *);
	void (*callback_error) (struct _can_t *, int error);

	ARRAY (stack, CAN_STACKSZ);	 /* task stack */
} can_t;

/*
 * User level API.
 */
void can_init (can_t *c, int port, int prio, int kbps);
void can_reset (can_t *c);
void can_stop (can_t *c, int stopflag);
void can_set_speed (can_t *c, unsigned kbitsec);
int can_transmit_space (can_t *c);

bool_t can_output (can_t *c, can_frame_t fr);
can_frame_t can_input (can_t *c);

#endif /* !__CAN_H_ */
