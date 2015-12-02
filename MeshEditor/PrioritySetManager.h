#pragma once
#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"

#include <set>
#include <map>
#include <deque>

template <typename NodeType, typename NodeTypeComp, typename Identifier>
class PrioritySetManager {
	typedef typename std::multiset<NodeType*, NodeTypeComp> priority_set;
	typedef typename std::multiset<NodeType*, NodeTypeComp>::iterator priority_set_iterator;
public:
	PrioritySetManager (){}
	~PrioritySetManager(){
		foreach (NodeType *node, core_priority_queue)
			delete node;
	}
public:
	bool empty () {
		return core_priority_queue.empty ();
	}
	priority_set_iterator begin (){
		return core_priority_queue.begin ();
	}
	priority_set_iterator end (){
		return core_priority_queue.end ();
	}
	priority_set_iterator insert (NodeType *new_node, Identifier &identifier){
		auto locate = core_priority_queue.insert (new_node);
		identifier_set_mapping.insert (std::make_pair (identifier, locate));
		return locate;
	}
	bool exists (Identifier &identifier){
		auto locate = identifier_set_mapping.find (identifier);
		return locate != identifier_set_mapping.end ();
	}
	priority_set_iterator find (Identifier &identifier){
		auto locate = identifier_set_mapping.find (identifier);
		if (locate == identifier_set_mapping.end ())
			return core_priority_queue.end ();
		else
			return locate->second;
	}
	void update (NodeType *node, Identifier &identifier){
		auto locate = identifier_set_mapping.find (identifier);
		assert (locate != identifier_set_mapping.end ());
		NodeType *old_node = *(locate->second);
		core_priority_queue.erase (locate->second);
		auto it = core_priority_queue.insert (node);
		locate->second = it;
		//别忘了在内存中析构它
		delete old_node;
	}
	NodeType * top (){
		return *(core_priority_queue.begin ());
	}
	void pop (Identifier &identifier){
		identifier_set_mapping.erase (identifier);
		core_priority_queue.erase (core_priority_queue.begin ());
	}
	NodeType * retrieve (Identifier &identifier){
		auto locate = identifier_set_mapping.find (identifier);
		if (locate == identifier_set_mapping.end ())
			return NULL;
		NodeType *old_node = *(locate->second);
		return old_node;
	}
	void erase (Identifier &identifier, bool delete_after = false){
		auto locate = identifier_set_mapping.find (identifier);
		if (locate == identifier_set_mapping.end ())
			return;
		NodeType *old_node = *(locate->second);
		core_priority_queue.erase (locate->second);
		identifier_set_mapping.erase (locate);
		//别忘了在内存中析构它
		if (delete_after)
			delete old_node;
	}
	void erase (NodeType *node, Identifier &identifier, bool delete_after = false){
		auto locate = identifier_set_mapping.find (identifier);
		if (locate == identifier_set_mapping.end ())
			return;
		core_priority_queue.erase (locate->second);
		identifier_set_mapping.erase (locate);
		if (delete_after)
			delete node;
	}
private:
	std::hash_map<Identifier, priority_set_iterator> identifier_set_mapping;
	priority_set core_priority_queue;
};

template <typename NodeType, typename Identifier>
class NormalSetManager {
	typedef typename std::map<Identifier, NodeType*>::iterator normal_set_iterator;
public:
	NormalSetManager (){}
	~NormalSetManager (){
		foreach (auto &p, normal_set)
			delete p.second;
	}
public:
	bool empty () {
		return normal_set.empty ();
	}
	normal_set_iterator begin () {
		return normal_set.begin ();
	}
	normal_set_iterator end () {
		return normal_set.end ();
	}
	void insert (NodeType *new_node, Identifier &identifier){
		normal_set.insert (std::make_pair (identifier, new_node));
	}
	bool exists (Identifier &identifier){
		auto locate = normal_set.find (identifier);
		return locate != normal_set.end ();
	}
	NodeType * retrieve (Identifier &identifier){
		auto locate = normal_set.find (identifier);
		if (locate == normal_set.end ())
			return NULL;
		else
			return locate->second;
	}
	void erase (NodeType *node, Identifier &identifier, bool delete_after = false){
		auto locate = normal_set.find (identifier);
		if (locate == normal_set.end ())
			return;
		normal_set.erase (locate);
		if (delete_after)
			delete node;
	}
private:
	std::map<Identifier, NodeType*> normal_set;
};