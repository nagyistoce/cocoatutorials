#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int syntax() { printf("syntax: lister arg+\n") ; return 1 ; }

// linked list
typedef struct list_s list_t, *list_p ;
struct list_s {
	const char* data ;
	list_p 		next ;
	list_p  	last ;
} ;

void list_add(list_p list,const char* data)
{
	if ( !list ) return ;

	list_p node = (list_p) malloc(sizeof(list_t)) ; // new element
	if ( node ) {
		node->data = data       ;
		node->last = list->last ;
		node->next = NULL       ;
		
		if ( !list->next ) { 		 // first time
			list->next       = node ; // update the list
		} else {
			list->last->next = node ; // extend the chain
		}
		list->last  = node ;          // always update the list
	}
}

void list_reverse (list_p list)
{
	list_p node = list ;
	while (node) {
		list_p temp = node->next ;
		node->next = node->last ;
		node->last = temp       ;
		// stop in the middle, so we dont double reverse!
		node = node != temp ? temp : NULL ;
	}
}

void list_print(char* msg,list_p list)
{
	if ( !msg && !list ) return ;
	
	printf("%s: ",msg) ;
	list_p node = list->next ;
	while (node) {
		printf("%s ",node->data) ;
		node = node->next ;
	}
	printf("\n") ;
}

void list_free (list_p list)
{
	if ( !list ) return ;
	
	list_p node = list->next ;
	while (node) {
		list_p next = node->next ;
		free((void*) node) ;
		node = next ;
	}
}

int main(int argc, const char* argv[])
{
	int i ;
	list_t list = { NULL,NULL,NULL }; // list is on the stack

	for ( i = 1 ; i < argc ; i++ ) {
		list_add(&list,argv[i]) ;
	}

	list_print  ("forward",&list) ;
	list_reverse(&list) ;
	list_print  ("reverse",&list) ;
	
	// house keeping
	list_free(&list) ;
	
	return argc < 2 ? syntax() : 0 ;
}

