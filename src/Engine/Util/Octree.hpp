#pragma once



#include <functional>

#include "glm/glm.hpp"
#include "glm/gtx/component_wise.hpp"

#include "Memory.hpp"
#include "Util/Geometry.hpp"



class OctreeShader;



template <typename T>
class Octree {

    static_assert(sizeof(T) <= sizeof(intptr_t), "T must be no larger than word size");
    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    static_assert(std::is_copy_constructible<T>::value, "T must be copy constructable");
    static_assert(std::is_copy_assignable<T>::value, "T must be copy assignable");

    friend OctreeShader;

    struct Node {

        friend Octree;

        glm::vec3 center;
        float radius;
        Vector<T> elements;
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

    bool set(T e, const AABox & region);

    bool remove(T e);

    void clear();

    // Retrieves all elements within nodes that pass the given function.
    // F takes the center and radius of a node and returns whether it should be included.
    size_t filter(const std::function<bool(const glm::vec3 &, float)> & f, Vector<T> & r_results) const;
    // Retrieves all elements within all nodes intersecting the given region.
    size_t filter(const AABox & region, Vector<T> & r_results) const;
    // Retrieves all elements within all nodes intersecting the given ray.
    size_t filter(const Ray & ray, Vector<T> & r_results) const;
    // Retrieves the nearest element and intersection with the given ray.
    // F takes a ray and an element and returns an Intersect.
    // FAR more efficient than the above method when only the nearest element is desired.
    std::pair<T, Intersect> filter(const Ray & ray, const std::function<Intersect(const Ray &, T)> & f) const;
    // Retrieves all elements within all nodes intersecting the region of the given element.
    size_t filter(T e, Vector<T> & r_results) const;

    private:

    bool addUp(Node & node, T e, const AABox & region);
    void addDown(Node & node, T e, const AABox & region);

    void fragment(Node & node);

    void trim(Node & node);
    
    size_t filter(const Node & node, const std::function<bool(const glm::vec3 &, float)> & f, Vector<T> & r_results) const;
    size_t filter(const Node & node, const AABox & region, Vector<T> & r_results) const;
    size_t filter(const Node & node, const Ray & ray, const glm::vec3 & invDir, Vector<T> & r_results) const;
    void filter(
        const Node & node, const Ray & ray, const std::function<Intersect(const Ray &, T)> & f,
        const glm::vec3 & invDir, const glm::vec3 & signDir, float near, float far, const uint8_t * oMap,
        T & r_elem, Intersect & r_inter
    ) const;

    private:

    UniquePtr<Node> m_root;
    AABox m_rootRegion;
    float m_minRadius;
    UnorderedMap<T, std::pair<Node *, AABox>> m_map;

};



#include "Octree.tpp"