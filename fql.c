/*
 * Copyright (c) 2018, Heinrich Hartmann
 * Copyright (c) 2016, Circonus, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name Circonus, Inc. nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <mtev_defines.h>
#include <mtev_conf.h>
#include <mtev_dso.h>
#include <mtev_main.h>
#include <mtev_memory.h>
#include <eventer/eventer.h>
#include <mtev_fq.h>

#include <fq.h>

#include <stdio.h>
#include <getopt.h>

#define APPNAME "fql"
#define CONFIG_FILE "fql.conf"

static int debug = 0;
static int foreground = 0;
static char *droptouser = NULL, *droptogroup = NULL;
static mtev_log_stream_t fqlog;

static int
usage(const char *prog) {
  fprintf(stderr, "%s [-D] [-d]\n", prog);
  fprintf(stderr, "\t-D\t\trun in the foreground (don't daemonize)\n");
  fprintf(stderr, "\t-d\t\tturn on debugging\n");
  fprintf(stderr, "\t-h\t\tshow this message\n");
  return 2;
}

static void
parse_cli_args(int argc, char * const *argv) {
  int c;
  while((c = getopt(argc, argv, "Ddh")) != EOF) {
    switch(c) {
    case 'd':
      debug++;
      break;
    case 'D':
      foreground++;
      break;
    case 'h':
      usage(argv[0]);
      break;
    }
  }
}

static mtev_hook_return_t
on_msg_received(void *closure, fq_client client, int connection_id, fq_msg *msg, void * a, size_t b) {
  mtevL(fqlog, "[%.*s] %.*s", msg->sender.len, msg->sender.name, msg->payload_len, msg->payload);
  return MTEV_HOOK_CONTINUE;
}

static int
child_main(void) {
  eventer_init();
  mtev_dso_init();
  mtev_dso_post_init();
  mtev_fq_handle_message_hook_register("fqc", on_msg_received, NULL);
  fqlog = mtev_log_stream_find("fqlog");

  mtevL(mtev_stderr, "Ready.\n");

  /* Lastly, spin up the event loop */
  eventer_loop();
  return 0;
}

int main(int argc, char **argv) {
  parse_cli_args(argc, argv);
  mtev_memory_init();
  mtev_main(APPNAME, CONFIG_FILE, debug, foreground,
            MTEV_LOCK_OP_LOCK, NULL, droptouser, droptogroup,
            child_main);
  return 0;
}
