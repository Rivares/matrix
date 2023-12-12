#include <gperftools/profiler.h>

#include <iostream>

#include <unordered_set>
#include <string>
#include <tuple>


struct NodeHash;


// Lazy initialization + Proxy
template <typename T, T value>
class Matrix
{

public:

    Matrix(): m_defValue(value)
    {   std::cout << __PRETTY_FUNCTION__ << '\n';   }
    ~Matrix()
    {   std::cout << __PRETTY_FUNCTION__ << '\n';   }


    struct Node
    {
        explicit Node(T i_ = 0, T j_ = 0, T value_ = 0) :
            m_i(i_), m_j(j_), m_value(value_), m_parent(nullptr)
        {}

        explicit Node(Matrix* parent_, T value_, T i_ = 0, T j_ = 0) :
            m_parent(parent_)
          , m_value(value_)
          , m_i(i_)
          , m_j(j_)
        {   std::cout << __PRETTY_FUNCTION__ << '\n';   }

        ~Node()
        {   std::cout << __PRETTY_FUNCTION__ << '\n';   }


        Node& operator [] (const T& j_) // optional
        {
            std::cout << __PRETTY_FUNCTION__ << '\n';

            if (j_ >= 0)
            {   m_j = j_;   }

            if (auto it = m_parent->m_data.find({m_i, m_j, m_value}); it != m_parent->m_data.end())
            {   m_value = std::get<2>(*it);  }

            return (*this);
        }

        Node& operator = (const T& val_)
        {
            std::cout << __PRETTY_FUNCTION__ << ' ' << val_ << '\n';

            if (val_ != m_parent->m_defValue)
            {
                this->m_value = val_;
                std::tuple<T, T, T> newTuple = {this->m_i, this->m_j, this->m_value};
                if (auto pairResult = m_parent->m_data.insert(newTuple);
                        pairResult.second == false)
                {
                    m_parent->m_data.erase(newTuple);
                    m_parent->m_data.insert(newTuple);
                }
            }
            else
            {
                m_parent->m_data.erase({this->m_i, this->m_j, this->m_value});
            }

            return (*this);
        }

        bool operator == (const T& rVal_) const
        {
            std::cout << __PRETTY_FUNCTION__ << '\n';

            if (auto it = m_parent->m_data.find({m_i, m_j, m_value}); it != m_parent->m_data.end())
            {   return (std::get<2>(*it) == rVal_);  }

            return false;
        }

        bool operator != (const T& rVal_) const
        {
            std::cout << __PRETTY_FUNCTION__ << '\n';

            return !((*this) == rVal_);
        }


        T m_i;
        T m_j;
        T m_value;

        Matrix* m_parent;
    };

    struct NodeHash
    {
        size_t operator () (const std::tuple<T, T, T>& node) const
        {
            return std::hash<size_t>{}(std::get<0>(node)) + std::hash<size_t>{}(std::get<1>(node));
        }
    };

    struct NodeEqual
    {
        bool operator () (const std::tuple<T, T, T>& lNode, const std::tuple<T, T, T>& rNode) const
        {
            return ( (std::get<0>(lNode) == std::get<0>(rNode)) && (std::get<1>(lNode) == std::get<1>(rNode)) );
        }
    };

    Node operator [] (const T& i_)
    {
        std::cout << __PRETTY_FUNCTION__ << '\n';

        if (i_ >= 0)
        {
            Node obj(this, m_defValue, i_);
            return obj;
        }

        return Node();
    }


    auto begin()
    {   return m_data.begin();  }

    auto cbegin() const
    {   return m_data.cbegin();  }

    auto end()
    {   return m_data.end();  }

    auto cend() const
    {   return m_data.cend();  }



    uint64_t size() const
    {   return m_data.size(); }

private:
    T m_defValue;

    std::unordered_set<std::tuple<T, T, T>, NodeHash, NodeEqual> m_data;
};

