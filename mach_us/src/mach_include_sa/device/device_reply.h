#ifndef	_device_reply_user_
#define	_device_reply_user_

/* Module device_reply */

#include <mach/kern_return.h>
#if	(defined(__STDC__) || defined(c_plusplus)) || defined(LINTLIBRARY)
#include <mach/port.h>
#include <mach/message.h>
#endif

#include <mach/std_types.h>
#include <device/device_types.h>
#include <device/net_status.h>

/* SimpleRoutine device_open_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t ds_device_open_reply
#if	defined(LINTLIBRARY)
    (reply_port, reply_portPoly, return_code, device_port)
	mach_port_t reply_port;
	mach_msg_type_name_t reply_portPoly;
	kern_return_t return_code;
	mach_port_t device_port;
{ return ds_device_open_reply(reply_port, reply_portPoly, return_code, device_port); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	mach_port_t device_port
);
#else
    ();
#endif
#endif

/* SimpleRoutine device_write_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t ds_device_write_reply
#if	defined(LINTLIBRARY)
    (reply_port, reply_portPoly, return_code, bytes_written)
	mach_port_t reply_port;
	mach_msg_type_name_t reply_portPoly;
	kern_return_t return_code;
	int bytes_written;
{ return ds_device_write_reply(reply_port, reply_portPoly, return_code, bytes_written); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	int bytes_written
);
#else
    ();
#endif
#endif

/* SimpleRoutine device_write_reply_inband */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t ds_device_write_reply_inband
#if	defined(LINTLIBRARY)
    (reply_port, reply_portPoly, return_code, bytes_written)
	mach_port_t reply_port;
	mach_msg_type_name_t reply_portPoly;
	kern_return_t return_code;
	int bytes_written;
{ return ds_device_write_reply_inband(reply_port, reply_portPoly, return_code, bytes_written); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	int bytes_written
);
#else
    ();
#endif
#endif

/* SimpleRoutine device_read_reply */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t ds_device_read_reply
#if	defined(LINTLIBRARY)
    (reply_port, reply_portPoly, return_code, data, dataCnt)
	mach_port_t reply_port;
	mach_msg_type_name_t reply_portPoly;
	kern_return_t return_code;
	io_buf_ptr_t data;
	mach_msg_type_number_t dataCnt;
{ return ds_device_read_reply(reply_port, reply_portPoly, return_code, data, dataCnt); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	io_buf_ptr_t data,
	mach_msg_type_number_t dataCnt
);
#else
    ();
#endif
#endif

/* SimpleRoutine device_read_reply_inband */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t ds_device_read_reply_inband
#if	defined(LINTLIBRARY)
    (reply_port, reply_portPoly, return_code, data, dataCnt)
	mach_port_t reply_port;
	mach_msg_type_name_t reply_portPoly;
	kern_return_t return_code;
	io_buf_ptr_inband_t data;
	mach_msg_type_number_t dataCnt;
{ return ds_device_read_reply_inband(reply_port, reply_portPoly, return_code, data, dataCnt); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	io_buf_ptr_inband_t data,
	mach_msg_type_number_t dataCnt
);
#else
    ();
#endif
#endif

#endif	_device_reply_user_
