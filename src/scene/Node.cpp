#include "scene/Node.h"

#include <utility>

#include "core/Logging.h"

Node::Node(std::shared_ptr<Node> parent, std::string name)
    : _parent(parent), _name(std::move(name)) {
    if (parent != nullptr) {
        parent->_child.push_back(shared_from_this());
    }
}

Node::~Node() {
    _child.clear();
    auto parent = _parent.lock();
    if (parent) {
        parent->removeChild(shared_from_this());
    }
}

std::shared_ptr<Node> Node::getParent() const {
    return _parent.lock();
}

std::vector<std::shared_ptr<Node>> Node::getChildren() const {
    return _child;
}

std::shared_ptr<Node> Node::addChild() {
    Node child(*this);
    auto child_ptr = std::make_shared<Node>(child);
    _child.push_back(child_ptr);
    return child_ptr;
}

void Node::removeChild(std::shared_ptr<Node> child) {
    auto child_iter = _child.begin();
    auto del_child_iter = _child.end();
    while (child_iter != _child.end()) {
        if (*child_iter == child) {
            del_child_iter = child_iter;
            break;
        }
        ++child_iter;
    }
    if (del_child_iter == _child.end()) {
        LOGW("Not possible to delete a non-existent child");
    } else {
        _child.erase(del_child_iter);
    }
}

std::string Node::getName() const {
    return _name;
}

void Node::changeName(const std::string& new_name) {
    _name = new_name;
}