#pragma once
#include "Util.h"

namespace ds {
using i32 = int;
using u32 = unsigned;
struct WBT {
  constexpr static u32 N = 5e5 + 10;
  constexpr static u32 D = 3, G = 2;

  struct Node {
    i32 key;
    u32 sz, l, r;
  } pool[N * 32];
  u32 top = 0, root[N];

  u32 alloc(i32 key, u32 l, u32 r) {
    u32 ret = ++top;
    pool[ret].key = key;
    pool[ret].sz = pool[l].sz + pool[r].sz + 1;
    pool[ret].l = l;
    pool[ret].r = r;
    return ret;
  }

  bool is_balanced(u32 x, u32 y) {
    return D * (pool[x].sz + 1) >= (pool[y].sz + 1);
  }

  bool is_single(u32 x, u32 y) {
    return (pool[x].sz + 1) < G * (pool[y].sz + 1);
  }

  u32 single_l(i32 key, u32 l, u32 r) {
    return alloc(pool[r].key, alloc(key, l, pool[r].l), pool[r].r);
  }

  u32 double_l(i32 key, u32 l, u32 r) {
    return alloc(pool[pool[r].l].key, alloc(key, l, pool[pool[r].l].l), alloc(pool[r].key, pool[pool[r].l].r, pool[r].r));
  }

  u32 single_r(i32 key, u32 l, u32 r) {
    return alloc(pool[l].key, pool[l].l, alloc(key, pool[l].r, r));
  }

  u32 double_r(i32 key, u32 l, u32 r) {
    return alloc(pool[pool[l].r].key, alloc(pool[l].key, pool[l].l, pool[pool[l].r].l), alloc(key, pool[pool[l].r].r, r));
  }

  u32 balance_l(i32 key, u32 l, u32 r) {
    if (is_balanced(l, r)) { return alloc(key, l, r); }
    return is_single(pool[r].l, pool[r].r) ? single_l(key, l, r) : double_l(key, l, r);
  }

  u32 balance_r(i32 key, u32 l, u32 r) {
    if (is_balanced(r, l)) { return alloc(key, l, r); }
    return is_single(pool[l].r, pool[l].l) ? single_r(key, l, r) : double_r(key, l, r);
  }

  u32 insert(u32 x, i32 key) {
    if (x == 0) {
      u32 ret = ++top;
      pool[ret].key = key;
      pool[ret].sz = 1;
      return ret;
    }
    if (key < pool[x].key) { return balance_r(pool[x].key, insert(pool[x].l, key), pool[x].r); }
    return balance_l(pool[x].key, pool[x].l, insert(pool[x].r, key));
  }

  // return (root after del max, max value)
  std::pair<u32, i32> pop_max(u32 x) {
    if (pool[x].r == 0) { return {pool[x].l, pool[x].key}; }
    auto pr = pop_max(pool[x].r);
    return {balance_r(pool[x].key, pool[x].l, pr.first), pr.second};
  }

  std::pair<u32, i32> pop_min(u32 x) {
    if (pool[x].l == 0) { return {pool[x].r, pool[x].key}; }
    auto pr = pop_min(pool[x].l);
    return {balance_l(pool[x].key, pr.first, pool[x].r), pr.second};
  }

  u32 remove(u32 x, i32 key) {
    if (x == 0) { return 0; }
    if (key < pool[x].key) { return balance_l(pool[x].key, remove(pool[x].l, key), pool[x].r); }
    if (pool[x].key < key) { return balance_r(pool[x].key, pool[x].l, remove(pool[x].r, key)); }
    u32 l = pool[x].l, r = pool[x].r;
    if (l == 0 || r == 0) { return l + r; }
    if (pool[l].sz > pool[r].sz) {
      auto pr = pop_max(l);
      return balance_l(pr.second, pr.first, r);
    } else {
      auto pr = pop_min(r);
      return balance_r(pr.second, l, pr.first);
    }
  }

  i32 nxt(u32 x, i32 key) {
    u32 p = 0;
    while (x) {
      if (key < pool[x].key) {
        p = x, x = pool[x].l;
      } else {
        x = pool[x].r;
      }
    }
    return p ? pool[p].key : 2147483647;
  }

  i32 pre(u32 x, i32 key) {
    u32 p = 0;
    while (x) {
      if (pool[x].key < key) {
        p = x, x = pool[x].r;
      } else {
        x = pool[x].l;
      }
    }
    return p ? pool[p].key : -2147483647;
  }

  u32 lt(u32 x, i32 key) {
    u32 ret = 0;
    while (x) {
      if (pool[x].key < key) {
        ret += 1 + pool[pool[x].l].sz, x = pool[x].r;
      } else {
        x = pool[x].l;
      }
    }
    return ret;
  }

  i32 kth(u32 x, u32 k) {
    while (true) {
      if (k <= pool[pool[x].l].sz) {
        x = pool[x].l;
      } else if (k == pool[pool[x].l].sz + 1) {
        return pool[x].key;
      } else {
        k -= pool[pool[x].l].sz + 1, x = pool[x].r;
      }
    }
  }
};
} // namespace ds