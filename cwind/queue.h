#ifndef	_SYS_QUEUE_H_
#define	_SYS_QUEUE_H_


/*
 * Tail queue definitions.
 */
#define	TAILQ_HEAD(type)					\
	type *_first;		/* first element */		\
	type **_last;	/* addr of last next element */

#define	TAILQ_HEAD_INITIALIZER(head) \
	{ NULL, &(head)._first }

#define	TAILQ_ENTRY(type)					\
	type *_next;		/* next element */		\
	type **_prev;	/* address of previous next element */

/*
 * Tail queue functions.
 */
#define	TAILQ_INIT(head) 						\
	(head)->_first = NULL;					        \
	(head)->_last = &(head)->_first;

#define	TAILQ_INSERT_HEAD(head, elm) 			\
	if (((elm)->_next = (head)->_first) != NULL)	\
		(head)->_first->_prev =	 &(elm)->_next;		\
	else								            \
		(head)->_last = &(elm)->_next;		        \
	(head)->_first = (elm);					        \
	(elm)->_prev = &(head)->_first;

#define	TAILQ_INSERT_TAIL(head, elm) 			\
	(elm)->_next = NULL;					\
	(elm)->_prev = (head)->_last;			\
	(head)->_last = &(elm)->_next;

#define	TAILQ_INSERT_AFTER(head, listelm, elm) 		\
	if (((elm)->_next = (listelm)->_next) != NULL)\
		(elm)->_next->_prev = 		\
		    &(elm)->_next;				\
	else								\
		(head)->_last = &(elm)->_next;		\
	(listelm)->_next = (elm);				\
	(elm)->_prev = &(listelm)->_next;

#define	TAILQ_INSERT_BEFORE(listelm, elm) 			\
	(elm)->_prev = (listelm)->_prev;		\
	(elm)->_next = (listelm);				\
	*(listelm)->_prev = (elm);				\
	(listelm)->_prev = &(elm)->_next;


#define TAILQ_REMOVE_HEAD(head) \
    if (((head)->_first = (head)->_first->_next) == NULL) \
(head)->_last = &(head)->_first;


#define	TAILQ_REMOVE(head, elm) 				\
	if (((elm)->_next) != NULL)				\
		(elm)->_next->_prev = 		\
		    (elm)->_prev;				\
	else								\
		(head)->_last = (elm)->_prev;		\
	*(elm)->_prev = (elm)->_next;

#define	TAILQ_FOREACH(var, head)					\
	for ((var) = ((head)->_first);				\
		(var);							\
		(var) = ((var)->_next))

#define	TAILQ_FOREACH_REVERSE(var, head, headname)		\
	for ((var) = (*(((struct headname *)((head)->_last))->_last));	\
		(var);							\
		(var) = (*(((struct headname *)((var)->_prev))->_last)))

/*
 * Tail queue access methods.
 */
#define	TAILQ_EMPTY(head)		((head)->_first == NULL)
#define	TAILQ_FIRST(head)		((head)->_first)
#define	TAILQ_NEXT(elm)		((elm)->_next)

#define	TAILQ_LAST(head, headname) \
	(*(((struct headname *)((head)->_last))->_last))
#define	TAILQ_PREV(elm, headname) \
	(*(((struct headname *)((elm)->_prev))->_last))


#endif

