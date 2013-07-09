#include "exv_conf.h"
#if EXV_USE_SSH == 1
#include <libssh/libssh.h>
#include <sys/stat.h>
#include <string>
#include "error.hpp"
#include "types.hpp"

namespace Exiv2 {
    class EXIV2API SSH {
    public:
        SSH (const std::string& host, const std::string& user, const std::string& pass);
        ~SSH();
        int runCommand(const std::string& cmd, std::string* output);
        int scp(const std::string& filePath, const byte* data, size_t size);
    private:
        std::string host_;
        std::string user_;
        std::string pass_;
        ssh_session session_;
    };
}
#endif

