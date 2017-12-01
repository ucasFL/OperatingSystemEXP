#include "common.h"
#include "mbox.h"
#include "sync.h"
#include "scheduler.h"


//typedef struct
//{
//    int8_t info[MAX_MBOX_LENGTH];
//    /* TODO */
//} Message;

typedef struct
{
	char name[MBOX_NAME_LENGTH];
	char buf[MAX_MBOX_LENGTH];
	int next_buf;
	//Message buf;
	//int head;
	//int tail;
	condition_t Buf_full, Buf_empty;
	//int Msg_byte;
	int turn;
	lock_t BoxLock;
	/* TODO */
} MessageBox;


static MessageBox MessageBoxen[MAX_MBOXEN];
//lock_t BoxLock;

/* Perform any system-startup
 * initialization for the message
 * boxes.
 */
void init_mbox(void)
{
	int i;
	for ( i = 0; i < MAX_MBOXEN; i++ ){
		enter_critical();
		MessageBoxen[i].name[0] = '\0';
		MessageBoxen[i].next_buf = 0;
		//MessageBoxen[i].head = 0;
		//MessageBoxen[i].tail = 0;
		//MessageBoxen[i].Msg_byte = 0;
		MessageBoxen[i].turn = 0;
		leave_critical();
		condition_init(&(MessageBoxen[i].Buf_full));
		condition_init(&(MessageBoxen[i].Buf_empty));
		enter_critical();
		lock_init(&(MessageBoxen[i].BoxLock));
		leave_critical();
	}
	/* TODO */
}

/* Opens the mailbox named 'name', or
 * creates a new message box if it
 * doesn't already exist.
 * A message box is a bounded buffer
 * which holds up to MAX_MBOX_LENGTH items.
 * If it fails because the message
 * box table is full, it will return -1.
 * Otherwise, it returns a message box
 * id.
 */
//int opentime = 0;
//int anothertime = 0;
mbox_t do_mbox_open(const char *name)
{
	//printf(2, 1, "open mbox");
	//    enter_critical();
	//printf(1, 1, "%s", MessageBoxen[0].name);
	//printf(2, 1, "%s", MessageBoxen[1].name);
	//opentime ++;
	//printf(2, 1, "%d", opentime);
  (void)name;
  int i;
  //printf(2, 1, "SSSSSSSSSSSSS");
  for (i = 0; i < MAX_MBOXEN; i++ ){
	  if (MessageBoxen[i].turn != 0 && same_string(name, MessageBoxen[i].name) ){
		  lock_acquire(&(MessageBoxen[i].BoxLock));
		  MessageBoxen[i].turn ++;
		//leave_critical();
		  lock_release(&(MessageBoxen[i].BoxLock));
		  //printf (3, 1, "%s mbox = %d          ", name, i);
		//leave_critical();
		  return i;
	  }
	  //else printf (i, 1, "%s mbox = %s", name, MessageBoxen[i].name);
  }
  //anothertime ++;
  //printf (6, 1, "%d", anothertime);
  for ( i = 0; i < MAX_MBOXEN; i++ ){
	  if (MessageBoxen[i].turn == 0){
		  lock_acquire( &(MessageBoxen[i].BoxLock) );
		//enter_critical();
		  MessageBoxen[i].turn ++;
		  bcopy(name, MessageBoxen[i].name, strlen(name));
		  lock_release( &MessageBoxen[i].BoxLock );
		  //printf (5, 1, "%s mbox = %d                ", name, i);
		//leave_critical();
		  return i;
	  }
  }
	/* TODO */
}

/* Closes a message box
 */
void do_mbox_close(mbox_t mbox)
{
  (void)mbox;
  condition_init( &(MessageBoxen[mbox].Buf_full) );
  condition_init( &(MessageBoxen[mbox].Buf_empty) );
		//enter_critical();
  MessageBoxen[mbox].next_buf = 0;
  //MessageBoxen[mbox].head = 0;
  //MessageBoxen[mbox].tail = 0;
  MessageBoxen[mbox].turn = 0;
  MessageBoxen[mbox].name[0] = '\0';
		//leave_critical();
  //MessageBoxen[mbox].Msg_byte = 0;
	/* TODO */
}

/* Determine if the given
 * message box is full.
 * Equivalently, determine
 * if sending to this mbox
 * would cause a process
 * to block.
 */
int do_mbox_is_full(mbox_t mbox)
{
  (void)mbox;
  if (MessageBoxen[mbox].next_buf >= MAX_MBOX_LENGTH)
	return 1;
  return 0;
  //return MessageBoxen[mbox].buf_used;
  //printf (5, 1, "full");
  //if ((MessageBoxen[mbox].tail + 1)%MAX_MBOX_LENGTH == MessageBoxen[mbox].head)
  //  return 1;
  //return 0;
	/* TODO */
}

/* Enqueues a message onto
 * a message box.  If the
 * message box is full, the
 * process will block until
 * it can add the item.
 * You may assume that the
 * message box ID has been
 * properly opened before this
 * call.
 * The message is 'nbytes' bytes
 * starting at offset 'msg'
 */
void do_mbox_send(mbox_t mbox, void *msg, int nbytes)
{
  //(void)mbox;
  ////(char *)msg;
  //(void)nbytes;

  lock_acquire( &(MessageBoxen[mbox].BoxLock) );
		//leave_critical();
  while (do_mbox_is_full(mbox))
	condition_wait (&(MessageBoxen[mbox].BoxLock), &(MessageBoxen[mbox].Buf_empty));
  bcopy((char *)msg, &(MessageBoxen[mbox].buf[MessageBoxen[mbox].next_buf]), nbytes);
  MessageBoxen[mbox].next_buf += nbytes;
  //int i;
  //for( i = MessageBoxen[mbox].next_buf; i < MessageBoxen[mbox].next_buf + nbytes; i++ ){
  //    MessageBoxen[mbox].buf[i] = *(char *)msg;
  //    ((char *)msg)++;
  //}
  //int i;
  //for ( i = 0; i < nbytes; i++ ){
  //    MessageBoxen[mbox].buf.info[MessageBoxen[i].tail] = *(int8_t *)msg;
  //    ((int8_t *)msg)++;
  //}
  //MessageBoxen[mbox].Msg_byte += nbytes;
  //MessageBoxen[i].tail %= MAX_MBOX_LENGTH;
		//enter_critical();
  //MessageBoxen[mbox].buf_used = 1;
  //MessageBoxen[mbox].buf = *(int *)msg;
  //      leave_critical();
  condition_signal(&(MessageBoxen[mbox].Buf_full));
		//enter_critical();
  lock_release( &(MessageBoxen[mbox].BoxLock) );
		//leave_critical();
	//printf(4, 1, "send massege");
  /* TODO */
}

/* Receives a message from the
 * specified message box.  If
 * empty, the process will block
 * until it can remove an item.
 * You may assume that the
 * message box has been properly
 * opened before this call.
 * The message is copied into
 * 'msg'.  No more than
 * 'nbytes' bytes will by copied
 * into this buffer; longer
 * messages will be truncated.
 */
void do_mbox_recv(mbox_t mbox, void *msg, int nbytes)
{
  (void)mbox;
  (void)msg;
  (void)nbytes;
		//enter_critical();
 //printf(14, 1, "in box recv");
  lock_acquire( &(MessageBoxen[mbox].BoxLock) );
  //printf(6, 1, "recever message");
		//leave_critical();
  while ( !MessageBoxen[mbox].next_buf  )
	condition_wait( &(MessageBoxen[mbox].BoxLock), &(MessageBoxen[mbox].Buf_full) );
  bcopy(MessageBoxen[mbox].buf, (char *)msg, nbytes);
  int i;
  for ( i = nbytes; i < MessageBoxen[mbox].next_buf; i++ ){
	  MessageBoxen[mbox].buf[i - nbytes] = MessageBoxen[mbox].buf[i];
  }
  MessageBoxen[mbox].next_buf -= nbytes;
  //printf(3, 1, "%d next_buf = %d", mbox, MessageBoxen[mbox].next_buf);
  //int i;
  //for ( i = 0; i < nbytes; i++ ){
  //    *(int8_t *)msg = MessageBoxen[mbox].buf.info[MessageBoxen[mbox].head++];
  //    ((int8_t *)msg)++;
  //}
  //MessageBoxen[mbox].Msg_byte -= nbytes;
  //MessageBoxen[mbox].head = (MessageBoxen[mbox].head + MAX_MBOX_LENGTH) % MAX_MBOX_LENGTH;
  //      enter_critical();
  //*(int *)msg = MessageBoxen[mbox].buf;
  //MessageBoxen[mbox].buf_used = 0;
  //      leave_critical();
  condition_signal(&(MessageBoxen[mbox].Buf_empty));
		//enter_critical();
  lock_release(&(MessageBoxen[mbox].BoxLock));
		//leave_critical();
  /* TODO */
}

//void printBoxname(){
//    printf(1, 1, "%s", MessageBoxen[0].name);
//    printf(2, 1, "%s", MessageBoxen[1].name);
//
//}
