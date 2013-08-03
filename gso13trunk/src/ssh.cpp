#include "ssh.hpp"
#if EXV_USE_SSH == 1

// class member definitions
namespace Exiv2 {
    SSH::SSH(const std::string& host, const std::string& user, const std::string& pass):
        host_(host),user_(user),pass_(pass),sftp_(0) {
        session_ = ssh_new();
        if (session_ == NULL) {
            throw Error(1, "Unable to create the the ssh session");
        }

        long timeout = 30; // seconds
        // try to connect
        ssh_options_set(session_, SSH_OPTIONS_HOST, host_.c_str());
        ssh_options_set(session_, SSH_OPTIONS_USER, user_.c_str());
        ssh_options_set(session_, SSH_OPTIONS_TIMEOUT, &timeout);
        if (ssh_connect(session_) != SSH_OK) {
            throw Error(1, ssh_get_error(session_));
        }
        // Authentication
        if (ssh_userauth_password(session_, NULL, pass_.c_str()) != SSH_AUTH_SUCCESS) {
            throw Error(1, ssh_get_error(session_));
        }
    }

    int SSH::runCommand(const std::string& cmd, std::string* output) {
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
                rc = ssh_channel_request_exec(channel, cmd.c_str());
                if (rc == SSH_OK) {
                    while ((rc = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
                        output->append(buffer, rc);
                    }
                }
                ssh_channel_send_eof(channel);
                ssh_channel_close(channel);
                ssh_channel_free(channel);
            }
        }
        return rc;
    }

    int SSH::scp(const std::string& filePath, const byte* data, size_t size) {
        ssh_scp scp;
        int rc;

        size_t found = filePath.find_last_of("/\\");
        std::string filename = filePath.substr(found+1);
        std::string path = filePath.substr(0, found+1);

        scp = ssh_scp_new(session_, SSH_SCP_WRITE, path.c_str());
        if (scp == NULL) {
            throw Error(1, ssh_get_error(session_));
            rc = SSH_ERROR;
        } else {
            rc = ssh_scp_init(scp);
            if (rc != SSH_OK) {
                throw Error(1, ssh_get_error(session_));
            } else {
                rc = ssh_scp_push_file (scp, filename.c_str(), size, S_IRUSR |  S_IWUSR);
                if (rc != SSH_OK) {
                    throw Error(1, ssh_get_error(session_));
                } else {
                    rc = ssh_scp_write(scp, data, size);
                    if (rc != SSH_OK) {
                        throw Error(1, ssh_get_error(session_));
                    }
                }
                ssh_scp_close(scp);
            }
            ssh_scp_free(scp);
        }

        return rc;
    }

    void SSH::openSftp() {
        if (sftp_) return;

        sftp_ = sftp_new(session_);
        if (sftp_ == NULL) {
            throw Error(1, "Unable to create the the sftp session");
        }
        if (sftp_init(sftp_) != SSH_OK) {
            sftp_free(sftp_);
            throw Error(1, "Error initializing SFTP session");
        }
    }

    void SSH::getFileSftp(const std::string& filePath, sftp_file& handle) {
        if (!sftp_) openSftp();
        handle = sftp_open(sftp_, ("/"+filePath).c_str(), 0x0000, 0); // read only
    }

    SSH::~SSH() {
        if (sftp_) sftp_free(sftp_);
        ssh_disconnect(session_);
        ssh_free(session_);
    }
}
#endif
