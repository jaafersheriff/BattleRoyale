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
        Vector<const T *> elements;
        UniquePtr<Node[]> children;
        Node * parent;
        uint8_t activeOs;
        uint8_t parentO;

        Node();
        Node(const glm::vec3 & center, float radius, Node * parent, uint8_t parentO);
        Node(const Node & other) = delete;
        Node(Node && other) = delete;
        
        Node & operator=(const Node & other) = delete;
        Node & operator=(Node && other) = delete;

    };

    public:

    Octree(const AABox & region, float minSize);

    bool set(const T & e, const AABox & region);

    bool remove(const T & e);

    size_t filter(const std::function<bool(const glm::vec3 &, float)> & f, Vector<const T *> & r_results) const;
    size_t filter(const AABox & region, Vector<const T *> & r_results) const;
    size_t filter(const Ray & ray, Vector<const T *> & r_results) const;
    std::pair<const T *, Intersect> filter(const Ray & ray, const std::function<Intersect(const Ray &, const T &)> & f) const;
    size_t filter(const T & e, Vector<const T *> & r_results) const;

    private:

    bool addUp(Node & node, const T & e, const AABox & region);
    void addDown(Node & node, const T & e, const AABox & region);

    void fragment(Node & node);

    void trim(Node & node);
        
    size_t filter(const Node & node, const std::function<bool(const glm::vec3 &, float)> & f, Vector<const T *> & r_results) const;
    size_t filter(const Node & node, const AABox & region, Vector<const T *> & r_results) const;
    void filter(
        const Node & node, const Ray & ray, const std::function<Intersect(const Ray &, const T &)> & f,
        const glm::vec3 & invDir, const glm::vec3 & signDir, float near, float far, const uint8_t * oMap, const uint8_t * route,
        const T * & r_elem, Intersect & r_inter) const;

    private:

    UniquePtr<Node> m_root;
    AABox m_rootRegion;
    float m_minRadius;
    UnorderedMap<const T *, std::pair<Node *, AABox>> m_map;

};



#include "Octree.tpp"