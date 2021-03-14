#include <vector>
#include <iostream>
#include <unordered_map>
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
struct HashMap {
    struct optional {
        char type;
        std::pair<KeyType, ValueType> val;
        optional() {
            type = 0;
        }
    };
    struct iterator {
        typename std::vector<optional>::iterator it, border;
        iterator operator++() {
            ++it;
            while (it != border && it->type != 1)
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
            void* a = &((*it).val);
            std::pair<const KeyType, ValueType> *b = (std::pair<const KeyType, ValueType>*)a;
            return *b;
        }
        std::pair<const KeyType, ValueType>* operator->() {
            void* a = &((*it).val);
            std::pair<const KeyType, ValueType> *b = (std::pair<const KeyType, ValueType>*)a;
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
        const std::pair<KeyType, ValueType>* operator->() { return &it->val; }
        typename std::vector<optional>::const_iterator it, border;
        const_iterator operator++() {
            ++it;
            while (it != border && it->type != 1)
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
        buff = std::vector<optional>(0);
    }
    Hash hasher;
    std::vector<optional> buff;
    int curr_size, bad_size;
    size_t next(size_t pos) const {
        if (pos + 1 == buff.size())
            return 0;
        else
            return pos + 1;
    }
    ValueType& add(const std::pair<KeyType, ValueType> &x) {
        size_t pos = hasher(x.first) % buff.size();
        while (buff[pos].type == 1 && !((const KeyType)buff[pos].val.first == x.first))
            pos = next(pos);
        if (buff[pos].type != 1) {
            buff[pos].val = x;
            buff[pos].type = 1;
            ++curr_size;
            if (buff[pos].type == 2)
                --bad_size;
        }
        return buff[pos].val.second;
    }
    const size_t Inf = 1e9;
    void erase(const KeyType &x) {
        if (!buff.size())
            return;
        size_t pos = hasher(x) % buff.size();
        while (buff[pos].type != 0 && !(buff[pos].type == 1 && buff[pos].val.first == x))
            pos = next(pos);
        if (buff[pos].type == 0)
            return;
        --curr_size;
        buff[pos].type = 2;
        ++bad_size;
    }
    ValueType& operator[](const KeyType &key) {
        insert({key, ValueType()});
        return find(key).it->val.second;
    }
    iterator find(const KeyType &key) {
        if (buff.size() == 0)
            return {buff.end(), buff.end()};
        size_t pos = hasher(key) % buff.size();
        while (buff[pos].type != 0 && !(buff[pos].val.first == key && buff[pos].type == 1))
            pos = next(pos);
        if (buff[pos].type == 0)
            return iterator{buff.end(), buff.end()};
        return iterator{buff.begin() + pos, buff.end()};
    }
    const_iterator find(const KeyType &key) const {
        if (buff.size() == 0)
            return {buff.end(), buff.end()};
        size_t pos = hasher(key) % buff.size();
        while (buff[pos].type != 0 && !(buff[pos].val.first == key && buff[pos].type == 1))
            pos = next(pos);
        if (buff[pos].type == 0)
            return const_iterator{buff.end(), buff.end()};
        return const_iterator{buff.begin() + pos, buff.end()};
    }
    void insert(const std::pair<KeyType, ValueType> &x) {
        if ((curr_size + 1 + bad_size) * 2 > buff.size()){
            std::vector<optional> pbuff = buff;
            buff = std::vector<optional>(4 * (curr_size + 1));
            curr_size = 0;
            bad_size = 0;
            for (auto &x : pbuff)
                if (x.type == 1)
                    add(x.val);
        }
        if (find(x.first) == end())
            add(x);
    }
    HashMap() {bad_size = curr_size = 0;}
    template<class T>
    HashMap(T _hasher) : hasher(_hasher) { bad_size = curr_size = 0; }
    //template<class T>
    //HashMap(const HashMap<KeyType, ValueType, T> &map) { *this = map; }
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
    HashMap(const std::initializer_list<std::pair<KeyType, ValueType>> &lst, Hash _hasher = Hash()) {
        curr_size = 0;
        bad_size = 0;
        hasher = _hasher;
        for (auto &x : lst)
            insert(x);
    }
    int size() const {
        return curr_size;
    }
    bool empty() const {
        return size() == 0;
    }
    Hash hash_function() const {
        return hasher;
    }
    iterator begin() {
        int pos = 0;
        while (pos != buff.size() && buff[pos].type != 1)
            ++pos;
        return iterator{buff.begin() + pos, buff.end()};
    }
    iterator end() {
        return iterator{buff.end(), buff.end()};
    }
    const_iterator begin() const {
        int pos = 0;
        while (pos != buff.size() && buff[pos].type != 1)
            ++pos;
        return const_iterator{buff.cbegin() + pos, buff.cend()};
    }
    const_iterator end() const {
        return const_iterator{buff.cend(), buff.cend()};
    }
};
int main() {
#define int long long
    using namespace std;
    ios_base::sync_with_stdio(false);
    int q;
    cin >> q;
    HashMap<int, int> h;
    for (int i = 0; i < q; ++i) {
        char t;
        cin >> t;
        //cout << t;
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
