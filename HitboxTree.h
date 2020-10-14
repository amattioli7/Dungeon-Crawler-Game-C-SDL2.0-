#include <iostream>
#include <vector>
#include <SDL.h>

#ifndef HITBOXTREE_H
#define HITBOXTREE_H

class HitboxTree {
private:
	class Node {
	public:
		SDL_Rect hitbox;
		std::vector<Node*> children;


		Node(SDL_Rect rect) {
			hitbox = rect;
		}
	};
public:
	Node *root;	// should be large hitbox covering entirety
						// of sprite

	HitboxTree();
	
	void setRoot(SDL_Rect r);	// assign SDL_Rect r to root node
	
	void addChild(SDL_Rect r);	// add SDL_Rect r as child in tree
								// NOTE: The children should be smaller hitboxes
								// within the larger root hitbox

	SDL_Rect getRootHitbox();
};

#endif