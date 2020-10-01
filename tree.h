#include <vector>

template <typename Non_terminal_t, typename Terminal_t>
struct Tree
{
    Non_terminal_t non_terminal;
    Terminal_t terminal;
    Tree<Non_terminal_t, Terminal_t>* parent;
    std::vector<Tree<Non_terminal_t, Terminal_t>*> children;
    Tree() = default;
    Tree(const Non_terminal_t& data1, const Terminal_t& data2): non_terminal(data1), terminal(data2){}
    ~Tree()=default;

    void add_node(Tree<Non_terminal_t,Terminal_t>* node)
    {
        children.push_back(node);
        node->parent = this;
    }

    inline bool is_leaf () const {return children.empty(); }
};