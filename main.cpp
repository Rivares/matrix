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


        Node& operator [] (T j_) // optional
        {
            std::cout << __PRETTY_FUNCTION__ << '\n';

            if (j_ >= 0)
            {   m_j = j_;   }

            return (*this);
        }

        void operator = (const T& val_)
        {
            std::cout << __PRETTY_FUNCTION__ << ' ' << val_ << '\n';

            if (val_ != m_parent->m_defValue)
            {
                this->m_value = val_;
                m_parent->m_data.insert(*this);
                m_parent->m_data1.insert({this->m_i, this->m_j, this->m_value, this->m_parent});
            }
            else
            {
                m_parent->m_data.erase(*this);
                m_parent->m_data1.erase({this->m_i, this->m_j, this->m_value, this->m_parent});
            }
        }

        bool operator == (const T& rVal_) const
        {
            std::cout << __PRETTY_FUNCTION__ << '\n';

            Node findingNode {m_i, m_j, m_value};

//            if (auto it = m_parent->m_data.find(findingNode); it != m_parent->m_data.end())
//            {   return ((*it).m_value == rVal_);  }
            if (auto it = m_parent->m_data1.find({m_i, m_j, m_value, m_parent}); it != m_parent->m_data1.end())
            {   return (std::get<2>((*it)) == rVal_);  }

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
        size_t operator () (const Node& node) const
        {
            return std::hash<size_t>{}(node.m_i) + std::hash<size_t>{}(node.m_j);
        }
    };

    struct NodeEqual
    {
        bool operator () (const Node& lNode, const Node& rNode) const
        {
            return ( (lNode.m_i == rNode.m_i) && (lNode.m_j == rNode.m_j) );
        }
    };

    struct Node1Hash
    {
        size_t operator () (const std::tuple<T, T, T, Matrix*>& node) const
        {
            return std::hash<size_t>{}(std::get<0>(node)) + std::hash<size_t>{}(std::get<1>(node));
        }
    };

    struct Node1Equal
    {
        bool operator () (const std::tuple<T, T, T, Matrix*>& lNode, const std::tuple<T, T, T, Matrix*>& rNode) const
        {
            return ( (std::get<0>(lNode) == std::get<0>(rNode)) && (std::get<1>(lNode) == std::get<1>(rNode)) );
        }
    };

    Node operator [] (T i_)
    {
        std::cout << __PRETTY_FUNCTION__ << '\n';

        if (i_ >= 0)
        {
            return Node(this, m_defValue, i_);
        }

        return Node();
    }

    // TODO Impl iterators
    auto begin()
    {   return m_data1.begin();  }

    auto cbegin() const
    {   return m_data1.cbegin();  }

    auto end()
    {   return m_data1.end();  }

    auto cend() const
    {   return m_data1.cend();  }



    uint64_t size() const
    {   return m_data1.size(); }

private:
    T m_defValue;

    std::unordered_set<Node, NodeHash, NodeEqual> m_data;
    std::unordered_set<std::tuple<T, T, T, Matrix*>, Node1Hash, Node1Equal> m_data1;
};





#include <cassert>

int main()
{
    ProfilerStart("matrix.prof");

///   Спроектировать 2-мерную разреженную бесконечную матрицу, заполненную значениями по
///   умолчанию. Матрица должна хранить только занятые элементы - значения которых хотя бы раз
///   присваивались. Присвоение в ячейку значения по умолчанию освобождает ячейку.
///   Необходимо уметь отвечать на вопрос - сколько ячеек реально занято?
///   Необходимо уметь проходить по всем занятым ячейкам. Порядок не имеет значения. Возвращается
///   позиция ячейки и ее значение.
///   При чтении элемента из свободной ячейки возвращать значение по умолчанию.
/*!
    При запуске программы необходимо создать матрицу с пустым значением 0, заполнить главную
    диагональ матрицы (от [0,0] до [9,9]) значениями от 0 до 9.
    Второстепенную диагональ (от [0,9] до [9,0]) значениями от 9 до 0.
    Необходимо вывести фрагмент матрицы от [1,1] до [8,8]. Между столбцами пробел. Каждая строка
    матрицы на новой строке консоли.
    Вывести количество занятых ячеек.
    Вывести все занятые ячейки вместе со своими позициями.
    Опционально реализовать N-мерную матрицу.
    Опционально реализовать каноническую форму оператора `=`, допускающую выражения
    `((matrix[100][100] = 314) = 0) = 217`
*/

    {
        std::cout << "Constructions project of objects:\n\n";

        // бесконечная матрица int заполнена значениями -1
        Matrix<int, -1> matrix;


        std::cout << "\nWork part:\n\n";

        assert(matrix.size() == 0); // все ячейки свободны

        matrix[1][2] = 8;
        matrix[1][2] = 8;

        assert(matrix.size() == 1);
        assert(matrix[1][2] == 8);

        auto a = matrix[1][2];

        assert(a == 8);
        assert(a != -1);
        assert(matrix.size() == 1);

        matrix[100][100] = 314;
        assert(matrix[100][100] == 314);
        assert(matrix.size() == 2);


//        // выведется одна строка
//        // 100100314
        for(auto item: matrix)
        {
            int x;
            int y;
            int v;
            Matrix<int, -1>* vv;
            std::tie(x, y, v, vv) = item;
            std::cout << x << y << v << std::endl;
//            auto [xx, yy, vv, ww] = item;
//            std::cout << xx << yy << vv << std::endl;
        }

        std::cout << "\n\nDestructions objects:\n";
    }

    std::cout << "\n";


    ProfilerStop();
    return 0;
}


