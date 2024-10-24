#include <iostream>
#include <fstream>
#include <string>

class logstream {
private:
    static std::ofstream* log_file;
    std::ostream* stream;

public:
    static void set_log_file(const std::string& name) {
        if (log_file != nullptr) {
            log_file->close();
            delete log_file;
        }
        log_file = new std::ofstream(name, std::ios::app);
    }

    logstream() {
        if (log_file == nullptr) {
            log_file = new std::ofstream("default.log", std::ios::app);
        }
        stream = log_file;
    }

    ~logstream() {
        if (stream == log_file) {
            *stream << std::flush;
        }
    }

    template <typename T>
    inline logstream& operator<<(const T& rhs) {
        *stream << rhs;
        return *this;
    }

    logstream& operator<<(std::ostream& (*manip)(std::ostream&)) {
        (*manip)(*stream);
        return *this;
    }
};

std::ofstream* logstream::log_file = nullptr;

int f(int a) {
    logstream log;
    log << "f(0x" << std::hex << a << ")" << std::endl;
    int r = a * a;
    log << "f() -- return " << r << std::endl;
    return r;
}

int main() {
    logstream::set_log_file("test.log");
    logstream log;
    log << "main()" << std::endl;
    f(15);
    log << "main() -- return" << std::endl;
    return 0;
}
