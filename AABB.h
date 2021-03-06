#ifndef AABB_H_
#define AABB_H_

#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include "collid.h"

// header file for AABB tree
// axis-aligned bounding box class
using CPoint = std::vector<double>;
// scope enum type forward declaration
enum class MotionState;

class Node;
class AABBTree;

class AABB {
    friend class Node;
    friend class AABBTree;
    CPoint lowerbound;
    CPoint upperbound;
    // indices will store the index of points on the  
    // corresponding triangle or bond.
    std::vector<long> indices;
    void updateAABBInfo(double);
    //void updateAABBInfo(const std::unordered_map<long, POINT*>&);
    bool contain(const AABB*);
    CD_HSE* hse = nullptr;
    double dt;
    MotionState abType;
public:
    // constructor
    AABB() {}
    AABB(CD_HSE*, MotionState);
    AABB(CD_HSE*, MotionState, double);
    AABB(const CPoint&, const CPoint&);
    // explicit saying that we need a default version of 
    // copy and move operations 
    AABB(const AABB&) = default;
    AABB& operator=(const AABB&) = default;
    AABB(AABB&&) = default;
    AABB& operator=(AABB&&) = default;
    // merge this with anther AABB to get a
    // merged AABB and construct the corresponding AABB tree
    AABB merge(const AABB&) const;
    // get the volume of the AABB
    double volume();
    bool isCollid(const AABB&);
};

// tree node corresponding to AABB
class AABBTree;

class Node {
    friend class AABBTree;
    // AABB stored in node. May store information for branch AABB
    // and may be adjusted for dynamic AABB 
    AABB box;
    // if leaf, point to the corresponding AABB
    // empty for branch
    std::unique_ptr<AABB> data;
    // parent node
    std::weak_ptr<Node> parent;
    // left and right children node
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    void updateBranch();
public:
    // make this node to be brance from two Node parameter
    void setBranch(std::shared_ptr<Node>, std::shared_ptr<Node>, std::shared_ptr<Node>);
    // judge if this node is a leaf
    bool isLeaf();
    // set an AABB element to be a leaf
    void setLeaf(AABB*);
    bool isCollid(Node*);
    void updateAABB();
    Node* getSibling() const;
};

class AABBTree {
    std::shared_ptr<Node> root;
    // node needed to be removed and reinsert to the tree
    std::unordered_map<long, POINT*> ump;
    // map from object's indices (2 or 3 points' global indices)
    // to corresponding CD_HSE* in collision library 
    std::map<std::vector<long>, CD_HSE*> vhMap;
    std::unordered_set<Node*> nodeSet;
    std::vector<std::shared_ptr<Node>> nodeArray;
    int count;
    int numLeaf = 0;
    double treeHeight(Node*); 
    double dt;
    bool isCollsn;
    void queryProximity(Node*, CollisionSolver*);
    bool queryCollision(Node*, CollisionSolver*);
    // insert a node into the subtree with parent 
    // as the root
    void insertNode(std::shared_ptr<Node>, std::shared_ptr<Node>&);
    MotionState type;
public:
    AABBTree(int);
    // don't want tree to be copied or moved
    AABBTree(const AABBTree&) = delete;
    AABBTree& operator=(const AABBTree&) = delete;
    AABBTree(AABBTree&&) = delete;
    AABBTree& operator=(AABBTree&&) = delete;
    // add an AABB element into a tree
    void addAABB(AABB*);
    // query all collid pairs
    void query(CollisionSolver*);
    int getCount() { return count; }
    double getVolume() { return root->box.volume(); } 
    void updateTreeStructure();
    void updatePointMap(const std::vector<CD_HSE*>&);
    void setTimeStep(double t) { dt = t; }
    bool getCollsnState() { return isCollsn; }
    void updateAABBTree(const std::vector<CD_HSE*>&);
    MotionState getType() { return type; }
};

#endif
