#include "SceneGraph.h"
//for each item initialise them.
bool SceneGraph::Initialise() {
	for (int x = 0; x < _children.size(); x++) {
		if (!_children[x]->Initialise()) {
			return false;
		}
	}
	return true;
}
//for each item call their update method with new transformation.
void SceneGraph::Update(const Matrix& worldTransformation) {
	for (int x = 0; x < _children.size(); x++) {
		_children[x]->Update(_thisWorldTransformation);
	}
}
//render each item.
void SceneGraph::Render() {
	for (int x = 0; x < _children.size(); x++) {
		_children[x]->Render();
	}
}
//shutdown each item.
void SceneGraph::Shutdown() {
	for (int x = 0; x < _children.size(); x++) {
		_children[x]->Shutdown();
	}
}
//add a new item to the graph.
void SceneGraph::Add(SceneNodePointer node) {
	_children.push_back(node);
}
//remove an item from the graph.
void SceneGraph::Remove(SceneNodePointer node) {
	for (int x = 0; x < _children.size(); x++) {
		if (_children[x] == node) {
			_children[x]->Remove(node);
		}
	}
}
//find an item in a scene graph.
SceneNodePointer SceneGraph::Find(wstring name) {
	//this if statement will stop it searching for items inside of scene graphs if we want to get a scene graph out.
	if (name == _name) {
		return shared_from_this();
	}
	for (int x = 0; x < _children.size(); x++) {
		if (_children[x]->Find(name) != nullptr) {
			return _children[x]->Find(name);
		}
	}
	return nullptr;
}
