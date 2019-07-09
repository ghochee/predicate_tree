template <side e>
constexpr auto other() noexcept {
    if constexpr (e == side::left) {
        return side::right;
    } else {
        return side::left;
    }
}

template <typename T>
template <traversal_order order, side wing>
raw_tree<T>::template iterator<order, wing>::iterator(raw_tree<T> &root) {
    if constexpr (order == traversal_order::pre) { node_ = &root; }

    if constexpr (order == traversal_order::in) {
        for (node_ = &root; node_->child_ref<wing>();
             node_ = node_->child_ref<wing>().get()) {}
    }
}

template <typename T>
template <traversal_order order, side wing>
template <traversal_order other_order, side other_wing>
raw_tree<T>::template iterator<order, wing>::iterator(
    const iterator<other_order, other_wing> &other)
    : node_(other.node_) {}

template <typename T>
template <traversal_order order, side wing>
bool raw_tree<T>::template iterator<order, wing>::operator==(
    const raw_tree<T>::iterator<order, wing> &other) const {
    return node_ == other.node_;
}

template <typename T>
template <traversal_order order, side wing>
bool raw_tree<T>::template iterator<order, wing>::operator!=(
    const raw_tree<T>::iterator<order, wing> &other) const {
    return node_ != other.node_;
}

template <typename T>
template <traversal_order order, side wing>
T &raw_tree<T>::template iterator<order, wing>::operator*() const {
    return **node_;
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing>
    &raw_tree<T>::template iterator<order, wing>::operator++() {
    if constexpr (order == traversal_order::pre) { preorder_increment(); }
    if constexpr (order == traversal_order::in) { inorder_increment(); }
    return *this;
}

template <typename T>
template <traversal_order order, side wing>
typename raw_tree<T>::template iterator<order, wing>
    raw_tree<T>::template iterator<order, wing>::operator++(int) {
    auto return_value = *this;
    ++(*this);
    return return_value;
}

template <typename T>
template <traversal_order order, side wing>
void raw_tree<T>::template iterator<order, wing>::preorder_increment() {
    if (node_ == nullptr) {
        // TODO(ghochee): Set to root.
        return;
    }

    if (node_->child_ref<wing>()) {
        node_ = node_->child_ref<wing>().get();
        return;
    }

    for (const raw_tree<T> *child = nullptr; node_;
         child = std::exchange(node_, node_->parent_)) {
        auto other_child = node_->child_ref<other<wing>()>().get();
        if (other_child && other_child != child) {
            node_ = other_child;
            break;
        }
    }
}

template <typename T>
template <traversal_order order, side wing>
void raw_tree<T>::template iterator<order, wing>::inorder_increment() {
    if (node_ == nullptr) {
        // TODO(ghochee): Set root and choose leftmost descendant.
        return;
    }

    if (node_->child_ref<other<wing>()>()) {
        for (node_ = node_->child_ref<other<wing>()>().get();
             node_->child_ref<wing>(); node_ = node_->child_ref<wing>().get()) {
        }
        return;
    }

    for (const raw_tree<T> *child = nullptr; node_;
         child = std::exchange(node_, node_->parent_)) {
        auto other_child = node_->child_ref<other<wing>()>().get();
        if (other_child != child) { return; }
    }
}
