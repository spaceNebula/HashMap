#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <exception>
#include <utility>
#include <algorithm>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
private:
    std::list<std::pair<KeyType, ValueType>> _elements{};
    std::vector<std::list<typename std::list<std::pair<KeyType, ValueType>>::iterator>> _table;
    Hash _hash_function;
    size_t _size = 0;

public:
    using iterator = typename std::list<std::pair<KeyType, ValueType>>::iterator;
    using const_iterator = typename std::list<std::pair<KeyType, ValueType>>::const_iterator;

    explicit HashMap(const Hash &hasher = Hash()) : _table(1e6 + 3), _hash_function(hasher) {}

    template<typename Iterator>
    HashMap(Iterator begin, Iterator end, const Hash &hash = Hash()) : _table(1e6 + 3) {
        for (auto it = begin; it != end; ++it) {
            operator[](it->first) = it->second;
        }
    }

    HashMap(const std::initializer_list<std::pair<KeyType, ValueType>> &list,
            const Hash &hash = Hash())
            : _hash_function(hash), _table(1e6 + 3) {
        for (const auto &it : list) {
            operator[](it.first) = it.second;
        }
    }

    size_t hash(KeyType key) const {
        return _hash_function(key) % _table.size();
    }

    void insert(std::pair<KeyType, ValueType> _pair) {
        auto id = hash(_pair.first);
        for (auto it : _table[id]) {
            if (it->first == _pair.first) {
                return;
            }
        }
        operator[](_pair.first) = _pair.second;
    }

    void erase(const KeyType &key) {
        auto id = hash(key);
        for (auto it = _table[id].begin(); it != _table[id].end(); ++it) {
            if ((*it)->first == key) {
                _elements.erase(*it);
                _table[id].erase(it);
                --_size;
                break;
            }
        }
    }

    ValueType &operator[](KeyType key) {
        auto id = hash(key);
        for (auto it : _table[id]) {
            if (it->first == key) {
                return it->second;
            }
        }

        ++_size;
        _elements.emplace_front(key, ValueType());
        _table[id].push_back(_elements.begin());
        return _elements.begin()->second;
    }

    iterator begin() {
        return _elements.begin();
    }

    iterator end() {
        return _elements.end();
    }

    const_iterator begin() const {
        return _elements.cbegin();
    }

    const_iterator end() const {
        return _elements.cend();
    }

    iterator find(KeyType key) {
        auto id = hash(key);
        for (auto it = _table[id].begin(); it != _table[id].end(); ++it) {
            if ((*it)->first == key) {
                return *it;
            }
        }
        return (*this).end();
    }

    const_iterator find(KeyType key) const {
        auto id = hash(key);
        for (auto it = _table[id].begin(); it != _table[id].end(); ++it) {
            if ((*it)->first == key) {
                return *it;
            }
        }
        return (*this).end();
    }

    Hash hash_function() const {
        return _hash_function;
    }

    size_t size() const {
        return _size;
    }

    const ValueType &at(KeyType key) const {
        auto id = hash(key);
        for (auto it : _table[id]) {
            if (it->first == key) {
                return it->second;
            }
        }

        throw std::out_of_range("Out of range.");
    }

    void clear() {
        for (auto it : _elements) {
            auto id = hash(it.first);
            if (_table[id].size()) {
                _table[id].clear();
            }
        }
        _elements.clear();
        _size = 0;
    }

    bool empty() const {
        return _size == 0;
    }
};
