@begin(verbatim)
Global problems that need to be addressed:

	QUOTAs
	CS_RPAUSE
	VICE 
	CS_BUGFIX	
	
mach_directory.c:

	Sticky bit handling on directories. (ISVTX).
		If (sticky directory && !suser && !owner_of_directory
			&& !owner_of_file) then cannot delete from dir.

	".." handling in namei().  Should never be called with complex
		pathname under new scheme.  This should not be a problem
		provided it is handled by the caller of fs_lookup().
		
	Symbolic link handling.  If pathnames are parsed
	one component at a time and always go through the
	fs_lookup() interface, we will never do this.  
	
	Question: Should this code ever call access()?  I think not.

sys_inode.c:	
	Handling of Character devices, see:
		ino_ioctl()   	
		ino_select()  	
		ino_close() 	
		rwip()
		openi():  only opens regular files.
				
	ino_close: Close on block device.
	ino_lock:  Attempt to interrupt a lock request on a file.
			--> reference to u.u_qsave.
		Solution:
			implement locked open mode:
				Fails on open if already locked or open for
					write.
				Emulation library sleeps waiting if
					necessary.
		
	vhangup: references suser, controlling tty, process group signalling.
	ino_flush: MACH_NBC only.  Can ufs-server use it?

	rwip():  signalling of file size resource limit, 
		 must check on a per-user basis.

	rwip():	 read-ahead block handling.

	rwip():	 clearing of setgid/setuid bits when file written by !suser.

ufs_alloc.c:

	alloc(),
	realloccg(): Exceeding freespace() limit on filesystem when super-user.
	
	realloccg(): hit on buffer cache for a B_DELWRI block causes
			u.u_ru.ru_oublock-- (??) 
			
ufs_bio.c:
	
	bread():  memory leak calling block_read() and not deallocating
		result.
	breada(): u.u_ru.ru_inblock accounting.
	
	bwrite(): u.u_ru.ru_oublock accounting.
	bwrite(): asynchronous block writes.

	bdwrite(): delayed write u.u_ru_oublock accounting.
	
	biowait(): handling of error on i/o in biowait().
	
ufs_bmap.c:
	
	bmap():	rablock and rasize handling.	


ufs_export.c:

	chown1(): sticky bit handling when changing protection on a file.
	chown1(): reference to suser(), reference to *nyi* groupmember
	
	fs_remove(): attempt to unlink a mounted file
	
	OWNER(): macro has reference to suser() in it.  Removed.
	
ufs_fio.c:
	
	access(): references u.u_uid, u.u_gid, and u.u_groups.  If this
		code is ever needed, it should be called with a credentials
		structure and the comparisons made against that.
		
ufs_inode.c:

	iget(): references to ip->vm_info->pager

	iget(): calls cacheinval(ip).
	
	irele(): inode_pager_active().
	
	itrunc(): inode_uncache(oip).
		
ufs_mount.c:

	mountfs(): reference to block device switch.z
	mountfs(): for bread() errors of fs_csaddr (cyclinder group
		summary information area) we leak the memory.
	mountfs(): calls cacheinval().
	mountfs(): calls inode_swap_preference().	
	mountfs(): would like to close the device if mount error occurs.
	
	unmount1(): mfs_cache_clear(), vm_object_cache_clear(), 
	unmount1(): stillopen handling would like to call device close
	getmdev(): reference to suser() to insure mounting proper fs.
	
@end(verbatim)
