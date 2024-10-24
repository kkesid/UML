#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

class BaseCache {
public:
    virtual bool has(const std::string& key) = 0;
    virtual std::string read(const std::string& key) = 0;
    virtual void write(const std::string& key, const std::string& value) = 0;
    virtual void deleteB(const std::string& key) = 0;
};

class MemoryCache : public BaseCache {
private:
    std::unordered_map<std::string, std::string> data;

public:
    bool has(const std::string& key) override {
        return data.find(key) != data.end();
    }

    std::string read(const std::string& key) override {
        if (has(key)) {
            return data[key];
        }
        return "";
    }

    void write(const std::string& key, const std::string& value) override {
        data[key] = value;
    }

    void deleteB(const std::string& key) override {
        data.erase(key);
    }
};

class FileCache : public BaseCache {
private:
    std::string cache_dir; 

    // Хэш-функция для генерации имени файла
    std::string hash_key(const std::string& key) {
        unsigned int hash = 0;
        for (char c : key) {
            hash = (hash * 31) + c;
        }
        return std::to_string(hash);
    }

public:
    FileCache(const std::string& dir) : cache_dir(dir) {
        // Создание каталога, если он не существует
    }

    bool has(const std::string& key) override {
        std::string filename = cache_dir + "/" + hash_key(key);
        std::ifstream file(filename);
        return file.good();
    }

    std::string read(const std::string& key) override {
        std::string filename = cache_dir + "/" + hash_key(key);
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string read_key, value;
            std::getline(ss, read_key, '\t');
            std::getline(ss, value, '\t');
            if (read_key == key) {
                return value;
            }
        }
        return "";
    }

    void write(const std::string& key, const std::string& value) override {
        std::string filename = cache_dir + "/" + hash_key(key);
        std::ofstream file(filename);
        file << key << "\t" << value << std::endl;
    }

    void deleteB(const std::string& key) override {
        std::string filename = cache_dir + "/" + hash_key(key);
        std::remove(filename.c_str());
    }
};

class NullCache : public BaseCache {
public:
    bool has(const std::string& key) override {
        return false; // Никогда не содержит ключей
    }

    std::string read(const std::string& key) override {
        return ""; // Всегда возвращает пустую строку
    }

    void write(const std::string& key, const std::string& value) override {
        // Ничего не делает, запись происходит мгновенно
    }

    void deleteB(const std::string& key) override {
        // Ничего не делает, удаление не происходит
    }
};

class PoorManMemoryCache : public BaseCache {
private:
    int max_size; 
    std::unordered_map<std::string, std::string> data;
    std::list<std::string> keys; 

public:
    PoorManMemoryCache(int size) : max_size(size) { }

    bool has(const std::string& key) override {
        return data.find(key) != data.end();
    }

    std::string read(const std::string& key) override {
        if (has(key)) {
            return data[key];
        }
        return "";
    }

    void write(const std::string& key, const std::string& value) override {
        if (has(key)) {
            data[key] = value;
        }
        else {
            data[key] = value;
            keys.push_front(key);
            if (keys.size() > max_size) {
                std::string oldest_key = keys.back();
                keys.pop_back();
                data.erase(oldest_key);
            }
        }
    }

    void deleteB(const std::string& key) override {
        data.erase(key);
        keys.remove(key);
    }
};

class CacheApplier {
private:
    BaseCache* strategy;

public:
    void set_strategy(BaseCache& strategy) {
        this->strategy = &strategy;
    }

    std::string read_from_cache(const std::string& key) {
        return strategy->read(key);
    }

    void write_from_cache(const std::string& key, const std::string& value) {
        strategy->write(key, value);
    }

    void delete_from_cache(const std::string& key) {
        strategy->deleteB(key);
    }
};

int main() {

    MemoryCache memory_cache;
    FileCache file_cache("cache_data");
    PoorManMemoryCache poor_man_cache(5);

    CacheApplier applier;

    applier.set_strategy(memory_cache);

    applier.write_from_cache("user1", "John Doe");
    applier.write_from_cache("user2", "Jane Smith");

    std::cout << "User1: " << applier.read_from_cache("user1") << std::endl;
    std::cout << "User2: " << applier.read_from_cache("user2") << std::endl;

    applier.delete_from_cache("user1");

    std::cout << "User1: " << applier.read_from_cache("user1") << std::endl;

    return 0;
}