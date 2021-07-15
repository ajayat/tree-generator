#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <exception>
#include <json.hpp>
#include <filesystem>

using ordered_json = nlohmann::basic_json<nlohmann::ordered_map>;
typedef std::map<const std::string, std::vector<std::string>> Dict;
typedef unsigned int uint;

class Tree {
public:
    explicit Tree(Dict& adjacences): adj(adjacences) {}

    void show(const std::string& root) {
        H = height(root); B = breadth(root); origin = root;
        tree.resize(H);
        for (auto& line : tree)
            line.resize(B);
        fill(root);

        for (auto i = 0u; i < B; i++) {
            std::vector<std::string> line;
            for (auto j = 0u; j < H; j++) {
                std::string elt{ format(tree[j][i], j, i) };
                std::cout << elt << " ";
            }
            std::cout << std::endl;
        }
    }

private:
    std::string origin;
    std::vector<std::vector<std::string>> tree;
    std::map<const std::string, std::vector<std::string>> adj;
    uint B{ 0 }, H{ 0 }; // breadth of tree

    unsigned int breadth(const std::string& root) {
        unsigned int sum{ 0u };
        for (const std::string& elt : adj[root])
            sum += breadth(elt);

        return sum / 2 * 2 + 1;
    }

    unsigned int height(const std::string& root) {
        unsigned int m{ 0u };
        for (const std::string& elt : adj[root])
            m = std::max(height(elt), m);

        return m + 1;
    }

    unsigned int half_breadth(const std::string& root) {
        auto half{ adj[root].size() / 2 };
        auto height{ 0u };

        for (auto i = 0; i < half; i++)
            height += breadth(adj[root][i]);

        if (adj[root].size() & 1)
            height += half_breadth(adj[root][half]);

        return height;
    }

    void fill(const std::string& root, uint offset=0, int depth=0) {
        tree[depth][offset + half_breadth(root)] = root;
        auto quotient{ adj[root].size() / 2};
        auto remainder{ adj[root].size() & 1};

        for (auto i = 0; i < adj[root].size(); i++) {
            fill(adj[root][i], offset, depth + 1);
            offset += breadth(adj[root][i]);
            if ((i == quotient - 1) && (remainder == 0))
                offset += 1;
        }
    }
    std::string parent(const std::string& node) {
        for (const auto& elt : adj) {
            const auto& ch{ elt.second }; // childs
            if (!ch.empty() && std::find(ch.begin(), ch.end(), node) != ch.end())
                return elt.first;
        }
        throw std::exception("Couldn't find parent");
    }

    static bool is_empty(const std::string& value) {
        return value.empty();
    }

    char vertical_sep(const std::string& node, const uint l, const uint c) {
        if (!node.empty()) {
            const std::vector<std::string>& brothers{ adj[parent(node)] };
            uint pos{ 0 };
            while (node != brothers[pos])
                pos++;
            if (pos == 0)
                return (tree[l - 1][c].empty()) ? char(218) : char(196);
            if (pos == adj[parent(node)].size() - 1)
                return char(192);
            return (tree[l - 1][c].empty()) ? char(195) : char(197);
        }
        auto rend{ tree[l].rend() };
        auto end{ tree[l].end() };
        auto prev{ std::find_if_not(tree[l].rbegin() + B - c, rend, is_empty) };
        auto after{ std::find_if_not(tree[l].begin() + c + 1, end, is_empty) };

        if (prev != rend && after != end && parent(*prev) == parent(*after))
            return (tree[l - 1][c].empty()) ? char(179): char(180);
        return ' ';
    }

    std::string format(const std::string& elt, const uint c, const uint l) {
        if (c == 0)
            return (elt.empty()) ? std::string(origin.size(), ' ') : elt;

        uint pad{ 0u };
        for (const auto& node : tree[c]) {
            if (node.size() > pad)
                pad = int(node.size());
        }
        std::string vsep{ vertical_sep(elt, c, l)};
        if (elt.empty())
            return vsep + std::string(pad + 1, ' ');
        for (int i = 0; i < pad - elt.size(); i++)
            vsep += char(196);
        return vsep + " " + elt;
    }
};

ordered_json read_json(const std::string& filename) {
    std::ifstream jsonfile(filename);
    ordered_json edges;

    if (jsonfile) {
        jsonfile >> edges;
        return edges;
    } else {
        throw std::exception("Cannot read the file");
    }
}

void adj_from_json(const ordered_json& edges, const std::string& root, Dict& adj) {

    for (const auto& item : edges[root].items()) {
        adj[root].emplace_back(item.key());
        adj_from_json(edges[root], item.key(), adj);
    }
}

void adj_from_directory(const std::filesystem::path& root, Dict& adj) {
    for (const auto& p: std::filesystem::recursive_directory_iterator(root))
        adj[p.path().parent_path().filename().string()]
            .emplace_back(p.path().filename().string());
}

int main() {
    try {
        ordered_json edges{ read_json("json/edges.json") };
        const auto& root{ edges.begin().key() };
        Dict adjacences;

        // Or recursive list of files in given directory:
        // adj_from_directory(std::filesystem::current_path(), adjacences);
        // Or from a json
        adj_from_json(edges, root, adjacences);

        // Show the tree in output console
        Tree(adjacences).show(root);
	}
    catch (const std::string& msg) {
        std::cout << msg << std::endl;
	}
	return 0;
}
