#pragma once

#include <iostream>

class SessionClient {
    private:
        std::string uuid;
    public :
        SessionClient(std::string &uuid);
        SessionClient();
        ~SessionClient();
        std::string     getUUID();
        void            setUUID(const std::string &uuid);
};