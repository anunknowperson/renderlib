#pragma once

#include <string>
#include <vector>
#include <memory>

class Node: public std::enable_shared_from_this<Node>
{
public:
	explicit Node(std::shared_ptr<Node> parent = nullptr, std::string name = "without name");

	~Node();

	std::shared_ptr<Node> get_parent() const;
	std::vector<std::shared_ptr<Node>> get_children() const;
	std::shared_ptr<Node> add_child();
	void remove_child(std::shared_ptr<Node> child);
	std::string get_name() const;
	void change_name(std::string new_name);

private:
	const std::weak_ptr<Node> _parent;
	std::vector<std::shared_ptr<Node>> _child;
	std::string _name;
};
