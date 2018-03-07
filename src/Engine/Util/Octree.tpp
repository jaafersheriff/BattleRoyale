namespace detail {

inline int detOctant(const glm::vec3 & center, const AABox & region) {
    if (region.max.z < center.z) {
        if (region.max.y < center.y) {
            if (region.max.x < center.x) {
                return 0;
            }
            else if (region.min.x > center.x) {
                return 1;
            }
        }
        else if (region.min.y > center.y) {            
            if (region.max.x < center.x) {
                return 2;
            }
            else if (region.min.x > center.x) {
                return 3;
            }
        }
    }
    else if (region.min.z > center.z) {
        if (region.max.y < center.y) {
            if (region.max.x < center.x) {
                return 4;
            }
            else if (region.min.x > center.x) {
                return 5;
            }
        }
        else if (region.min.y > center.y) {            
            if (region.max.x < center.x) {
                return 6;
            }
            else if (region.min.x > center.x) {
                return 7;
            }
        }
    }
    return -1;
}

inline bool intersect(const Ray & ray, const glm::vec3 & invDir, const glm::vec3 & min, const glm::vec3 & max, float & r_near, float & r_far) {
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

inline bool isIntersect(const AABox & b1, const AABox & b2) {    
    return
        b1.max.z > b2.min.z ||
        b1.min.z < b2.max.z ||
        b1.max.y > b2.min.y ||
        b1.min.y < b2.max.y ||
        b1.max.x > b2.min.x ||
        b1.min.x < b2.max.x;
}

}



template <typename T>
Octree<T>::Node::Node() :
    center(),
    elements(),
    nodes(),
    active(0)
{}


template <typename T>
Octree<T>::Node::Node(const glm::vec3 & center) :
    center(center),
    elements(),
    nodes(),
    active(0)
{}

template <typename T>
void Octree<T>::Node::add(Element && element, int depthRemaining, float radius) {
    // The node is a leaf. Extra logic necessary
    if (!nodes) {
        // If the node is empty or at max depth, simply add to elements
        if (!elements.size() || !depthRemaining) {
            elements.emplace_back(std::move(element));
        }
        else {
            // If the node only has one element, it may not have been tried
            // to be put into a sub node. Try that now
            if (elements.size() == 1) {
                int o(detail::detOctant(center, elements.front().second));
                if (o >= 0) {
                    fragment(radius);
                    nodes[o].add(std::move(elements.front()), depthRemaining - 1, radius * 0.5f);
                    active |= 1 << o;
                    elements.clear();       
                }
            }
            // Try to put the new element into a sub node
            int o(detail::detOctant(center, element.second));
            if (o >= 0) {
                if (!nodes) fragment(radius);
                nodes[o].add(std::move(element), depthRemaining - 1, radius * 0.5f);
                active |= 1 << o;
            }
            else {
                elements.emplace_back(std::move(element));
            }
        }
    }
    // The typical case where the node is not a leaf
    else {
        int o(detail::detOctant(center, element.second));
        if (o >= 0) {
            nodes[o].add(std::move(element), depthRemaining - 1, radius * 0.5f);
            active |= 1 << o;
        }
        else {
            elements.emplace_back(std::move(element));
        }
    }
}

template <typename T>
bool Octree<T>::Node::remove(const Element & element) {
    int o;
    if (nodes) o = detail::detOctant(center, element.second);
    if (!nodes || o == -1) {
        for (auto it(elements.begin()); it != elements.end(); ++it) {
            if (it->first == element.first) {
                elements.erase(it);
                return true;
            }
        }
    }
    else {
        int op(1 << o);
        if (active & op) {
            Node & node(nodes[o]);
            node.remove(element);
            if (node.elements.size() == 0 && !node.nodes) {
                active ^= op;
                if (!active) {
                    nodes.release();
                }
            }
            return true;
        }
    }
    return false;
}

template <typename T>
size_t Octree<T>::Node::filter(const std::function<bool(const AABox &)> & filter, Vector<T> & r_results, float radius) const {
    size_t n(elements.size());
    for (const auto e & elements) {
        r_results.push_back(e.first);
    }
    if (nodes) {
        for (int o(0), op(1); o < 8; ++o, op <<= 1) {
            if (active & op && filter(AABox(nodes[o].center - radius, nodes[o].center + radius))) {
                n += nodes[o].filter(filter, r_results, radius * 0.5f);
            }
        }
    }
    return n;
}

template <typename T>
size_t Octree<T>::Node::filter(const AABox & region_, Vector<T> & r_results) const {
    size_t n(elements.size());    
    for (const auto & e : elements) {
        r_results.push_back(e.first);
    }

    if (nodes) {
        unsigned char possible(active);
        if (region_.max.z <= center.z) possible &= unsigned char(0x0F);
        if (region_.min.z >= center.z) possible &= unsigned char(0xF0);
        if (region_.max.y <= center.y) possible &= unsigned char(0x33);
        if (region_.min.y >= center.y) possible &= unsigned char(0xCC);
        if (region_.max.x <= center.x) possible &= unsigned char(0x55);
        if (region_.min.x >= center.x) possible &= unsigned char(0xAA);
        for (int o(0), op(1); o < 8; ++o, op <<= 1) {
            if (possible & op) {
                n += nodes[o].filter(region_, r_results);
            }
        }
    }

    return n;
}

// least efficient
template <typename T>
size_t Octree<T>::Node::filter(const Ray & ray, const glm::vec3 & invDir, Vector<T> & r_results, float radius) const {
    size_t n(elements.size());    
    for (const auto & e : elements) {
        r_results.push_back(e.first);
    }
    
    if (nodes) {
        for (int o(0), op(1); o < 8; ++o, op <<= 1) {
            if (active & op) {
                const Node & node(nodes[o]);
                float near, far;
                if (detail::intersect(ray, invDir, node.center - radius * 0.5f, node.center + radius * 0.5f, near, far)) {
                    n += node.filter(ray, invDir, r_results, radius * 0.5f);
                }
            }
        }
    }

    return n;
}

/*template <typename T>
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
} */

template <typename T>
void Octree<T>::Node::fragment(float radius) {
    nodes = UniquePtr<Node[]>::make(8);
    float hr(radius * 0.5f);
    glm::vec3 min(center - hr), max(center + hr);

    nodes[0].center.x = min.x; nodes[0].center.y = min.y; nodes[0].center.z = min.z;
    nodes[1].center.x = max.x; nodes[1].center.y = min.y; nodes[1].center.z = min.z;
    nodes[2].center.x = min.x; nodes[2].center.y = max.y; nodes[2].center.z = min.z;
    nodes[3].center.x = max.x; nodes[3].center.y = max.y; nodes[3].center.z = min.z;
    nodes[4].center.x = min.x; nodes[4].center.y = min.y; nodes[4].center.z = max.z;
    nodes[5].center.x = max.x; nodes[5].center.y = min.y; nodes[5].center.z = max.z;
    nodes[6].center.x = min.x; nodes[6].center.y = max.y; nodes[6].center.z = max.z;
    nodes[7].center.x = max.x; nodes[7].center.y = max.y; nodes[7].center.z = max.z;
}



template <typename T>
Octree<T>::Octree(const AABox & region, float minSize) {
    Util::nat iSize(Util::floor(glm::max(glm::compMax(region.max - region.min) / minSize, 1.0f)));
    iSize = Util::ceil2(iSize); // round up to nearest power of 2
    m_maxDepth = int(Util::log2Floor(iSize));
    m_rootRadius = iSize * 0.5f * minSize;
    m_root = UniquePtr<Node>::make(region.center());
}

template <typename T>
bool Octree<T>::add(const T & v, const AABox & region) {
    if (detail::isIntersect(region, m_rootRegion)) {
        m_root->add(Element(v, region), m_maxDepth + 1, m_rootRadius);
        return true;
    }
    return false;
}

template <typename T>
bool Octree<T>::remove(const T & v, const AABox & region) {
    if (detail::isIntersect(region, m_rootRegion)) {
        return m_root->remove(Element(v, region));
    }
    return false;
}

template <typename T>
size_t Octree<T>::filter(const std::function<bool(const AABox &)> & filter, Vector<T> & r_results) const {
    return filter(m_rootRegion) ? m_root->filter(filter, r_results, m_rootRadius) : 0;
}

template <typename T>
size_t Octree<T>::filter(const AABox & region, Vector<T> & r_results) const {
    return detail::isIntersect(region, m_rootRegion) ? m_root->filter(region, r_results) : 0;
}

template <typename T>
size_t Octree<T>::filter(const Ray & ray, Vector<T> & r_results) const {
    glm::vec3 invDir(
        Util::isZero(ray.dir.x) ? Util::infinity() : 1.0f / ray.dir.x,
        Util::isZero(ray.dir.y) ? Util::infinity() : 1.0f / ray.dir.y,
        Util::isZero(ray.dir.z) ? Util::infinity() : 1.0f / ray.dir.z
    );
    float near, far;
    return detail::intersect(ray, invDir, m_rootRegion.min, m_rootRegion.max, near, far) ? m_root->filter(ray, invDir, r_results, m_rootRadius) : 0;
}
