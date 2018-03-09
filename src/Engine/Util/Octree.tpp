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
Octree<T>::Octree(const AABox & region, float minSize) {
    Util::nat iSize(Util::floor(glm::max(glm::compMax(region.max - region.min) / minSize, 1.0f)));
    iSize = Util::ceil2(iSize); // round up to nearest power of 2
    m_minRadius = minSize * 0.5f;
    m_root = UniquePtr<Node>::make(region.center(), iSize * m_minRadius, nullptr, -1);
    m_rootRegion.min = m_root->center - m_root->radius;
    m_rootRegion.max = m_root->center + m_root->radius;
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
        bool res(addUp(node, v, region));
        trim(node);
        return res;
    }
    else {
        if (detail::intersects(m_rootRegion, region)) {
            addDown(*m_root, v, region);
            return true;
        }
        return false;
    }
}

template <typename T>
bool Octree<T>::remove(const T & v) {
    auto it(m_map.find(v));
    if (it == m_map.end()) {
        return false;
    }

    Node & node(*it->second);
    for (Util::nat i(node.elements.size() - 1); i >= 0; --i) {
        if (node.elements[i].first == v) {
            node.elements.erase(node.elements.begin() + i);
            break;
        }
    }

    trim(node);

    m_map.erase(it);
    return true;
}

template <typename T>
size_t Octree<T>::filter(const std::function<bool(const glm::vec3 &, float)> & f, Vector<T> & r_results) const {
    return f(m_root->center, m_root->radius) ? filter(*m_root, f, r_results) : 0;
}

template <typename T>
size_t Octree<T>::filter(const AABox & region, Vector<T> & r_results) const {
    return detail::intersects(m_rootRegion, region) ? filter(*m_root, region, r_results) : 0;
}

template <typename T>
size_t Octree<T>::filter(const Ray & ray, Vector<T> & r_results) const {
    glm::vec3 invDir(
        Util::isZero(ray.dir.x) ? Util::infinity() : 1.0f / ray.dir.x,
        Util::isZero(ray.dir.y) ? Util::infinity() : 1.0f / ray.dir.y,
        Util::isZero(ray.dir.z) ? Util::infinity() : 1.0f / ray.dir.z
    );
    float near, far;
    return detail::intersect(ray, invDir, m_rootRegion.min, m_rootRegion.max, near, far) ? filter(*m_root, ray, invDir, r_results) : 0;
}

template <typename T>
bool Octree<T>::addUp(Node & node, const T & v, const AABox & region) {
    AABox nodeRegion(node.center - node.radius, node.center + node.radius);
    if (detail::contains(nodeRegion, region)) {
        addDown(node, v, region);
        return true;
    }
    else {
        if (node.parent) {
            return addUp(*node.parent, v, region);
        }
        else {
            if (detail::intersects(nodeRegion, region)) {
                node.elements.emplace_back(v, region);
                m_map[v] = &node;
                return true;
            }
            return false;
        }
    }
}

template <typename T>
void Octree<T>::addDown(Node & node, const T & v, const AABox & region) {
    // The node is a leaf. Extra logic necessary
    if (!node.children) {
        // If the node is empty or at max depth, simply add to elements
        if (!node.elements.size() || Util::isLE(node.radius, m_minRadius)) {
            node.elements.emplace_back(v, region);
            m_map[v] = &node;
        }
        else {
            // If the node only has one element, it may not have been tried
            // to be put into a sub node. Try that now
            if (node.elements.size() == 1) {
                int o(detail::detOctant(node.center, node.elements.front().second));
                if (o >= 0) {
                    fragment(node);
                    auto & e(node.elements.front());
                    addDown(node.children[o], e.first, e.second);
                    node.activeOs |= 1 << o;
                    node.elements.clear();       
                }
            }
            // Try to put the new element into a sub node
            int o(detail::detOctant(node.center, region));
            if (o >= 0) {
                if (!node.children) fragment(node);
                addDown(node.children[o], v, region);
                node.activeOs |= 1 << o;
            }
            else {
                node.elements.emplace_back(v, region);
                m_map[v] = &node;
            }
        }
    }
    // The typical case where the node is not a leaf
    else {
        int o(detail::detOctant(node.center, region));
        if (o >= 0) {
            addDown(node.children[o], v, region);
            node.activeOs |= 1 << o;
        }
        else {
            node.elements.emplace_back(v, region);
            m_map[v] = &node;
        }
    }
}

template <typename T>
void Octree<T>::fragment(Node & node) {
    node.children = UniquePtr<Node[]>::make(8);
    float hr(node.radius * 0.5f);
    glm::vec3 min(node.center - hr), max(node.center + hr);

    node.children[0].center.x = min.x; node.children[0].center.y = min.y; node.children[0].center.z = min.z;
    node.children[1].center.x = max.x; node.children[1].center.y = min.y; node.children[1].center.z = min.z;
    node.children[2].center.x = min.x; node.children[2].center.y = max.y; node.children[2].center.z = min.z;
    node.children[3].center.x = max.x; node.children[3].center.y = max.y; node.children[3].center.z = min.z;
    node.children[4].center.x = min.x; node.children[4].center.y = min.y; node.children[4].center.z = max.z;
    node.children[5].center.x = max.x; node.children[5].center.y = min.y; node.children[5].center.z = max.z;
    node.children[6].center.x = min.x; node.children[6].center.y = max.y; node.children[6].center.z = max.z;
    node.children[7].center.x = max.x; node.children[7].center.y = max.y; node.children[7].center.z = max.z;
    
    for (int o(0); o < 8; ++o) {
        node.children[o].radius = hr;
        node.children[o].parent = &node;
        node.children[o].parentOP = 1 << o;
    }
}

template <typename T>
void Octree<T>::trim(Node & node_) {
    Node * node(&node_);
    while (node) {
        if (node->elements.size() || node->children || !node->parent) {
            return;
        }

        node->parent->activeOs &= ~node->parentOP;
        if (!node->parent->activeOs) {
            node->parent->children.release();
        }

        node = node->parent;
    }
}

template <typename T>
size_t Octree<T>::filter(const Node & node, const std::function<bool(const glm::vec3 &, float)> & f, Vector<T> & r_results) const {
    size_t n(node.elements.size());
    for (const auto & e : node.elements) {
        r_results.push_back(e.first);
    }
    if (node.children) {
        for (unsigned char o(0), op(1); o < 8; ++o, op <<= 1) {
            if (node.activeOs & op && f(node.children[o].center, node.children[o].radius)) {
                n += filter(node.children[o], f, r_results);
            }
        }
    }
    return n;
}

template <typename T>
size_t Octree<T>::filter(const Node & node, const AABox & region_, Vector<T> & r_results) const {
    size_t n(node.elements.size());    
    for (const auto & e : node.elements) {
        r_results.push_back(e.first);
    }

    if (node.children) {
        unsigned char possible(node.activeOs);
        if (region_.max.z <= node.center.z) possible &= unsigned char(0x0F);
        if (region_.min.z >= node.center.z) possible &= unsigned char(0xF0);
        if (region_.max.y <= node.center.y) possible &= unsigned char(0x33);
        if (region_.min.y >= node.center.y) possible &= unsigned char(0xCC);
        if (region_.max.x <= node.center.x) possible &= unsigned char(0x55);
        if (region_.min.x >= node.center.x) possible &= unsigned char(0xAA);
        for (unsigned char o(0), op(1); o < 8; ++o, op <<= 1) {
            if (possible & op) {
                n += filter(node.children[o], region_, r_results);
            }
        }
    }

    return n;
}

// least efficient
template <typename T>
size_t Octree<T>::filter(const Node & node, const Ray & ray, const glm::vec3 & invDir, Vector<T> & r_results) const {
    size_t n(node.elements.size());    
    for (const auto & e : node.elements) {
        r_results.push_back(e.first);
    }
    
    if (node.children) {
        for (unsigned char o(0), op(1); o < 8; ++o, op <<= 1) {
            if (node.activeOs & op) {
                const Node & child(node.children[o]);
                float near, far;
                if (detail::intersect(ray, invDir, child.center - child.radius, child.center + child.radius, near, far)) {
                    n += filter(child, ray, invDir, r_results);
                }
            }
        }
    }

    return n;
}
