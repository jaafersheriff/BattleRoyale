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

inline float maxCompNonInf(glm::vec3 v) {
    float temp;
    if (v.y < v.x) { temp = v.x; v.x = v.y; v.y = temp; }
    if (v.z < v.y) { temp = v.y; v.y = v.z; v.z = temp; }
    if (v.y < v.x) { temp = v.x; v.x = v.y; v.y = temp; }
    if (v.z != std::numeric_limits<float>::infinity()) return v.z;
    if (v.y != std::numeric_limits<float>::infinity()) return v.y;
    return v.x;
}

inline bool intersect(const Ray & ray, const glm::vec3 & invDir, const glm::vec3 & min, const glm::vec3 & max, float & r_near, float & r_far) {
    glm::vec3 tsLow((min - ray.pos) * invDir);
    glm::vec3 tsHigh((max - ray.pos) * invDir);

    float tMin(glm::compMax(glm::min(tsLow, tsHigh)));
    float tMax(glm::compMin(glm::max(tsLow, tsHigh)));
    if (!(tMax > 0.0f && tMax > tMin)) { // extra negation so that a NaN case returns no intersection
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
    parentO(0)
{}

template <typename T>
Octree<T>::Node::Node(const glm::vec3 & center, float radius, Node * parent, uint8_t parentO) :
    center(center),
    radius(radius),
    elements(),
    children(),
    parent(parent),
    activeOs(0),
    parentO(parentO)
{}



template <typename T>
Octree<T>::Octree(const AABox & region, float minSize) {
    // Octree must be a cube with size a power of 2 multiple of minSize
    Util::nat iSize(Util::floor(glm::max(glm::compMax(region.max - region.min) / minSize, 1.0f)));
    iSize = Util::ceil2(iSize); // round up to nearest power of 2
    m_minRadius = minSize * 0.5f;
    m_root = UniquePtr<Node>::make(region.center(), iSize * m_minRadius, nullptr, 0);
    m_rootRegion.min = m_root->center - m_root->radius;
    m_rootRegion.max = m_root->center + m_root->radius;
}

template <typename T>
bool Octree<T>::set(T e, const AABox & region) {
    auto it(m_map.find(e));
    if (it != m_map.end()) {
        Node & node(*it->second.first);
        m_map.erase(it);
        for (Util::nat i(node.elements.size() - 1); i >= 0; --i) {
            if (node.elements[i] == e) {
                node.elements.erase(node.elements.begin() + i);
                break;
            }
        }
        bool res(addUp(node, e, region));
        trim(node);
        return res;
    }
    else {
        if (detail::intersects(m_rootRegion, region)) {
            addDown(*m_root, e, region);
            return true;
        }
        return false;
    }
}

template <typename T>
bool Octree<T>::remove(T e) {
    auto it(m_map.find(e));
    if (it == m_map.end()) {
        return false;
    }

    Node & node(*it->second.first);
    for (Util::nat i(node.elements.size() - 1); i >= 0; --i) {
        if (node.elements[i] == e) {
            node.elements.erase(node.elements.begin() + i);
            break;
        }
    }

    trim(node);

    m_map.erase(it);
    return true;
}

template <typename T>
void Octree<T>::clear() {
    m_root->elements.clear();
    m_root->children.release();
    m_root->activeOs = 0;
    m_map.clear();
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
std::pair<T, Intersect> Octree<T>::filter(const Ray & ray, const std::function<Intersect(const Ray &, T)> & f) const {
    glm::vec3 absDir(glm::abs(ray.dir));
    glm::vec3 invDir, signDir;
    if (Util::isZeroAbs(absDir.x)) {
        invDir.x = Util::infinity();
        signDir.x = 0.0f;
    }
    else {
        invDir.x = 1.0f / ray.dir.x;
        signDir.x = ray.dir.x < 0.0f ? -1.0f : 1.0f;
    }
    if (Util::isZeroAbs(absDir.y)) {
        invDir.y = Util::infinity();
        signDir.y = 0.0f;
    }
    else {
        invDir.y = 1.0f / ray.dir.y;
        signDir.y = ray.dir.y < 0.0f ? -1.0f : 1.0f;
    }
    if (Util::isZeroAbs(absDir.z)) {
        invDir.z = Util::infinity();
        signDir.z = 0.0f;
    }
    else {
        invDir.z = 1.0f / ray.dir.z;
        signDir.z = ray.dir.z < 0.0f ? -1.0f : 1.0f;
    }

    float near, far;
    if (!detail::intersect(ray, invDir, m_rootRegion.min, m_rootRegion.max, near, far)) {
        return std::pair<T, Intersect>{};
    }

    // TODO: this should be fine for you guys with your old fangled 32 bits, but should make sure
    uint64_t oMap;
    if (ray.dir.z >= 0.0f) {
        if (ray.dir.y >= 0.0f) {
            if (ray.dir.x >= 0.0f)
                oMap = 0x0706050403020100ULL;
            else
                oMap = 0x0607040502030001ULL;
        }
        else {
            if (ray.dir.x >= 0.0f)
                oMap = 0x0504070601000302ULL;
            else
                oMap = 0x0405060700010203ULL;
        }
    }
    else {
        if (ray.dir.y >= 0.0f) {
            if (ray.dir.x >= 0.0f)
                oMap = 0x0302010007060504ULL;
            else
                oMap = 0x0203000106070405ULL;
        }
        else {
            if (ray.dir.x >= 0.0f)
                oMap = 0x0100030205040706ULL;
            else
                oMap = 0x0001020304050607ULL;
        }
    }

    // In case the 64 bit thing doesn't work
    /*uint8_t oMap[8]{ 0, 1, 2, 3, 4, 5, 6, 7 };
    uint8_t temp;
    if (ray.dir.z < 0.0f) {
        temp = oMap[0]; oMap[0] = oMap[4]; oMap[4] = temp;
        temp = oMap[1]; oMap[1] = oMap[5]; oMap[5] = temp;
        temp = oMap[2]; oMap[2] = oMap[6]; oMap[6] = temp;
        temp = oMap[3]; oMap[3] = oMap[7]; oMap[7] = temp;
    }
    if (ray.dir.y < 0.0f) {
        temp = oMap[0]; oMap[0] = oMap[2]; oMap[2] = temp;
        temp = oMap[1]; oMap[1] = oMap[3]; oMap[3] = temp;
        temp = oMap[4]; oMap[4] = oMap[6]; oMap[6] = temp;
        temp = oMap[5]; oMap[5] = oMap[7]; oMap[7] = temp;
    }
    if (ray.dir.x < 0.0f) {
        temp = oMap[0]; oMap[0] = oMap[1]; oMap[1] = temp;
        temp = oMap[2]; oMap[2] = oMap[3]; oMap[3] = temp;
        temp = oMap[4]; oMap[4] = oMap[5]; oMap[5] = temp;
        temp = oMap[6]; oMap[6] = oMap[7]; oMap[7] = temp;
    }*/

    std::pair<T, Intersect> res{};
    filter(*m_root, ray, f, invDir, signDir, near, far, reinterpret_cast<uint8_t *>(&oMap), res.first, res.second);
    return res;
}

template <typename T>
size_t Octree<T>::filter(T e, Vector<T> & r_results) const {
    auto it(m_map.find(e));
    if (it == m_map.end()) {
        return 0;
    }

    size_t n(0);
    Node * node(it->second.first->parent);
    while (node) {
        n += node->elements.size();
        for (T e : node->elements) {
            r_results.push_back(e);
        }
        node = node->parent;
    }

    return n + filter(*it->second.first, it->second.second, r_results);
}

template <typename T>
bool Octree<T>::addUp(Node & node, T e, const AABox & region) {
    AABox nodeRegion(node.center - node.radius, node.center + node.radius);
    if (detail::contains(nodeRegion, region)) {
        addDown(node, e, region);
        return true;
    }
    else {
        if (node.parent) {
            return addUp(*node.parent, e, region);
        }
        else {
            if (detail::intersects(nodeRegion, region)) {
                node.elements.push_back(e);
                m_map[e] = std::pair<Node *, AABox>(&node, region);
                return true;
            }
            return false;
        }
    }
}

template <typename T>
void Octree<T>::addDown(Node & node, T e, const AABox & region) {
    // The node is a leaf. Extra logic necessary
    if (!node.children) {
        // If the node is empty or at max depth, simply add to elements
        if (!node.elements.size() || Util::isLE(node.radius, m_minRadius)) {
            node.elements.push_back(e);
            m_map[e] = std::pair<Node *, AABox>(&node, region);
        }
        else {
            // If the node only has one element, it may not have been tried
            // to be put into a sub node. Try that now
            if (node.elements.size() == 1) {
                T e_(node.elements.front());
                const AABox & region_(m_map.at(e_).second);
                int o(detail::detOctant(node.center, region_));
                if (o >= 0) {
                    fragment(node);
                    addDown(node.children[o], e_, region_);
                    node.activeOs |= 1 << o;
                    node.elements.clear();       
                }
            }
            // Try to put the new element into a sub node
            int o(detail::detOctant(node.center, region));
            if (o >= 0) {
                if (!node.children) fragment(node);
                addDown(node.children[o], e, region);
                node.activeOs |= 1 << o;
            }
            else {
                node.elements.push_back(e);
                m_map[e] = std::pair<Node *, AABox>(&node, region);
            }
        }
    }
    // The typical case where the node is not a leaf
    else {
        int o(detail::detOctant(node.center, region));
        if (o >= 0) {
            addDown(node.children[o], e, region);
            node.activeOs |= 1 << o;
        }
        else {
            node.elements.push_back(e);
            m_map[e] = std::pair<Node *, AABox>(&node, region);
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
        node.children[o].parentO = o;
    }
}

template <typename T>
void Octree<T>::trim(Node & node_) {
    Node * node(&node_);
    while (node) {
        if (node->elements.size() || node->children || !node->parent) {
            return;
        }

        node->parent->activeOs &= ~(1 << node->parentO);
        if (!node->parent->activeOs) {
            node->parent->children.release();
        }

        node = node->parent;
    }
}

template <typename T>
size_t Octree<T>::filter(const Node & node, const std::function<bool(const glm::vec3 &, float)> & f, Vector<T> & r_results) const {
    size_t n(node.elements.size());
    for (T e : node.elements) {
        r_results.push_back(e);
    }
    if (node.children) {
        for (int o(0); o < 8; ++o) {
            if (node.activeOs & (1 << o) && f(node.children[o].center, node.children[o].radius)) {
                n += filter(node.children[o], f, r_results);
            }
        }
    }
    return n;
}

template <typename T>
size_t Octree<T>::filter(const Node & node, const AABox & region, Vector<T> & r_results) const {
    size_t n(node.elements.size());    
    for (T e : node.elements) {
        r_results.push_back(e);
    }

    if (node.children) {
        int possible(node.activeOs);
        if (region.max.z <= node.center.z) possible &= 0x0F;
        if (region.min.z >= node.center.z) possible &= 0xF0;
        if (region.max.y <= node.center.y) possible &= 0x33;
        if (region.min.y >= node.center.y) possible &= 0xCC;
        if (region.max.x <= node.center.x) possible &= 0x55;
        if (region.min.x >= node.center.x) possible &= 0xAA;
        for (int o(0); o < 8; ++o) {
            if (possible & (1 << o)) {
                n += filter(node.children[o], region, r_results);
            }
        }
    }

    return n;
}

template <typename T>
size_t Octree<T>::filter(const Node & node, const Ray & ray, const glm::vec3 & invDir, Vector<T> & r_results) const {
    size_t n(node.elements.size());    
    for (const T & e : node.elements) {
        r_results.push_back(e);
    }
    
    if (node.children) {
        for (int o(0); o < 8; ++o) {
            if (node.activeOs & (1 << o)) {
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

template <typename T>
void Octree<T>::filter(const Node & node, const Ray & ray, const std::function<Intersect(const Ray &, T)> & f, const glm::vec3 & invDir, const glm::vec3 & signDir, float near_, float far_, const uint8_t * oMap, T & r_elem, Intersect & r_inter) const {
    for (T e : node.elements) {
        Intersect potential(f(ray, e));
        if (potential.dist < r_inter.dist) {
            r_inter = potential;
            r_elem = e;
        }
    }
    
    if (!node.children) {
        return;
    }

    // Determine starting octant, near, and far
    int o(0);
    glm::vec3 farCorner(node.center);
    if ((ray.pos.z - node.center.z) * signDir.z >= 0.0f) { o |= 4; farCorner.z += node.radius * signDir.z; }
    if ((ray.pos.y - node.center.y) * signDir.y >= 0.0f) { o |= 2; farCorner.y += node.radius * signDir.y; }
    if ((ray.pos.x - node.center.x) * signDir.x >= 0.0f) { o |= 1; farCorner.x += node.radius * signDir.x; }
    float near(near_), far(glm::compMin((farCorner - ray.pos) * invDir));
    if (o) {
        glm::vec3 nearCorner(farCorner - node.radius * signDir);
        near = detail::maxCompNonInf((nearCorner - ray.pos) * invDir);
    }
    
    // Follow octant route. At most 4 can be visited
    int oCount(0);
    while (true) {
        if (near >= r_inter.dist) {
            break; // A closer intersection has already been found. No need to continue
        }

        if (node.activeOs & (1 << oMap[o])) {
            Intersect potential;
            T elem;
            filter(node.children[oMap[o]], ray, f, invDir, signDir, near, far, oMap, elem, potential);
            if (potential.dist < r_inter.dist) {
                r_inter = potential;
                r_elem = elem;
                break;
            }
        }

        // No more than 4 octants can ever be intersected by one ray
        if (o == 7 || ++oCount >= 4) {
            break;
        }

        // Progress to next octant
        near = far;
        glm::vec3 ts((farCorner - ray.pos) * invDir);
        if (ts.x <= ts.y && ts.x <= ts.z) {
            if (o & 1) {
                break; // Escaped prematurely in x direction
            }
            o |= 1;
            far = ts.x;
            farCorner.x += node.radius * signDir.x;
        }
        else if (ts.y <= ts.z) {
            if (o & 2) {
                break; // Escaped prematurely in y direction
            }
            o |= 2;
            far = ts.y;
            farCorner.y += node.radius * signDir.y;
        }
        else {
            if (o & 4) {
                break; // Escaped prematurely in z direction
            }
            o |= 4;
            far = ts.z;
            farCorner.z += node.radius * signDir.z;
        }
    }
}
