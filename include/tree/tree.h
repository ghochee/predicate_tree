#include <cstddef>
#include <functional>
#include <memory>

enum class traversal_order { pre, in, post };

// TODO(ghochee): This can be a partial specialization of
// tree<T, TallComparator, LeftComparator> where
// TallComparator = LeftComparator = std::function<bool(const T &, const T &)>
template <typename T>
class tree {
  private:
    // Strict weak ordering is needed for the predicates. This is a totally
    // strictly weak ordering :).
    static bool indifferent(const T &, const T &) { return false; }

  public:
    // TODO(ghochee): This should be private? but testable?
    class Node;

    typedef T value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    class iterator;

    tree(std::function<bool(const T &, const T &)> isTall = indifferent,
         std::function<bool(const T &, const T &)> isLeft = indifferent);

    iterator begin() const;
    iterator end() const;

    // Insert 'value' into the tree. 'isTall' and 'isLeft' predicates are used
    // to determine correct position to insert at.
    //
    // 'pos' is a hint for insertion location. 'isTall' and 'isLeft' is used to
    //     determine if incorrect in which it is ignored.
    iterator insert(iterator pos, T &&value);
    // Same as above but with pos = end_.
    iterator insert(T &&value);
    // Same as above but with const-lvalue-ref 'value'.
    iterator insert(iterator pos, const T &value);
    iterator insert(const T &value);

    // Erase nodes from the tree which are in the range [first, last).
    //
    // 'first' and 'last' are iterators.
    // - If last is not reachable from first then all elements from first
    //   through end() are erased.
    //
    // NOTE: For pre-order iterator the semantics of iterator increment will
    // interfere with the erasure of elements in the tree, including deleting
    // elements outside the range. The only situations in which this is defined
    // are:
    // - [first, last) is empty.
    // - [first, last) is a single element.
    iterator erase(iterator first, iterator last);

  private:
    static iterator end_;

    // Insert helper which inserts a new node containing 'value' under 'pos'
    // **and** updates 'pos' to the result of the Node::merge call.
    iterator insert(T &&value, std::unique_ptr<Node> &pos);

    // Erase helper which erases the node and merges the two children of ths
    // node. The 'pos' reference is updated with the merged tree.
    void erase(std::unique_ptr<Node> &pos);

    const std::function<bool(const T &, const T &)> isTall_;
    const std::function<bool(const T &, const T &)> isLeft_;

    std::unique_ptr<Node> root_;
};

#include "iterator.h"
#include "node.h"

#include "tree.hh"
