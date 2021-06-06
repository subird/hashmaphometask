#include <vector>
#include <iostream>
#include <unordered_map>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
struct HashMap {
    static const char EMPTY_ELEMENT = 1;
    static const char NORMAL_ELEMENT = 1;
    static const char BAD_ELEMENT = 2;
    struct optionalAdvanced;
    struct const_iterator;
    struct iterator;
    Hash hasher;
    std::vector<optionalAdvanced> buffer;
    int curr_size, bad_size;
    iterator cachedBegin;

    struct optionalAdvanced {
        char type;
        std::pair<KeyType, ValueType> val;
        optionalAdvanced() {
            type = EMPTY_ELEMENT;
        }
    };
    struct iterator {
        typename std::vector<optionalAdvanced>::iterator it, border;
        iterator operator++() {
            ++it;
            while (it != border && it->type != NORMAL_ELEMENT)
                ++it;
            return *this;
        }
        iterator operator++(int) {
            iterator old = *this;
            operator++();
            return old;
        }
        bool operator==(const iterator &a) const {
            return a.it == it;
        }
        bool operator!=(const iterator &a) const {
            return a.it != it;
        }
        std::pair<const KeyType, ValueType>& operator*(void) {
            void* a = &(it->val);
            std::pair<const KeyType, ValueType> *b = reinterpret_cast<std::pair<const KeyType, ValueType>*>(a);
            return *b;
        }
        std::pair<const KeyType, ValueType>* operator->() {
            void* a = &((*it).val);
            std::pair<const KeyType, ValueType> *b = reinterpret_cast<std::pair<const KeyType, ValueType>*>(a);
            return b;
        }
    };
    struct const_iterator {
        bool operator==(const const_iterator &a) const {
            return a.it == it;
        }
        bool operator!=(const const_iterator &a) const {
            return a.it != it;
        }
        const std::pair<KeyType, ValueType>* operator->() {
            return &it->val;
        }
        typename std::vector<optionalAdvanced>::const_iterator it, border;
        const_iterator operator++() {
            ++it;
            while (it != border && it->type != NORMAL_ELEMENT)
                ++it;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator old = *this;
            operator++();
            return old;
        }
        const std::pair<KeyType, ValueType>& operator*(void) {
            return it->val;
        }
    };
    void clear() {
        curr_size = 0;
        bad_size = 0;
        buffer = std::vector<optionalAdvanced>(0);
    }
    size_t next(size_t pos) const {
        if (pos + 1 == buffer.size())
            return 0;
        else
            return pos + 1;
    }
    ValueType& add(const std::pair<KeyType, ValueType> &x) {
        size_t pos = hasher(x.first) % buffer.size();
        while (buffer[pos].type == NORMAL_ELEMENT && !((const KeyType)buffer[pos].val.first == x.first))
            pos = next(pos);
        if (buffer[pos].type != NORMAL_ELEMENT) {
            buffer[pos].val = x;
            buffer[pos].type = NORMAL_ELEMENT;
            ++curr_size;
            if (buffer[pos].type == BAD_ELEMENT)
                --bad_size;
        }
        return buffer[pos].val.second;
    }
    void erase(const KeyType &x) {
        if (!buffer.size())
            return;
        size_t pos = hasher(x) % buffer.size();
        while (buffer[pos].type != EMPTY_ELEMENT && !(buffer[pos].type == NORMAL_ELEMENT && buffer[pos].val.first == x))
            pos = next(pos);
        if (buffer[pos].type == EMPTY_ELEMENT)
            return;
        --curr_size;
        buffer[pos].type = BAD_ELEMENT;
        ++bad_size;
        recalcBegin();
    }
    ValueType& operator[](const KeyType &key) {
        insert({key, ValueType()});
        return find(key).it->val.second;
    }
    iterator findFast(const KeyType &key) {
        if (buffer.size() == 0)
            return {buffer.end(), buffer.end()};
        size_t pos = hasher(key) % buffer.size();
        while (buffer[pos].type != EMPTY_ELEMENT && !(buffer[pos].val.first == key && buffer[pos].type == NORMAL_ELEMENT))
            pos = next(pos);
        if (buffer[pos].type == EMPTY_ELEMENT)
            return iterator{buffer.end(), buffer.end()};
        return iterator{buffer.begin() + pos, buffer.end()};
    }
    iterator find(const KeyType &key) {
        return findFast(key);
    }
    const_iterator find(const KeyType &key) const {
        return findFast(key);
    }
    void rehash() {
        if ((curr_size + 1 + bad_size) * 2 > buffer.size()){
            std::vector<optionalAdvanced> pbuffer = buffer;
            buffer = std::vector<optionalAdvanced>(4 * (curr_size + 1));
            curr_size = 0;
            bad_size = 0;
            for (auto &x : pbuffer)
                if (x.type == 1)
                    add(x.val);
        }
    }
    void insert(const std::pair<KeyType, ValueType> &x) {
        rehash();
        if (find(x.first) == end())
            add(x);
        recalcBegin();
    }
    HashMap() {bad_size = curr_size = 0;}
    template<class T>
    HashMap(T _hasher) : hasher(_hasher) { bad_size = curr_size = 0; }
    template<class T>
    HashMap(T begin, T end, Hash _hasher = Hash()) {
        bad_size = curr_size = 0;
        hasher = _hasher;
        while (begin != end) {
            insert(*begin);
            ++begin;
        }
    }
    const ValueType& at(const KeyType &key) const {
        if (find(key) == end())
            throw std::out_of_range("no such key");
        return find(key).it->val.second;;
    }
    HashMap(const std::initializer_list<std::pair<KeyType, ValueType>> &lst, Hash _hasher = Hash()) : HashMap(lst.begin(), lst.end(), _hasher) {}
    int size() const {
        return curr_size;
    }
    bool empty() const {
        return size() == 0;
    }
    Hash hash_function() const {
        return hasher;
    }
    void recalcBegin() {
        int pos = 0;
        while (pos != buffer.size() && buffer[pos].type != 1)
            ++pos;
        cachedBegin = iterator{buffer.begin() + pos, buffer.end()};
    }
    iterator end() {
        return iterator{buffer.end(), buffer.end()};
    }
    iterator begin() {
        return cachedBegin;
    }
    const_iterator begin() const {
        return cachedBegin;
    }
    const_iterator end() const {
        return const_iterator{buffer.cend(), buffer.cend()};
    }
};
int main() {
    using namespace std;
    ios_base::sync_with_stdio(false);
    int q;
    cin >> q;
    HashMap<int, int> h;
    for (int i = 0; i < q; ++i) {
        char t;
        cin >> t;
        if (t == '+') {
            int k, v;
            cin >> k >> v;
            h[k] = v;
        } else if (t == '-') {
            int k;
            cin >> k;
            h.erase(k);
        } else if (t == '?') {
            int k;
            cin >> k;
            if (h.find(k) != h.end())
                cout << h.find(k)->second << "\n";
            else
                cout << -1 << "\n";
        } else if (t == '!') {
            h.clear();
        } else {
            for (auto x : h)
                cout << x.first << " " << x.second << "\n";
        }
    }
    cout << h.size();
    return 0;
}