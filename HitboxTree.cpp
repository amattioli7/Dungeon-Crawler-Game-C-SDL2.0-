// HitboxTree.cpp
#include "HitboxTree.h"

HitboxTree::HitboxTree() {
	return;
}

void HitboxTree::setRoot(SDL_Rect rect) {
	this->root = new HitboxTree::Node(rect);
}

void HitboxTree::addChild(SDL_Rect rect) {
	HitboxTree::Node *n = new HitboxTree::Node(rect);
	this->root->children.push_back(n);
}

SDL_Rect HitboxTree::getRootHitbox() {
	return this->root->hitbox;
}