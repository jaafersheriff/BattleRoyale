namespace detail {

inline int detOctant(const glm::vec3 & center, const AABox & bounds) {
    if (bounds.max.z < center.z) {
        if (bounds.max.y < center.y) {
            if (bounds.max.x < center.x) {
                return 0;
            }
            else if (bounds.min.x > center.x) {
                return 1;
            }
        }
        else if (bounds.min.y > center.y) {            
            if (bounds.max.x < center.x) {
                return 2;
            }
            else if (bounds.min.x > center.x) {
                return 3;
            }
        }
    }
    else if (bounds.min.z > center.z) {
        if (bounds.max.y < center.y) {
            if (bounds.max.x < center.x) {
                return 4;
            }
            else if (bounds.min.x > center.x) {
                return 5;
            }
        }
        else if (bounds.min.y > center.y) {            
            if (bounds.max.x < center.x) {
                return 6;
            }
            else if (bounds.min.x > center.x) {
                return 7;
            }
        }
    }
    return -1;
}

bool intersectAABox(const Ray & ray, const glm::vec3 & invDir, glm::vec3 & min, const glm::vec3 & max, float & r_near, float & r_far) {
    glm::vec3 tsLow((min - ray.pos) * invDir);
    glm::vec3 tsHigh((max - ray.pos) * invDir);

    float tMin(glm::compMax(glm::min(tsLow, tsHigh)));
    float tMax(glm::compMin(glm::max(tsLow, tsHigh)));
    
    if (tMax <= 0.0f || !(tMax >= tMin)) { // extra negation so that a NaN case returns no intersection
        return false;
    }

    r_near = tMin;
    r_far = tMax;
    return true;
}

}



template <typename T>
Octree<T>::Node::Node(const glm::vec3 & min, const glm::vec3 & max) :
    min(min),
    max(max),
    elements(),
    nodes(),
    active(0)
{}

template <typename T>
void Octree<T>::Node::add(Element && element) {
    // The node is a leaf. Extra logic necessary
    if (!nodes) {
        // If the node is empty, simply add to elements
        if (!elements.size()) {
            elements.emplace_back(std::move(element));
        }
        else {
            glm::vec3 center(region.center());
            // If the node only has one element, it will not have been tried
            // to be put into a sub node. Try that now
            if (elements.size() == 1) {
                int o(detail::detOctant(center, elements.front().second));
                if (o >= 0) {
                    nodes = UniquePtr<Node[]>::make(8);
                    nodes[o].add(std::move(elements.front()));
                    active |= 1 << o;
                    elements.clear();       
                }
            }
            // Try to put the new element into a sub node
            int octant(detail::detOctant(center, element.second));
            if (octant >= 0) {
                if (!nodes) {
                    nodes = UniquePtr<Node[]>::make(8);
                }
                nodes[octant].add(std::move(element));
                active |= 1 << octant;
            }
            else {
                elements.emplace_back(std::move(element));
            }
        }
    }
    // The typical case where the node is not a leaf
    else {
        int octant(detail::detOctant(region.center(), element.second));
        if (octant >= 0) {
            nodes[octant].add(std::move(element));
            active |= 1 << octant;
        }
        else {
            elements.emplace_back(std::move(element));
        }
    }
}

template <typename T>
void Octree<T>::Node::remove(const Element & element) {
    if (!nodes) {
        for (auto it(elements.begin()); it != elements.end(); ++it) {
            if (it->first == element.first) {
                elements.erase(it);
                break;
            }
        }
    }
    else {
        int octant(detail::detOctant(region.center(), element.second));
        if (octant >= 0) {
            int op(1 << octant);
            if (active & op) {
                Node & node(nodes[octant]);
                node.remove(element);
                if (node.elements.size() == 0 && !node.nodes) {
                    active ^= op;
                    if (!active) {
                        nodes.release();
                    }
                }
            }
        }
        else {
            for (auto it(elements.begin()); it != elements.end(); ++it) {
                if (it->first == element.first) {
                    elements.erase(it);
                    break;
                }
            }
        }
    }
}

template <typename T>
size_t Octree<T>::Node::filter(const std::function<bool(const AABox &)> & filter, Vector<const T *> & r_results) const {
    size_t n(elements.size());
    for (const auto e & elements) {
        r_results.push_back(e.first);
    }
    for (int o(0), op(1); o < 8; ++o, op <<= 1) {
        if (active & op && filter(nodes[o].region)) {
            n += nodes[o].filter(filter, r_results);
        }
    }
    return n;
}

template <typename T>
size_t Octree<T>::Node::filter(const glm::vec3 & min, const glm::vec3 & max, Vector<const T *> & r_results) const {
    size_t n(elements.size());    
    for (const auto e & elements) {
        r_results.push_back(e.first);
    }

    glm::vec3 center(region.center());
    unsigned char possible(active);
    if (max.z <= center.z) possible &= unsigned char(0x0F);
    if (min.z >= center.z) possible &= unsigned char(0xF0);
    if (max.y <= center.y) possible &= unsigned char(0x33);
    if (min.y >= center.y) possible &= unsigned char(0xCC);
    if (max.x <= center.x) possible &= unsigned char(0x55);
    if (min.x >= center.x) possible &= unsigned char(0xAA);
    for (int o(0), op(1); o < 8; ++o, op <<= 1) {
        if (possible & op) {
            n += nodes[o].filter(min, max, r_results);
        }
    }

    return n;
}

// least efficient
template <typename T>
size_t Octree<T>::Node::filter(const Ray & ray, const glm::vec3 & invDir, Vector<const T *> & r_results) const {
    size_t n(elements.size());    
    for (const auto e & elements) {
        r_results.push_back(e.first);
    }
    
    for (int o(0), op(1); o < 8; ++o, op <<= 1) {
        if (active & op) {
            const Node & node(nodes[o]);
            float near, far;
            if (detail::intersectAABox(ray, invDir, node.region.min, node.region.min, near, far)) {
                n += node.filter(ray, invDir, r_results);
            }
        }
    }

    return n;
}

template <typename T>
size_t Octree<T>::Node::filter(const Ray & ray, const glm::vec3 & invDir, Vector<const T *> & r_results) const {
    glm::vec3 invDir(1.0f / ray.dir);

    glm::vec3 center(region.center());

    glm::vec3 tsLow((region.min - ray.pos) * invDir);
    glm::vec3 tsMid((center - ray.pos) * invDir);
    glm::vec3 tsHigh((region.max - ray.pos) * invDir);

    unsigned char possible(active);



    // if mid.z is hit before low.x or low.y, cull lowbox.z
    // if mid.y is hit before low.x or low.z, cull lowbox.y
    // if mid.x is hit before low.y or low.z, cull lowbox.x

    // if mid.z is not hit before high.x and high.y, cull highbox.z
    // "
    // "

    // if mid.z is hit before mid.x, cull lowbox.xz
    // """""""

    // if mid.x is hit before mid.z, cull highbox.xz
    // """""""


    float tMin(glm::compMax(glm::min(tsLow, tsHigh)));
    float tMax(glm::compMin(glm::max(tsLow, tsHigh)));
    
    if (tMax <= 0.0f || !(tMax >= tMin)) { // extra negation so that a NaN case returns no intersection
        return false;
    }

    r_near = tMin;
    r_far = tMax;
    return true;
} 

template <typename T>
size_t Octree<T>::Node::filter(const Ray & ray, const glm::vec3 & invDir, Vector<const T *> & r_results) const {
    glm::vec3 invDir(1.0f / ray.dir);

    glm::vec3 center(region.center());

    glm::vec3 tsLow((region.min - ray.pos) * invDir);
    glm::vec3 tsMid((center - ray.pos) * invDir);
    glm::vec3 tsHigh((region.max - ray.pos) * invDir);

    unsigned char possible(active);



    // if mid.z is hit before low.x or low.y, cull lowbox.z
    // if mid.y is hit before low.x or low.z, cull lowbox.y
    // if mid.x is hit before low.y or low.z, cull lowbox.x

    // if mid.z is not hit before high.x and high.y, cull highbox.z
    // "
    // "

    // if mid.z is hit before mid.x, cull lowbox.xz
    // """""""

    // if mid.x is hit before mid.z, cull highbox.xz
    // """""""


    float tMin(glm::compMax(glm::min(tsLow, tsHigh)));
    float tMax(glm::compMin(glm::max(tsLow, tsHigh)));
    
    if (tMax <= 0.0f || !(tMax >= tMin)) { // extra negation so that a NaN case returns no intersection
        return false;
    }

    r_near = tMin;
    r_far = tMax;
    return true;
}



template <typename T>
Octree<T>::Octree(const glm::vec3 & min, const glm::vec3 & max) :
    m_root()
{
    glm::vec3 size(max - min);
    Util::nat iSize(glm::compMax(size));
    assert(iSize >= 0);
    iSize = Util::ceil2(iSize); // round up to nearest power of 2
    if (iSize < 1) iSize = 1;
    size = glm::vec3(float(iSize));
    glm::vec3 center((min + max) * 0.5f);

    m_root = UniquePtr<Node>::make(center - size * 0.5f, center + size * 0.5f);
}
