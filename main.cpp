#include <gperftools/profiler.h>

#include <iostream>

#include <unordered_set>
#include <string>

struct Node
{
    Node(uint64_t i_, uint64_t j_, uint64_t value_) :
        i(i_), j(j_), value(value_)
    {}

    uint64_t i;
    uint64_t j;
    uint64_t value;
};

bool operator == (const Node& lNode, const Node& rNode)
{
    return ( (lNode.i == rNode.i) && (lNode.j == rNode.j) );
}

struct NodeHash
{
    size_t operator () (const Node& node) const
    {
        return std::hash<size_t>{}(node.i) + std::hash<size_t>{}(node.j);
    }
};




// Lazy initialization + Proxy
template <typename T, T value>
class Matrix
{

public:

    class RowProxy
    {
        T m_i;
        T m_j;
        T m_val;

        Matrix* m_parent = nullptr;

    public:

        RowProxy() = default;

        RowProxy(Matrix* ptr_, const T& i_) : m_parent(ptr_), m_i(i_)
        {   std::cout << __PRETTY_FUNCTION__ << '\n';   }

        ~RowProxy()
        {   std::cout << __PRETTY_FUNCTION__ << '\n';   }

        T& operator [](T j_)
        {
            std::cout << __PRETTY_FUNCTION__ << '\n';

            if (j_ >= 0)
            {
                m_j = j_;
            }

            return m_parent->getDefVal();
        }

        void setVal(const T& val_)
        {   m_val = val_;   }

        const T& getVal() const
        {   return m_val;   }
    };

    Matrix(): m_defValue(value)
    {   std::cout << __PRETTY_FUNCTION__ << '\n';   }
    ~Matrix()
    {   std::cout << __PRETTY_FUNCTION__ << '\n';   }

    RowProxy& operator [](T i_)
    {
        std::cout << __PRETTY_FUNCTION__ << '\n';

        if (i_ >= 0)
        {
            RowProxy* obj = new RowProxy(this, i_);
//            m_data.insert(obj);

            return *obj;
        }
        RowProxy dumy;
        return dumy;
    }

    T& getDefVal()
    {   return m_defValue;   }

    Matrix& operator = (const T& val_)
    {
        std::cout << __PRETTY_FUNCTION__ << ' ' << '\n';
        m_data.back()->setVal(val_);

        return *this;
    }

    bool operator == (const T& rVal_) const
    {
        std::cout << __PRETTY_FUNCTION__ << ' ' << '\n';

        return true;//(m_data.back()->getVal() == rVal_);
    }
    bool operator == (const Matrix& rVal_)
    {
        std::cout << __PRETTY_FUNCTION__ << ' ' << '\n';

        return (m_data.back()->getVal() == rVal_.getData().back()->getVal());
    }
    bool operator < (const Matrix& rVal_)
    {
        std::cout << __PRETTY_FUNCTION__ << ' ' << '\n';

        return (m_data.back()->getVal()  < rVal_.getData().back()->getVal());
    }
    bool operator <= (const Matrix& rVal_)
    {
        std::cout << __PRETTY_FUNCTION__ << ' ' << '\n';

        return (m_data.back()->getVal() <= rVal_.getData().back()->getVal());
    }

    std::unordered_set<RowProxy, std::hash<RowProxy>> getData() const
    {   return m_data; }

    uint64_t size() const
    {   return m_data.size(); }

private:
    T m_defValue;

    std::unordered_set<RowProxy, NodeHash> m_data;
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

        assert(matrix.size() == 0); // все ячейки свободны

        matrix[1][2] = 8;
        auto a = matrix[1][2];

//        assert(a == 0);
//        assert(a == -1);
        assert(matrix.size() == 2);

//        matrix[100][100] = 314;
//        assert(matrix[100][100] == 314);
//        assert(matrix.size() == 1);
//        // выведется одна строка
//        // 100100314
//        for(auto c: matrix)
//        {
//        int x;
//        int y;
//        int v;
//        std::tie(x, y, v) = c;
//        std::cout << x << y << v << std::endl;
//        }

        std::cout << "\n\nDestructions objects:\n";
    }

    std::cout << "\n";


    ProfilerStop();
    return 0;
}

