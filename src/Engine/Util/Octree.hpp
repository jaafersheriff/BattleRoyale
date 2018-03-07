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

    using Element = std::pair<T, AABox>;

    struct Node {

        friend Octree;

        glm::vec3 center;
        Vector<Element> elements;
        UniquePtr<Node[]> nodes;
        unsigned char active;

        Node();
        Node(const glm::vec3 & center);
        Node(const Node & other) = delete;
        
        Node & operator=(const Node & other) = delete;

        void add(Element && element, int depthRemaining, float radius);

        bool remove(const Element & element);

        size_t filter(const std::function<bool(const AABox &)> & filter, Vector<T> & r_results, float radius) const;
        size_t filter(const AABox & region, Vector<T> & r_results) const;
        size_t filter(const Ray & ray, const glm::vec3 & invDir, Vector<T> & r_results, float radius) const;

        void fragment(float radius);

    };

    public:

    Octree(const AABox & region, float minSize);

    bool add(const T & v, const AABox & region);

    bool remove(const T & v, const AABox & region);

    size_t filter(const std::function<bool(const AABox &)> & filter, Vector<T> & r_results) const;
    size_t filter(const AABox & region, Vector<T> & r_results) const;
    size_t filter(const Ray & ray, Vector<T> & r_results) const;

    private:

    UniquePtr<Node> m_root;
    AABox m_rootRegion;
    float m_rootRadius;
    int m_maxDepth;

};



#include "Octree.tpp"