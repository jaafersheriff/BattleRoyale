#pragma once



#include <functional>

#include "glm/glm.hpp"
#include "glm/gtx/component_wise.hpp"

#include "Memory.hpp"
#include "Util/Geometry.hpp"



class OctreeShader;



template <typename T>
class Octree {

    friend OctreeShader;

    struct Node {

        friend Octree;

        glm::vec3 center;
        float radius;
        Vector<T> elements;
        UniquePtr<Node[]> children;
        Node * parent;
        unsigned char activeOs;
        unsigned char parentOP;

        Node();
        Node(const glm::vec3 & center, float radius, Node * parent, unsigned char parentOP);
        Node(const Node & other) = delete;
        Node(Node && other) = delete;
        
        Node & operator=(const Node & other) = delete;
        Node & operator=(Node && other) = delete;

    };

    public:

    Octree(const AABox & region, float minSize);

    bool set(const T & e, const AABox & region);

    bool remove(const T & e);

    size_t filter(const std::function<bool(const glm::vec3 &, float)> & f, Vector<T> & r_results) const;
    size_t filter(const AABox & region, Vector<T> & r_results) const;
    size_t filter(const Ray & ray, Vector<T> & r_results) const;
    size_t filter(const T & e, Vector<T> & r_results) const;

    private:

    bool addUp(Node & node, const T & e, const AABox & region);
    void addDown(Node & node, const T & e, const AABox & region);

    void fragment(Node & node);

    void trim(Node & node);
        
    size_t filter(const Node & node, const std::function<bool(const glm::vec3 &, float)> & f, Vector<T> & r_results) const;
    size_t filter(const Node & node, const AABox & region, Vector<T> & r_results) const;
    size_t filter(const Node & node, const Ray & ray, const glm::vec3 & invDir, Vector<T> & r_results) const;

    private:

    UniquePtr<Node> m_root;
    AABox m_rootRegion;
    float m_minRadius;
    UnorderedMap<T, std::pair<Node *, AABox>> m_map;

};



#include "Octree.tpp"