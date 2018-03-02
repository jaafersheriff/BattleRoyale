#pragma once



#include <functional>

#include "glm/glm.hpp"
#include "glm/gtx/component_wise.hpp"

#include "Memory.hpp"
#include "Util/Geometry.hpp"



template <typename T>
class Octree {

    using Element = std::pair<const T *, AABox>;

    struct Node {

        friend Octree;

        AABox region;
        Vector<Element> elements;
        UniquePtr<Node[]> nodes;
        unsigned char active;

        Node(const glm::vec3 & min, const glm::vec3 & max);
        Node(const Node & other) = delete;
        
        Node & operator=(const Node & other) = delete;

        void add(Element && element);

        void remove(const Element & element);

        size_t filter(const std::function<bool(const AABox &)> & filter, Vector<const T *> & r_results) const;
        size_t filter(const glm::vec3 & min, const glm::vec3 & max, Vector<const T *> & r_results) const;
        size_t filter(const Ray & ray, const glm::vec3 & invDir, Vector<const T *> & r_results) const;

    };

    public:

    Octree(const glm::vec3 & min, const glm::vec3 & max);

    private:

    UniquePtr<Node> m_root;

};



#include "Octree.tpp"