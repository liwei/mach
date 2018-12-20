/*
 * cas_defs.h
 *	internal definitions for the central authentication service.
 *
 */

/*
 * HISTORY:
 * $Log:	cas_defs.h,v $
 * Revision 1.2  89/05/17  16:54:22  dorr
 * 	add extern def for lookup_id()
 * 	[89/05/15  12:23:33  dorr]
 * 
 *  2-Mar-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#ifndef	_CAS_DEFS_
#define	_CAS_DEFS_

#define CAS_DEBUG		0

#define	ENCRYPT_PASSWORDS	1

#include <mach/boolean.h>

#include "auth_defs.h"

#define MEMBERS_MAX		16

typedef struct {
    group_name_t	group_name;
    group_id_t		group_id;
    group_type_t	group_type;
    full_name_t		full_name;
    group_id_t		owner_id;
    pass_word_t		pass_word;	/* Should be encrypted. */
    boolean_t		touched;	/* Used for transitive closure calculations. */
    group_id_list_t	tr_memberships;	/* Memberships computed by transitive closure. */
    int			tr_memberships_count;
    int			members_count;
    group_id_t		members[MEMBERS_MAX];
    int			memberships_count;
    group_id_t		memberships[MEMBERS_MAX];
} group_record_t, *group_record_ptr_t;

#define GROUP_RECORD_NULL	((group_record_ptr_t)0)

extern group_record_ptr_t	group_id_map[GROUP_ID_MAX];

#if	ENCRYPT_PASSWORDS
#define PW_EQUAL(p1,p2) (				\
    ((p1)[0] == (p2)[0]) && ((p1)[1] == (p2)[1])	\
    && ((p1)[2] == (p2)[2]) && ((p1)[3] == (p2)[3])	\
    && ((p1)[4] == (p2)[4]) && ((p1)[5] == (p2)[5])	\
    && ((p1)[6] == (p2)[6]) && ((p1)[7] == (p2)[7])	\
    && ((p1)[8] == (p2)[8]) && ((p1)[9] == (p2)[9])	\
    && ((p1)[10] == (p2)[10]) && ((p1)[11] == (p2)[11])	\
    && ((p1)[12] == (p2)[12]) && ((p1)[13] == (p2)[13])	\
    && ((p1)[14] == (p2)[14]) && ((p1)[15] == (p2)[15])	\
)
#else	ENCRYPT_PASSWORDS
#define PW_EQUAL(p1,p2) ((strcmp((p1),(p2))) == 0)
#endif	ENCRYPT_PASSWORDS


/*
 * Public functions provided by cas_utils.
 */
extern boolean_t cas_utils_init();

extern void delete_name();
/*
group_name_t	group_name;
*/

extern void enter_name();
/*
group_record_ptr_t	group_record_ptr;
*/

extern group_record_ptr_t lookup_name();
/*
group_name_t	group_name;
*/

extern group_record_ptr_t lookup_id();
/*
group_id_t	group_id;
*/

extern boolean_t check_for_membership();
/*
group_id_t		group_id;
group_record_ptr_t	group_rec_ptr;
*/

void delete_from_list();
/*
group_id_list_t	group_id_list;
int		*count_ptr;
group_id_t	delete_group;
*/

extern void do_transitive_closure();
/*
group_record_ptr_t	tr_rec_ptr;
boolean_t		do_memberships;
group_id_list_t		*group_ids_ptr;
int			*group_ids_count_ptr;
*/

#if	ENCRYPT_PASSWORDS
extern void encrypt_password();
/*
pass_word_t	password;
*/
#endif	ENCRYPT_PASSWORDS


/*
 * Public functions provided by cas_db.
 */
extern boolean_t read_database();

extern void save_database();


/*
 * Public functions provided by cas_procs.
 */
extern void cas_handle_port_death();
/*
port_t		dead_port;
*/


#endif	_CAS_DEFS_
