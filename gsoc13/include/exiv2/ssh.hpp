#include "exv_conf.h"
#if EXV_USE_SSH == 1
#include <libssh/libssh.h>
#include <string>
#include "error.hpp"

namespace Exiv2 {
    class EXIV2API SSH {
    public:
        SSH (const std::string& host, const std::string& user, const std::string& pass);
        ~SSH();
        int RunCommand(const std::string& cmd, std::string* output);
    private:
        std::string host_;
        std::string user_;
        std::string pass_;
        ssh_session session_;
    };
}
#endif

