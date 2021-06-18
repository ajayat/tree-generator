#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <json/json.h>

int H; int B; // breadth and height of the tree
std::map<std::string, std::vector<std::string>> adj;
std::vector<std::vector<std::string>> tree;
std::string origin;

Json::Value read_json(const std::string& filename) {
	Json::Value edges;
	Json::Reader reader;
	std::ifstream jsonfile(filename);

	if (jsonfile) {
		reader.parse(jsonfile, edges);
		return edges;
	} else {
		throw Json::Exception("Cannot read the file");
	}
}

int breadth(const std::string& root) {
	int sum{ 0u };
	for (const std::string& elt : adj[root])
		sum += breadth(elt);

	return sum / 2 * 2 + 1;
}

int height(const std::string& root) {
	int max{ 0u };
	for (const std::string& elt : adj[root])
		max = std::max(height(elt), max);

	return max + 1;
}

void find_adj(const Json::Value& edges, const std::string& root) {
	adj[root] = edges[root].getMemberNames();

	for (const std::string& child : edges[root].getMemberNames()) {
		find_adj(edges[root], child);
	}
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

void find_coords(const std::string& root, int offset=0, int depth=0) {
	tree[depth][offset + half_breadth(root)] = root;
	div_t divresult{ div(adj[root].size(), 2) };

	for (auto i = 0; i < adj[root].size(); i++) {
		find_coords(adj[root][i], offset, depth + 1);
		offset += breadth(adj[root][i]);
		if ((i == divresult.quot - 1) && (divresult.rem == 0))
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

bool is_empty(const std::string& value) {
	return value.empty();
}

std::string vertical_sep(const std::string& node, int const l, int const c) {
	if (!node.empty()) {
		const std::vector<std::string>& brothers{ adj[parent(node)] };
		unsigned int pos{ 0 };
		while (node != brothers[pos])
			pos++;
		if (pos == 0)
			return (tree[l - 1][c].empty()) ? "┌" : "─";
		if (pos == adj[parent(node)].size() - 1)
			return "└";
		return (tree[l - 1][c].empty()) ? "├" : "┼";
	}
	auto rend{ tree[l].rend() }; 
	auto end{ tree[l].end() };
	auto prev{ std::find_if_not(tree[l].rbegin() + B - c, rend, is_empty) };
	auto after{ std::find_if_not(tree[l].begin() + c + 1, end, is_empty) };

	if (prev != rend && after != end && parent(*prev) == parent(*after))
		return (tree[l - 1][c].empty()) ? "│" : "┤";
	return " ";
}

std::string format(const std::string& elt, const int c, const int l) {
	if (c == 0)
		return (elt.empty()) ? std::string(origin.size(), ' ') : elt;

	unsigned int pad{ 0u };
	for (const auto& node : tree[c]) {
		if (!tree[c].empty())
			pad = std::max<size_t>(pad, node.size());
	}
	std::string vsep{ vertical_sep(elt, c, l)};
	if (elt.empty())
		return vsep + std::string(pad + 1, ' ');
	for (int i = 0; i < pad - elt.size(); i++)
	    vsep += "─";
	return vsep + " " + elt;
}

int main() {
	try {
		Json::Value edges{ read_json("json/edges.json") };
		origin = edges.getMemberNames()[0];
		find_adj(edges, origin);

		B = breadth(origin);
		H = height(origin);
		tree.resize(H);
		for (auto& line : tree)
			line.resize(B);
		find_coords(origin);
		
		for (auto i = 0; i < B; i++) {
			std::vector<std::string> line;
			for (auto j = 0; j < H; j++) {
				std::string elt{ format(tree[j][i], j, i) };
				std::cout << elt << " ";
			}
			std::cout << std::endl;
		}
	}
	catch (const std::string& msg) {
		std::cout << msg << std::endl;
	}

	return 0;
}
