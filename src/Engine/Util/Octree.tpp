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

inline bool intersects(const AABox & b1, const AABox & b2) {
    return
        b1.max.z > b2.min.z &&
        b1.min.z < b2.max.z &&
        b1.max.y > b2.min.y &&
        b1.min.y < b2.max.y &&
        b1.max.x > b2.min.x &&
        b1.min.x < b2.max.x;
}

inline bool contains(const AABox & b1, const AABox & b2) {
    return
        b1.min.z <= b2.min.z &&
        b1.max.z >= b2.max.z &&
        b1.min.y <= b2.min.y &&
        b1.max.y >= b2.max.y && 
        b1.min.x <= b2.min.x &&
        b1.max.x >= b2.max.x;
}

}



template <typename T>
Octree<T>::Node::Node() :
    center(),
    radius(0.0f),
    elements(),
    children(),
    parent(nullptr),
    activeOs(0),
    parentOP(0)
{}

template <typename T>
Octree<T>::Node::Node(const glm::vec3 & center, float radius, Node * parent, unsigned char parentOP) :
    center(center),
    radius(radius),
    elements(),
    children(),
    parent(parent),
    activeOs(0),
    parentOP(parentOP)
{}

template <typename T>
bool Octree<T>::Node::addUp(const T & v, const AABox & region, float minRadius, UnorderedMap<T, Node *> & r_map) {
    AABox nodeRegion(this->region());
    if (detail::contains(nodeRegion, region)) {
        addDown(v, region, minRadius, r_map);
        return true;
    }

    if (parent) {
        if (!elements.size() && !children) {
            parent->activeOs &= ~parentOP;
            if (!parent->activeOs) {
                parent->children.release();
            }
        }
        return parent->addUp(v, region, minRadius, r_map);
    }
    else {
        if (detail::intersects(nodeRegion, region)) {
            elements.emplace_back(v, region);
            r_map[v] = this;
            return true;
        }
        return false;
    }
}

template <typename T>
void Octree<T>::Node::addDown(const T & v, const AABox & region, float minRadius, UnorderedMap<T, Node *> & r_map) {
    // The node is a leaf. Extra logic necessary
    if (!children) {
        // If the node is empty or at max depth, simply add to elements
        if (!elements.size() || Util::isLE(radius, minRadius)) {
            elements.emplace_back(v, region);
            r_map[v] = this;
        }
        else {
            // If the node only has one element, it may not have been tried
            // to be put into a sub node. Try that now
            if (elements.size() == 1) {
                int o(detail::detOctant(center, elements.front().second));
                if (o >= 0) {
                    fragment();
                    auto & e(elements.front());
                    children[o].addDown(e.first, e.second, minRadius, r_map);
                    activeOs |= 1 << o;
                    elements.clear();       
                }
            }
            // Try to put the new element into a sub node
            int o(detail::detOctant(center, region));
            if (o >= 0) {
                if (!children) fragment();
                children[o].addDown(v, region, minRadius, r_map);
                activeOs |= 1 << o;
            }
            else {
                elements.emplace_back(v, region);
                r_map[v] = this;
            }
        }
    }
    // The typical case where the node is not a leaf
    else {
        int o(detail::detOctant(center, region));
        if (o >= 0) {
            children[o].addDown(v, region, minRadius, r_map);
            activeOs |= 1 << o;
        }
        else {
            elements.emplace_back(v, region);
            r_map[v] = this;
        }
    }
}

template <typename T>
void Octree<T>::Node::remove(const T & v) {
    for (Util::nat i(elements.size() - 1); i >= 0; --i) {
        if (elements[i].first == v) {
            elements.erase(elements.begin() + i);
            break;
        }
    }

    trim();
}

template <typename T>
void Octree<T>::Node::trim() {
    if (!elements.size() && !children && parent) {
        parent->activeOs &= ~parentOP;
        if (!parent->activeOs) {
            parent->children.release();
        }
        parent->trim();
    }
}

template <typename T>
size_t Octree<T>::Node::filter(const std::function<bool(const glm::vec3 &, float)> & filter, Vector<T> & r_results) const {
    size_t n(elements.size());
    for (const auto e & elements) {
        r_results.push_back(e.first);
    }
    if (children) {
        for (unsigned char o(0), op(1); o < 8; ++o, op <<= 1) {
            if (activeOs & op && filter(children[o].center, children[o].radius)) {
                n += children[o].filter(filter, r_results);
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

    if (children) {
        unsigned char possible(activeOs);
        if (region_.max.z <= center.z) possible &= unsigned char(0x0F);
        if (region_.min.z >= center.z) possible &= unsigned char(0xF0);
        if (region_.max.y <= center.y) possible &= unsigned char(0x33);
        if (region_.min.y >= center.y) possible &= unsigned char(0xCC);
        if (region_.max.x <= center.x) possible &= unsigned char(0x55);
        if (region_.min.x >= center.x) possible &= unsigned char(0xAA);
        for (unsigned char o(0), op(1); o < 8; ++o, op <<= 1) {
            if (possible & op) {
                n += children[o].filter(region_, r_results);
            }
        }
    }

    return n;
}

// least efficient
template <typename T>
size_t Octree<T>::Node::filter(const Ray & ray, const glm::vec3 & invDir, Vector<T> & r_results) const {
    size_t n(elements.size());    
    for (const auto & e : elements) {
        r_results.push_back(e.first);
    }
    
    if (children) {
        for (unsigned char o(0), op(1); o < 8; ++o, op <<= 1) {
            if (activeOs & op) {
                const Node & node(children[o]);
                float near, far;
                if (detail::intersect(ray, invDir, node.center - node.radius, node.center + node.radius, near, far)) {
                    n += node.filter(ray, invDir, r_results);
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

    unsigned char possible(activeOs);



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
void Octree<T>::Node::fragment() {
    children = UniquePtr<Node[]>::make(8);
    float hr(radius * 0.5f);
    glm::vec3 min(center - hr), max(center + hr);

    children[0].center.x = min.x; children[0].center.y = min.y; children[0].center.z = min.z;
    children[1].center.x = max.x; children[1].center.y = min.y; children[1].center.z = min.z;
    children[2].center.x = min.x; children[2].center.y = max.y; children[2].center.z = min.z;
    children[3].center.x = max.x; children[3].center.y = max.y; children[3].center.z = min.z;
    children[4].center.x = min.x; children[4].center.y = min.y; children[4].center.z = max.z;
    children[5].center.x = max.x; children[5].center.y = min.y; children[5].center.z = max.z;
    children[6].center.x = min.x; children[6].center.y = max.y; children[6].center.z = max.z;
    children[7].center.x = max.x; children[7].center.y = max.y; children[7].center.z = max.z;
    
    for (int o(0); o < 8; ++o) {
        children[o].radius = hr;
        children[o].parent = this;
        children[o].parentOP = 1 << o;
    }
}

template <typename T>
AABox Octree<T>::Node::region() const {
    return AABox(center - radius, center + radius);
}



template <typename T>
Octree<T>::Octree(const AABox & region, float minSize) {
    Util::nat iSize(Util::floor(glm::max(glm::compMax(region.max - region.min) / minSize, 1.0f)));
    iSize = Util::ceil2(iSize); // round up to nearest power of 2
    m_maxDepth = int(Util::log2Floor(iSize));
    m_minRadius = minSize * 0.5f;
    m_root = UniquePtr<Node>::make(region.center(), iSize * m_minRadius, nullptr, -1);
    m_rootRegion.min = m_root->center - m_root->radius;
    m_rootRegion.max = m_root->center + m_root->radius;
}

template <typename T>
bool Octree<T>::add(const T & v, const AABox & region) {
    if (detail::isIntersect(region, m_rootRegion)) {
        m_root->add(v, region, m_minRadius, m_map);
        return true;
    }
    return false;
}

template <typename T>
bool Octree<T>::set(const T & v, const AABox & region) {
    auto it(m_map.find(v));
    if (it != m_map.end()) {
        Node & node(*it->second);
        m_map.erase(it);
        for (Util::nat i(node.elements.size() - 1); i >= 0; --i) {
            if (node.elements[i].first == v) {
                node.elements.erase(node.elements.begin() + i);
                break;
            }
        }
        return node.addUp(v, region, m_minRadius, m_map);
    }
    else {
        if (detail::intersects(m_rootRegion, region)) {
            m_root->addDown(v, region, m_minRadius, m_map);
            return true;
        }
        return false;
    }
}

template <typename T>
bool Octree<T>::remove(const T & v) {
    auto it(m_map.find(v));
    if (it != m_map.end()) {
        it->second->remove(v);
        m_map.erase(it);
        return true;
    }
    return false;
}

template <typename T>
size_t Octree<T>::filter(const std::function<bool(const glm::vec3 &, float)> & filter, Vector<T> & r_results) const {
    return filter(m_root->center, m_root->radius) ? m_root->filter(filter, r_results) : 0;
}

template <typename T>
size_t Octree<T>::filter(const AABox & region, Vector<T> & r_results) const {
    return detail::intersects(m_rootRegion, region) ? m_root->filter(region, r_results) : 0;
}

template <typename T>
size_t Octree<T>::filter(const Ray & ray, Vector<T> & r_results) const {
    glm::vec3 invDir(
        Util::isZero(ray.dir.x) ? Util::infinity() : 1.0f / ray.dir.x,
        Util::isZero(ray.dir.y) ? Util::infinity() : 1.0f / ray.dir.y,
        Util::isZero(ray.dir.z) ? Util::infinity() : 1.0f / ray.dir.z
    );
    float near, far;
    return detail::intersect(ray, invDir, m_rootRegion.min, m_rootRegion.max, near, far) ? m_root->filter(ray, invDir, r_results) : 0;
}
