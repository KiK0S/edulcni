#include <vector>
#include <utility>
#include <cassert>
#include <map>

namespace edulcni {


template<typename T>
struct Stack {
    virtual void push(T elem) = 0;
    virtual void pop() = 0;
    virtual int max_height()  = 0;
    virtual int max_width()  = 0;
    virtual std::vector<std::pair<int, T>> get_snapshot() = 0;
    virtual void clear() = 0;
};

template<typename T>
struct BasicStack : public Stack<T> {
    std::vector<T> vals;
    virtual void push(T elem) {
        vals.push_back(elem);
    };
    virtual void pop() {
        assert(vals.size() > 0);
        vals.pop_back();
    }
    virtual std::vector<std::pair<int, T>> get_snapshot() {
        std::vector<std::pair<int, T>> res;
        for (int i = 0; i < vals.size(); i++) {
            res.emplace_back(i, vals[i]);
        }
        return res;
    }
    virtual int max_height() {
        return (int)vals.size()-1;
    }
    virtual int max_width() {
        return 1;
    }
    virtual void clear() {
        vals.clear();
    }
};

template<typename T>
struct PersistentStack : public Stack<T> {
    std::vector<std::pair<int, T>> vals;
    int current_depth = 0;

    virtual void push(T elem) { 
        vals.emplace_back(current_depth, elem);
        current_depth++;
    };
    virtual void pop() {
        assert(current_depth > 0);
        current_depth--;
    }
    virtual std::vector<std::pair<int, T>> get_snapshot() {
        return vals;
    }
    virtual int max_height() {
        if (vals.size() == 0) return -1;
        return max_element(vals.begin(), vals.end())->first;
    }
    virtual int max_width() {
        std::map<int, int> cnt;
        int res = 0;
        for (auto [depth, label] : vals) {
            cnt[depth]++;
            res = std::max(res, cnt[depth]);
        }
        return res;
    }
    void clear() {
        vals.clear();
        current_depth = 0;
    }
};
}