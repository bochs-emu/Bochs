/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
/*
 * QEMU compatibility functions
 *
 * Copyright (c) 2003-2008  Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "slirp.h"

#if BX_NETWORKING && BX_NETMOD_SLIRP_NEW

void pstrcpy(char *buf, int buf_size, const char *str)
{
    int c;
    char *q = buf;

    if (buf_size <= 0)
        return;

    for(;;) {
        c = *str++;
        if (c == 0 || q >= buf + buf_size - 1)
            break;
        *q++ = c;
    }
    *q = '\0';
}

void qemu_set_nonblock(int fd)
{
#ifdef WIN32
    unsigned long opt = 1;
    ioctlsocket(fd, FIONBIO, &opt);
#else
    int f;
    f = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, f | O_NONBLOCK);
#endif
}

#ifdef WIN32
int inet_aton(const char *cp, struct in_addr *ia)
{
  uint32_t addr = inet_addr(cp);
  if (addr == 0xffffffff) {
    return 0;
  }
  ia->s_addr = addr;
  return 1;
}
#endif

int socket_set_fast_reuse(int fd)
{
#ifndef WIN32
    int val = 1, ret;

    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                     (const char *)&val, sizeof(val));

    assert(ret == 0);

    return ret;
#else
    return 0;
#endif
}

int socket_set_nodelay(int fd)
{
    int v = 1;
    return qemu_setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v));
}

void qemu_set_cloexec(int fd)
{
#ifndef WIN32
    int f;
    f = fcntl(fd, F_GETFD);
    fcntl(fd, F_SETFD, f | FD_CLOEXEC);
#endif
}

/*
 * Opens a socket with FD_CLOEXEC set
 */
int qemu_socket(int domain, int type, int protocol)
{
    int ret;

#ifdef SOCK_CLOEXEC
    ret = socket(domain, type | SOCK_CLOEXEC, protocol);
    if (ret != -1 || errno != EINVAL) {
        return ret;
    }
#endif
    ret = socket(domain, type, protocol);
    if (ret >= 0) {
        qemu_set_cloexec(ret);
    }

    return ret;
}

#ifndef WIN32

#define CONFIG_SMBD_COMMAND "/usr/sbin/smbd"

#include <pwd.h>

/* automatic user mode samba server configuration */
void slirp_smb_cleanup(Slirp *s, char *smb_tmpdir)
{
    char cmd[128];
    char error_msg[256];
    int ret;

    if (smb_tmpdir[0] != '\0') {
        snprintf(cmd, sizeof(cmd), "rm -rf %s", smb_tmpdir);
        ret = system(cmd);
        if (ret == -1 || !WIFEXITED(ret)) {
            snprintf(error_msg, sizeof(error_msg), "'%s' failed.", cmd);
            slirp_warning(s, error_msg);
        } else if (WEXITSTATUS(ret)) {
            snprintf(error_msg, sizeof(error_msg), "'%s' failed. Error code: %d",
                     cmd, WEXITSTATUS(ret));
            slirp_warning(s, error_msg);
        }
        smb_tmpdir[0] = '\0';
    }
}

int slirp_smb(Slirp *s, char *smb_tmpdir, const char *exported_dir,
              struct in_addr vserver_addr)
{
    static int instance;
    int i;
    char smb_conf[128], smb_cmdline[128];
    char share[64], error_msg[256];
    struct passwd *passwd;
    FILE *f;

    passwd = getpwuid(geteuid());
    if (!passwd) {
        sprintf(error_msg, "failed to retrieve user name");
        slirp_warning(s, error_msg);
        return -1;
    }

    if (access(CONFIG_SMBD_COMMAND, F_OK)) {
        sprintf(error_msg, "could not find '%s', please install it",
                CONFIG_SMBD_COMMAND);
        slirp_warning(s, error_msg);
        return -1;
    }

    if (access(exported_dir, R_OK | X_OK)) {
        snprintf(error_msg, sizeof(error_msg), "error accessing shared directory '%s': %s",
                 exported_dir, strerror(errno));
        slirp_warning(s, error_msg);
        return -1;
    }

    i = strlen(exported_dir) - 2;
    while ((i > 0) && (exported_dir[i] != '/')) i--;
    snprintf(share, sizeof(share), "%s", &exported_dir[i+1]);
    if (share[strlen(share)-1] == '/') share[strlen(share)-1] = '\0';

    snprintf(smb_tmpdir, 128, "/tmp/bochs-smb.%ld-%d",
             (long)getpid(), instance++);
    if (mkdir(smb_tmpdir, 0700) < 0) {
        snprintf(error_msg, sizeof(error_msg), "could not create samba server dir '%s'",
                 smb_tmpdir);
        slirp_warning(s, error_msg);
        return -1;
    }
    snprintf(smb_conf, sizeof(smb_conf), "%s/%s", smb_tmpdir, "smb.conf");

    f = fopen(smb_conf, "w");
    if (!f) {
        slirp_smb_cleanup(s, smb_tmpdir);
        snprintf(error_msg, sizeof(error_msg), "could not create samba server configuration file '%s'",
                 smb_conf);
        slirp_warning(s, error_msg);
        return -1;
    }
    fprintf(f,
            "[global]\n"
            "private dir=%s\n"
            "socket address=127.0.0.1\n"
            "pid directory=%s\n"
            "lock directory=%s\n"
            "state directory=%s\n"
            "log file=%s/log.smbd\n"
            "smb passwd file=%s/smbpasswd\n"
            "security = user\n"
            "map to guest = Bad User\n"
            "[%s]\n"
            "path=%s\n"
            "read only=no\n"
            "guest ok=yes\n"
            "force user=%s\n",
            smb_tmpdir,
            smb_tmpdir,
            smb_tmpdir,
            smb_tmpdir,
            smb_tmpdir,
            smb_tmpdir,
            share,
            exported_dir,
            passwd->pw_name
            );
    fclose(f);

    snprintf(smb_cmdline, sizeof(smb_cmdline), "%s -s %s",
             CONFIG_SMBD_COMMAND, smb_conf);

    if (slirp_add_exec(s, 0, smb_cmdline, &vserver_addr, 139) < 0 ||
        slirp_add_exec(s, 0, smb_cmdline, &vserver_addr, 445) < 0) {
        slirp_smb_cleanup(s, smb_tmpdir);
        sprintf(error_msg, "conflicting/invalid smbserver address");
        slirp_warning(s, error_msg);
        return -1;
    }
    return 0;
}
#endif

#endif
