/* 
 * Mach Operating System
 * Copyright (c) 1994-1987 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator   or   Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they made and grant Carnegie Mellon the
 * rights to redistribute these changes.
 */
/*
 *
 * File: us/include/ns_types.h,v $
 *
 */

/*
 * Mach Name Server.
 *
 * Standard types used in Name Server requests.
 */

/*
 * HISTORY:
 * $Log:	ns_types.h,v $
 * Revision 1.19  94/07/08  15:51:31  mrt
 * 	Updated copyright.
 * 
 * Revision 1.18  92/07/05  23:23:14  dpj
 * 	Add NST_TASK_GROUP (Not currently used) and NST_TASK
 * 	[92/06/24  13:33:58  jms]
 * 	Eliminated nested struct declarations.
 * 	Removed definitions for access_table entries, and dependency
 * 	on mach_object.h.
 * 	[92/04/17  16:00:02  dpj]
 * 
 * Revision 1.17  92/03/05  14:55:15  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.
 * 	[92/02/26  16:50:45  jms]
 * 
 * Revision 1.16  91/05/05  19:23:55  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:43:52  dpj]
 * 
 * 	Added NSF_TFOLLOW, NSA_TFORWARD and NST_* for pipes and
 * 	network-related items.  
 * 	[91/04/28  09:30:28  dpj]
 * 
 * 	Added NST_CLTS and NST_COTS for network support.
 * 	[91/02/25  10:22:41  dpj]
 * 
 * Revision 1.15  89/10/30  16:28:20  dpj
 * 	Defined NST_UPIPE and NST_RESERVED.
 * 	Removed (temporarily) NST_BIPIPE.
 * 	[89/10/27  16:33:20  dpj]
 * 
 * Revision 1.14  89/06/30  18:30:32  dpj
 * 	Fixed definitions of NSF_FOLLOW and NSF_MOUNT.
 * 	Replaced NST_MOUNT with NST_MOUNTP.
 * 	Added NS_AUTHID_WILDCARD, NST_UNSPECIFIED.
 * 	[89/06/29  00:00:51  dpj]
 * 
 * Revision 1.13  89/05/17  16:04:12  dorr
 * 	include file cataclysm
 * 
 * Revision 1.12.1.1  89/05/15  11:28:11  dorr
 * 	create ns_identity_t
 * 
 * Revision 1.12  89/03/17  12:19:19  sanzi
 * 	add ns_offset_t
 * 	[89/03/02  14:22:55  dorr]
 * 	
 * 	Added NST_TTY.
 * 	[89/02/21  17:52:18  dpj]
 * 	
 * 	dlong -> dlong_t.
 * 	[89/02/20  15:44:36  dpj]
 * 	
 * 	add nsa_end.
 * 	[89/02/20  14:44:29  dorr]
 * 	
 * 	Define ns_size_t in terms of struct dlong.
 * 	Include dlong.h.
 * 	[89/02/17  15:33:39  sanzi]
 * 	
 * 	get rid of io_types.h (recursive include).  define ns_size_t,
 * 	in terms of which io_size_t & co can be defined.
 * 	[89/02/16  11:48:57  dorr]
 * 	
 * 	Fix size entry in struct ns_attr.  Add include of io_types.h.
 * 	[89/02/15  11:33:07  sanzi]
 * 	
 * 	ns_prot_len is a count of int's
 * 	[89/02/14  17:47:31  dorr]
 * 	
 * 	include mach_object.h (to get mach_method_id_t)
 * 	[89/02/14  11:23:15  dorr]
 * 
 */

#ifndef	_NS_TYPES_H_
#define	_NS_TYPES_H_

#include	<mach.h>
#include	<mach/time_value.h>
#include	<ns_error.h>
#include	<dlong.h>		/* for double long computations */
#include	<base.h>

#ifndef	NULL
#define	NULL	0
#endif	NULL

/*
 * Macro to clear a time_value. XXX Should be moved somewhere else.
 */
#define	CLEAR_TV(_tv) { (_tv).seconds = 0; (_tv).microseconds = 0; }


/*
 * Major sizes.
 */
#define	NS_PATHLEN	1024
#define	NS_NAMELEN	256

/*
 * Basic data types.
 */

typedef char			ns_path_t[NS_PATHLEN]; /* full path name */
typedef char			ns_name_t[NS_NAMELEN]; /* component of a
						       /* path name */
typedef unsigned int		ns_type_t;	/* object type */
typedef unsigned int		ns_access_t; 	/* set of access rights */
typedef mach_port_t		ns_token_t;	/* authentication token */
typedef mach_port_t		ns_identity_t;	/* authentication identity */
typedef int			ns_authid_t;	/* authentication ID */

/*
 * Size of I/O objects.
 *
 * XXX This type should normally only be defined inside the I/O interface
 * specification, but it is needed here for the standard attributes.
 */
typedef struct 	dlong_t		ns_size_t;


/*
 * Flags for "mode" argument in ns_resolve() primitive.
 */
typedef unsigned int		ns_mode_t;
#define	NSF_ACCESS		0x1	/* simply check access, do not */
					/* return the terminal object */
#define	NSF_FOLLOW		0x2	/* TRUE: follow symlink */
					/* at end of path */
					/* FALSE: return symlink object */
					/* itself at end of path */
#define	NSF_MOUNT		0x4	/* TRUE: apply mount point */
					/* at end of path */
					/* FALSE: return mount point object */
					/* itself at end of path */
#define	NSF_TFOLLOW		0x8	/* TRUE: follow transparent symlink */
					/* at end of path */
					/* FALSE: return tsymlink object */
					/* itself at end of path */
#define	NSF_RESERVED_1		0x10	/* reserved. Future use: */
					/* NSF_WILDCARD : allow wildcards */

#define	NSF_FOLLOW_ALL		(NSF_FOLLOW | NSF_TFOLLOW | NSF_MOUNT)

/*
 * Flags for "action" argument in ns_resolve() primitive.
 */
typedef unsigned int		ns_action_t;
#define	NSA_END			0x0	/* all set */
#define	NSA_AUTHENTICATE	0x2	/* authenticate new object */
#define	NSA_NOCACHE		0x4	/* do not cache new object */
#define	NSA_FORWARD		0x1
				/* restart at normal forwarding point */
#define	NSA_TFORWARD		0x8
				/* restart at transparent forwarding point */


/*
 * One entry in a standard Access Control List.
 *
 * The ACL contains a list of pairs
 * (authentication ID, allowed access rights).
 * Unused entries have a null "rights" field.
 *
 * The ACL is of variable size, with no limit.
 *
 * The total allowed rights for a given set of credentials correspond to
 * the union of all the rights from all the pairs that match an
 * authentication ID in the credentials structure.
 * The special authid 0 in an ACL entry matches all authentication IDs.
 * There are no negative access rights.
 */
typedef struct ns_acl_entry {
	ns_authid_t    		authid;
	ns_access_t		rights;
} *ns_acl_entry_t;
#define	NS_AUTHID_WILDCARD	0


/*
 * Standard protection structure.
 *
 * The protection structure contains the ACL, plus a generation number
 * used to deal with concurrent modifications of this structure by several
 * clients. See the ns_set_protection() primitive for details.
 *
 * Although the ACL is of variable size, and therefore the ns_prot is
 * also of variable size, the default allocation size for a new ns_prot is
 * DEFAULT_NS_PROT_LEN.
 */
#define	NS_PROT_VERSION		1
struct ns_prot_head {
	unsigned int		version;
	unsigned int		generation;
	unsigned int		acl_len;	/* number of entries in ACL */
};
typedef struct ns_prot {
	struct ns_prot_head 	head;
	struct ns_acl_entry	acl[1];		/* variable size */
} *ns_prot_t;
#define	NS_PROT_NULL		(ns_prot_t)0
#define	NS_PROT_SIZE(prot)	(sizeof((prot)->head) +				\
			    	((prot)->head.acl_len * sizeof((prot)->acl[0])))
#define	NS_PROT_LEN(prot_len)	((sizeof(struct ns_prot_head) + 		\
			    	((prot_len) * sizeof(struct ns_acl_entry)))/sizeof(int))
#define	DEFAULT_NS_PROT_LEN	NS_PROT_LEN(16)


/*
 * Limited protection structure, for inclusion in attributes structure.
 * This structure contains the first 4 non-null entries in the full
 * ns_prot structure. If there are less than 4 valid entries, extraneous
 * entries in this acl have a null "rights" field.
 */
typedef struct ns_prot_ltd {
	struct ns_acl_entry	acl[4];
} *ns_prot_ltd_t;
  

/*
 * Standard credentials structure.
 *
 * This structure, returned by the authentication upon presentation
 * of an authentication token, is a list of all the authentication IDs
 * corresponding to this token. Unused entries are set to zero.
 *
 * The list is of variable size, with no limit. However, the default
 * allocation size for a new ns_cred is NS_CRED_SIZE.
 */
#define	NS_CRED_VERSION		1
struct ns_cred_head {
	unsigned int		version;
	unsigned int		cred_len;	/* number of entries in list */
};
typedef struct ns_cred {
	struct ns_cred_head	head;
	ns_authid_t		authid[1];	/* variable size */
} *ns_cred_t;
#define	NS_CRED_NULL		(ns_cred_t)0
#define	NS_CRED_SIZE(cred)	(sizeof((cred)->head) +			\
				((cred)->head.cred_len * sizeof((cred)->authid[0])))
#define	NS_CRED_LEN(cred_len)	((sizeof(struct ns_cred_head) + 		\
				((cred_len) * sizeof(ns_authid_t)))/sizeof(int))
#define	DEFAULT_NS_CRED_LEN	NS_CRED_LEN(16)


/*
 * Manager ID for objects.
 *
 * This ID is part of the attributes for any object. It characterizes the
 * server implementing that object, and must be the same for all objects
 * managed by the same server.
 *
 * This ID should be unique across all servers participating in the global
 * name space. Normally, "v1" is the IP address of the host on which the
 * server is running, and "v2" is a host-wide unique number.
 */
typedef struct ns_mgr_id {
	long			v1;
	long			v2;
} ns_mgr_id_t;


/*
 * Object ID for objects.
 *
 * This ID is part of the attributes for any object. It characterizes that
 * object amoung all objects managed by the same server, so that the pair
 * (mgr_id, obj_id) is unique across all objects in the global name space.
 */
typedef	long			ns_obj_id_t;


/* 
 * List of names returned by ns_list_entries()
 */
typedef ns_name_t	*ns_name_list_t;

/*
 * Entry info returned by ns_list_entries().
 */
typedef struct ns_entry {
	ns_type_t		type;		/* object type */
	ns_obj_id_t		obj_id;		/* object ID */
} *ns_entry_t;


/*
 * Standard attributes structure for objects.
 *
 * This structure lists some attributes of general interest for
 * many types of objects. Every object must return one on demand (through
 * ns_get_attributes()), although it may mark some fields as "invalid"
 * if the corresponding attribute is not supported by that type of object.
 */
#define	NS_ATTR_VERSION	1
typedef	struct ns_attr {
	unsigned int		version;	/* version # */
	unsigned int		valid_fields;	/* one bit for each field */
	ns_type_t		type;		/* object type */
	unsigned int		nlinks;		/* number of links from */
						/* containing directories */
						/* = reference count on the */
						/* object when it is not */
						/* active (open) */
	ns_mgr_id_t		mgr_id;		/* manager ID */
	ns_obj_id_t		obj_id;		/* object ID within */
						/* the server */
	time_value_t		access_time;	/* last access time */
	time_value_t		modif_time;	/* last modification time */
	time_value_t		creation_time;	/* object creation time */
	ns_size_t		size;		/* size for I/O component */
						/* of the object, in bytes */
	struct ns_prot_ltd	prot_ltd;	/* abridged protection */
						/* information */
} *ns_attr_t;

/*
 * Definitions for fields in the valid_fields entry.
 *
 * Each bit that is TRUE indicates that the corresponding field is
 * valid in the ns_attr structure returned by the object.
 */
#define	NS_ATTR_TYPE		0x1
			 /*     0x2	*/
#define	NS_ATTR_NLINKS		0x4
#define	NS_ATTR_ID		0x8	/* both mgr and obj IDs */
#define	NS_ATTR_ATIME		0x10
#define	NS_ATTR_MTIME		0x20
#define	NS_ATTR_CTIME		0x40
#define	NS_ATTR_SIZE		0x80
#define	NS_ATTR_PROT		0x100

/*
 * Common attributes combinations.
 */
#define	NS_ATTR_ALL		0x1ff
#define	NS_ATTR_MIN		(NS_ATTR_TYPE | NS_ATTR_NLINKS | NS_ATTR_ID)



/****************************************************************************/


/*
 * Common object types (ns_type_t).
 *
 * These types represent the common types supported by the standard system.
 *
 * If new types are needed for some specific emulation library, that
 * cannot be implemented with existing types, they should be added here.
 *
 * Other types may be dynamically defined if the system supports a
 * Type Server, but such functionality is not part of the minimum
 * standard.
 */
#define	NST_INVALID		0
#define	NST_DIRECTORY		1	/* generic directory */
#define	NST_SYMLINK		2	/* pure symbolic link */
#define	NST_MOUNTPT		3	/* pure mount point */
#define	NST_FORWARD		4	/* complex forwarding object */
#define	NST_FILE		5	/* random-access file */
#define	NST_TTY			6	/* TTY */
#define	NST_UPIPE_BYTES		7	/* unidirectional pipe w/ bytes */
#define	NST_UPIPE_RECS		8	/* unidirectional pipe w/ records */
			/* connection-less transport endpoint */
#define	NST_CLTS_BYTES		9
#define	NST_CLTS_RECS		10
			/* connection-oriented transport endpoint */
#define	NST_COTS_BYTES		11
#define	NST_COTS_RECS		12
#define	NST_CONNECTOR		13
#define	NST_TRANSPARENT_SYMLINK	14
#define	NST_TASK		15
#define	NST_TASK_GROUP		16
#define	NST_RESERVED		0xfffffffe	/* reserved entry */
#define	NST_UNSPECIFIED		0xffffffff	/* unspecified type */


/*
 * Standard access rights for the common types (ns_access_t).
 *
 * Each right may not be meaningful for each object type, but they
 * have the exact same meaning for all types for which they are meaningful.
 *
 * If there are many standard types, or if there are dynamically defined
 * types, the union of all possible operations on all types may require
 * more than 32 access rights. In this case, it may not be possible
 * for the same logical right to have the same representation for
 * all types for which it is applicable. A Type Server should then
 * be used to manage those rights. In no conditions may there be more than
 * 32 access rights defined for any one object type.
 *
 * For the standard system, there are less than 32 access rights for
 * the set of all object types, so their definition can be simplified.
 *
 * These rights should be registered with the Type Server if there is one.
 */
#define	NSR_ADMIN		0x1	/* change protection on object */
#define	NSR_LOOKUP		0x2	/* lookup entry in directory */
#define	NSR_INSERT		0x4	/* insert entry in directory */
#define	NSR_DELETE		0x8	/* remove entry from directory */
#define	NSR_READ		0x10	/* read data from I/O object */
#define	NSR_WRITE		0x20	/* write data into I/O object */
#define	NSR_EXECUTE		0x40	/* dummy execute right. */
					/* not enforced, but used to tag */
					/* executable files */
#define	NSR_REFERENCE		0x80	/* just reference the obj */
					/* for ns_authenticate(), no */
					/* other privilege implied */
#define	NSR_GETATTR		0x100	/* get attributes and protection */


/*
 * Common rights combinations.
 */
#define	NSR_NONE		0x0	/* no access */
#define	NSR_ALL			0x1ff	/* all access */


#endif	_NS_TYPES_H_
