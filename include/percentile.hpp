#pragma once

#include <cmath>
#include <vector>

namespace percentile {
    template<class T>
    class Percentile {
    public:
        void insert(const T& x) {
            insert_imp(x);
        }
        
        bool hasData() {
            return m_S.size() != 0;
        }
        
        T quantile(double percent) const {
            if (m_S.size()) {
                if (percent > 1.0)
                    percent = 1.0;
                
                float offset = ((float) m_totalCount) * percent;
                int index = floor(offset);
                
                // To improve searching when given high percentiles like 90 or 99, we can search backwards
                if (percent > 0.8) {
                    // relocate index backward
                    index = (m_totalCount - index + 1);
    
                    for (uint32_t i = m_S.size() - 1; i > 0; i--) {
                        index -= m_S[i].count;
                        if (index <= 0) {
                            return m_S[i].v;
                        }
                    }
                } else {
                    for (uint32_t i = 0; i < m_S.size(); i++) {
                        index -= m_S[i].count;
                        if (index <= 0) {
                            return m_S[i].v;
                        }
                    }
                }
                // should not happen
                assert(false);
            }
            return T{};
        }

    private:
        struct tuple {
            T v;
            uint32_t count;
            
            tuple(T v, uint32_t c)
            : v(v), count(c) {}
        };
        
        typedef std::vector<tuple> tuples_t;
        
        static bool compareFunc(const tuple& a, const tuple& b) {
            return a.v < b.v;
        };
            
        void insert_imp(const T& x) {
            tuple newvalue(x, 1);
            
            // search the first element that is not less than given value
            auto it = std::lower_bound(m_S.begin(), m_S.end(), newvalue, &Percentile::compareFunc);
            if (it != m_S.end() && it->v == x) {
                it->count++;
            } else {
                m_S.insert(it, newvalue);
            }
            m_totalCount++;
        }
        
        tuples_t m_S;
        uint32_t m_totalCount = 0;
    };
}
