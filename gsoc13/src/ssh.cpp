#include "ssh.hpp"
#if EXV_USE_LIBSSH == 1
// class member definitions
namespace Exiv2 {
    SSH::SSH(const std::string& host, const std::string& user, const std::string& pass):host_(host),user_(user),pass_(pass) {
        int rc;
        session_ = ssh_new();
        if (session_ == NULL) {
            throw Error(1, "Unable to create the the ssh session");
        }
        // try to connect
        ssh_options_set(session_, SSH_OPTIONS_HOST, host_.c_str());
        ssh_options_set(session_, SSH_OPTIONS_USER, user_.c_str());
        rc = ssh_connect(session_);
        if (rc != SSH_OK) {
            throw Error(1, ssh_get_error(session_));
        }
        // Authentication
        rc = ssh_userauth_password(session_, NULL, pass_.c_str());
        if (rc != SSH_AUTH_SUCCESS) {
            throw Error(1, ssh_get_error(session_));
        }
    }

    int SSH::RunCommand(const std::string& cmd, std::string* output) {
        int rc;
        ssh_channel channel;
        channel = ssh_channel_new(session_);
        if (channel == NULL) {
           rc = SSH_ERROR;
        } else {
            rc = ssh_channel_open_session(channel);
            if (rc != SSH_OK) {
                ssh_channel_free(channel);
            } else {
                char buffer[256];
                rc = channel_request_exec(channel, cmd.c_str());
                if (rc > 0) {
                    rc = -1;
                } else {
                    while ((rc = channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
                        output->append(buffer, sizeof(buffer));
                    }
                }
                ssh_channel_send_eof(channel);
                ssh_channel_close(channel);
                ssh_channel_free(channel);
            }
        }
        return rc;
    }

    SSH::~SSH() {
        ssh_disconnect(session_);
        ssh_free(session_);
    }
}
#endif
