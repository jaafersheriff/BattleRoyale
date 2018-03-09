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
        Vector<std::pair<T, AABox>> elements;
        UniquePtr<Node[]> children;
        Node * parent;
        unsigned char activeOs;
        unsigned char parentOP;

        Node();
        Node(const glm::vec3 & center, float radius, Node * parent, unsigned char parentOP);
        Node(const Node & other) = delete;
        Node(Node && other) = delete;/* :
            center(other.center),
            radius(other.radius),
            elements(std::move(other.elements)),
            children(std::move(other.children)),
            parent(other.parent),
            activeOs(other.activeOs),
            parentOP(other.parentOP)
        {
            other.parent = nullptr;
            other.activeOs = 0;
            other.parentOP = 0;
        }*/
        
        Node & operator=(const Node & other) = delete;
        Node & operator=(Node && other) = delete;/*{
            center = other.center;
            radius = other.radius;
            elements = std::move(other.elements);
            children = std::move(other.children);
            parent = otehr.parent;
            activeOS = other.activeOs;
            parentOP = other.parentOP;
            other.parent = nullptr;
            other.activeOs = 0;
            other.parentOP = 0;
        }*/

        //~Node() { if (children) delete[] children; }

        bool addUp(const T & v, const AABox & region, float minRadius, UnorderedMap<T, Node *> & r_map);
        void addDown(const T & v, const AABox & region, float minRadius, UnorderedMap<T, Node *> & r_map);

        void remove(const T & v);

        void trim();
        
        size_t filter(const std::function<bool(const glm::vec3 &, float)> & filter, Vector<T> & r_results) const;
        size_t filter(const AABox & region, Vector<T> & r_results) const;
        size_t filter(const Ray & ray, const glm::vec3 & invDir, Vector<T> & r_results) const;

        void fragment();

        AABox region() const;

    };

    public:

    Octree(const AABox & region, float minSize);

    bool add(const T & v, const AABox & region);

    bool set(const T & v, const AABox & region);

    bool remove(const T & v);

    size_t filter(const std::function<bool(const glm::vec3 &, float)> & filter, Vector<T> & r_results) const;
    size_t filter(const AABox & region, Vector<T> & r_results) const;
    size_t filter(const Ray & ray, Vector<T> & r_results) const;

    private:

    UniquePtr<Node> m_root;
    AABox m_rootRegion;
    int m_maxDepth;
    float m_minRadius;
    UnorderedMap<T, Node *> m_map;

};



#include "Octree.tpp"