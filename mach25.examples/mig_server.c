/*
 *      mig_server-Routine aus [Walm89a]          'mig_server.c'
 *      mig_server subroutine, out of [Walm89a]
 */

#include <mach.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/std_types.h>
#include <stdio.h>

msg_return_t mig_server(port_set_name_t service, boolean_t
                (*function)())
{
        /*
         *      Buffers should be aligned on 4-byte boundaries,
         *      so that internal fields are aligned properly
         *      for int fetches and stores.
         */

        int requestbuf[MSG_SIZE_MAX/sizeof(int)];
        int replybuf[MSG_SIZE_MAX/sizeof(int)];

        msg_header_t *request = (msg_header_t *) requestbuf;
        death_pill_t *reply = (death_pill_t *) replybuf;
        msg_return_t mr;

        /*
         *   Problems with this server loop:
         *      Requests which are not processed successfully
         *      (bad msg_id, type mismatch, whatever) should
         *      be cleaned up; ports & memory deallocated.
         *
         *      Replies which are dropped (reply port died or was
         *      full, some problem with rights or the memory in the 
         *      reply)should also be cleaned up.
         * But these are hard problems (harder than they might 
         * appear) so we ignore them.
         */


        for (;;) {
                /* receive a request message */

                request->msg_size = sizeof(requestbuf);
                request->msg_local_port = service;

                mr = msg_receive(request, MSG_OPTION_NONE, 0);
                if (mr != RCV_SUCCESS)
                        return mr;

                /* ignore notification messages from the kernel */

                if (request->msg_local_port == task_notify())
                        continue;

                /* demux and process the request, generating a reply */

                (void) (*function)(request, &reply->Head);

                /* send the reply if necessary */

                if ((reply->Head.msg_remote_port != PORT_NULL) &&
                    (reply->RetCode != MIG_NO_REPLY)) {
                        /* don't block if the reply port is full */

                        (void) msg_send(&reply->Head, SEND_TIMEOUT, 0);
                }
        }
}
